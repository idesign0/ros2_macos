import numpy as np
import uuid
import math
from scipy.optimize import linear_sum_assignment

from nav2_dynamic_msgs.msg import Obstacle, ObstacleArray
from visualization_msgs.msg import Marker, MarkerArray

import rclpy
import copy
from rclpy.node import Node
import colorsys
from kf_hungarian_tracker.obstacle_class import ObstacleClass

from tf2_ros import TransformException
from tf2_ros.buffer import Buffer
from tf2_ros.transform_listener import TransformListener
from tf2_geometry_msgs import do_transform_point, do_transform_vector3
from geometry_msgs.msg import PointStamped, Vector3Stamped


class KFHungarianTracker(Node):
    """Use Kalman Fiter and Hungarian algorithm to track multiple dynamic obstacles

    Use Hungarian algorithm to match presenting obstacles with new detection and maintain a kalman filter for each obstacle.
    spawn ObstacleClass when new obstacles come and delete when they disappear for certain number of frames

    Attributes:
        obstacle_list: a list of ObstacleClass that currently present in the scene
        sec, nanosec: timing from sensor msg
        detection_sub: subscrib detection result from detection node
        tracker_obstacle_pub: publish tracking obstacles with ObstacleArray
        tracker_pose_pub: publish tracking obstacles with PoseArray, for rviz visualization
    """

    def __init__(self):
        """initialize attributes and setup subscriber and publisher"""

        super().__init__("kf_hungarian_node")
        self.declare_parameters(
            namespace="",
            parameters=[
                ("global_frame", "camera_link"),
                ("process_noise_cov", [2.0, 2.0, 0.5]),
                ("top_down", False),
                ("death_threshold", 3),
                ("measurement_noise_cov", [1.0, 1.0, 1.0]),
                ("error_cov_post", [1.0, 1.0, 1.0, 10.0, 10.0, 10.0]),
                ("vel_filter", [0.1, 2.0]),
                ("height_filter", [-2.0, 2.0]),
                ("cost_filter", 1.0),
                ("transform_to_global_frame", True),
                ("infer_orientation_from_velocity", True),
            ],
        )
        self.global_frame = self.get_parameter("global_frame")._value
        self.death_threshold = self.get_parameter("death_threshold")._value
        self.measurement_noise_cov = self.get_parameter("measurement_noise_cov")._value
        self.error_cov_post = self.get_parameter("error_cov_post")._value
        self.process_noise_cov = self.get_parameter("process_noise_cov")._value
        self.vel_filter = self.get_parameter("vel_filter")._value
        self.height_filter = self.get_parameter("height_filter")._value
        self.top_down = self.get_parameter("top_down")._value
        self.cost_filter = self.get_parameter("cost_filter")._value
        self.transform_to_global_frame = self.get_parameter(
            "transform_to_global_frame"
        )._value
        self.infer_orientation_from_velocity = self.get_parameter(
            "infer_orientation_from_velocity"
        )._value

        self.obstacle_list = []
        self.sec = 0
        self.nanosec = 0

        # subscribe to detector
        self.detection_sub = self.create_subscription(
            ObstacleArray, "detection", self.callback, 10
        )

        # publisher for tracking result
        self.tracker_obstacle_pub = self.create_publisher(ObstacleArray, "tracking", 10)
        self.tracker_marker_pub = self.create_publisher(
            MarkerArray, "tracking_marker", 10
        )

        # setup tf related
        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer, self)

    def callback(self, msg):
        """callback function for detection result"""

        # update delta time
        dt = (msg.header.stamp.sec - self.sec) + (
            msg.header.stamp.nanosec - self.nanosec
        ) / 1e9
        self.sec = msg.header.stamp.sec
        self.nanosec = msg.header.stamp.nanosec

        # get detection
        detections = msg.obstacles
        num_of_detect = len(detections)
        num_of_obstacle = len(self.obstacle_list)

        # kalman predict
        for obj in self.obstacle_list:
            obj.predict(dt)

        if (self.transform_to_global_frame) and (self.global_frame is not None):
            try:
                trans = self.tf_buffer.lookup_transform(
                    self.global_frame, msg.header.frame_id, rclpy.time.Time()
                )
                msg.header.frame_id = self.global_frame
                # do_transform_vector3(vector, trans) resets trans.transform.translation
                # values to 0.0, so we need to preserve them for future usage in the loop below
                translation_backup_x = trans.transform.translation.x
                translation_backup_y = trans.transform.translation.y
                translation_backup_z = trans.transform.translation.z
                for i in range(len(detections)):
                    trans.transform.translation.x = translation_backup_x
                    trans.transform.translation.y = translation_backup_y
                    trans.transform.translation.z = translation_backup_z
                    # transform position (point)
                    p = PointStamped()
                    p.point = detections[i].position
                    detections[i].position = do_transform_point(p, trans).point
                    # transform velocity (vector3)
                    v = Vector3Stamped()
                    v.vector = detections[i].velocity
                    detections[i].velocity = do_transform_vector3(v, trans).vector
                    # transform size (vector3)
                    s = Vector3Stamped()
                    s.vector = detections[i].size
                    detections[i].size = do_transform_vector3(s, trans).vector

            except TransformException as ex:
                self.get_logger().error(
                    "fail to get tf from {} to {}: {}".format(
                        msg.header.frame_id, self.global_frame, ex
                    )
                )
                return

        # hungarian matching
        cost = np.zeros((num_of_obstacle, num_of_detect))
        for i in range(num_of_obstacle):
            for j in range(num_of_detect):
                cost[i, j] = self.obstacle_list[i].distance(detections[j])
        obs_ind, det_ind = linear_sum_assignment(cost)

        # filter assignment according to cost
        new_obs_ind = []
        new_det_ind = []
        for o, d in zip(obs_ind, det_ind):
            if cost[o, d] < self.cost_filter:
                new_obs_ind.append(o)
                new_det_ind.append(d)
        obs_ind = new_obs_ind
        det_ind = new_det_ind

        # kalman update
        for o, d in zip(obs_ind, det_ind):
            self.obstacle_list[o].correct(detections[d])

        # birth of new detection obstacles and death of disappear obstacle
        self.birth(det_ind, num_of_detect, detections)
        dead_object_list = self.death(obs_ind, num_of_obstacle)

        # apply velocity and height filter
        filtered_obstacle_list = []
        for obs in self.obstacle_list:
            obs_vel = np.linalg.norm(
                np.array([obs.msg.velocity.x, obs.msg.velocity.y, obs.msg.velocity.z])
            )
            obs_height = obs.msg.position.z
            if (
                obs_vel > self.vel_filter[0]
                and obs_vel < self.vel_filter[1]
                and obs_height > self.height_filter[0]
                and obs_height < self.height_filter[1]
            ):
                filtered_obstacle_list.append(obs)

        # construct ObstacleArray
        if self.tracker_obstacle_pub.get_subscription_count() > 0:
            obstacle_array = ObstacleArray()
            obstacle_array.header = msg.header
            track_list = []

            for obs in filtered_obstacle_list:
                track_list.append(obs.msg)

            obstacle_array.obstacles = track_list
            self.tracker_obstacle_pub.publish(obstacle_array)

        # rviz visualization
        if self.tracker_marker_pub.get_subscription_count() > 0:
            marker_array = MarkerArray()
            marker_list = []
            # add current active obstacles
            for obs in filtered_obstacle_list:
                obstacle_uuid = uuid.UUID(bytes=bytes(obs.msg.uuid.uuid))
                (r, g, b) = colorsys.hsv_to_rgb(
                    obstacle_uuid.int % 360 / 360.0, 1.0, 1.0
                )  # encode id with rgb color

                # make a cube
                marker = Marker()
                marker.header = msg.header
                marker.ns = str(obstacle_uuid)
                marker.id = 0
                marker.type = 1  # CUBE
                marker.action = 0
                marker.color.a = 0.5
                marker.color.r = r
                marker.color.g = g
                marker.color.b = b
                marker.pose.position = obs.msg.position
                angle = np.arctan2(obs.msg.velocity.y, obs.msg.velocity.x)
                if self.infer_orientation_from_velocity:
                    marker.pose.orientation.z = float(np.sin(angle / 2))
                    marker.pose.orientation.w = float(np.cos(angle / 2))
                else:
                    marker.pose.orientation.z = 0.0
                    marker.pose.orientation.w = 1.0

                marker.scale = obs.msg.size
                marker_list.append(marker)
                # make an arrow
                arrow = Marker()
                arrow.header = msg.header
                arrow.ns = str(obstacle_uuid)
                arrow.id = 1
                arrow.type = 0
                arrow.action = 0
                arrow.color.a = 1.0
                arrow.color.r = r
                arrow.color.g = g
                arrow.color.b = b
                arrow.pose.position = obs.msg.position
                arrow.pose.orientation.z = float(np.sin(angle / 2))
                arrow.pose.orientation.w = float(np.cos(angle / 2))
                arrow.scale.x = np.linalg.norm(
                    [obs.msg.velocity.x, obs.msg.velocity.y, obs.msg.velocity.z]
                )
                arrow.scale.y = 0.05
                arrow.scale.z = 0.05
                marker_list.append(arrow)

            # add dead obstacles to delete in rviz
            for dead_uuid in dead_object_list:
                marker = Marker()
                marker.header = msg.header
                marker.ns = str(dead_uuid)
                marker.id = 0
                marker.action = 2  # delete
                arrow = Marker()
                arrow.header = msg.header
                arrow.ns = str(dead_uuid)
                arrow.id = 1
                arrow.action = 2
                marker_list.append(marker)
                marker_list.append(arrow)
            marker_array.markers = marker_list
            self.tracker_marker_pub.publish(marker_array)

    def birth(self, det_ind, num_of_detect, detections):
        """generate new ObstacleClass for detections that do not match any in current obstacle list"""
        for det in range(num_of_detect):
            if det not in det_ind:
                obstacle = ObstacleClass(
                    detections[det],
                    self.top_down,
                    self.measurement_noise_cov,
                    self.error_cov_post,
                    self.process_noise_cov,
                )
                self.obstacle_list.append(obstacle)

    def death(self, obj_ind, num_of_obstacle):
        """count obstacles' missing frames and delete when reach threshold"""
        new_object_list = []
        dead_object_list = []
        # for previous obstacles
        for obs in range(num_of_obstacle):
            if obs not in obj_ind:
                self.obstacle_list[obs].dying += 1
            else:
                self.obstacle_list[obs].dying = 0

            if self.obstacle_list[obs].dying < self.death_threshold:
                new_object_list.append(self.obstacle_list[obs])
            else:
                obstacle_uuid = uuid.UUID(
                    bytes=bytes(self.obstacle_list[obs].msg.uuid.uuid)
                )
                dead_object_list.append(obstacle_uuid)

        # add newly born obstacles
        for obs in range(num_of_obstacle, len(self.obstacle_list)):
            new_object_list.append(self.obstacle_list[obs])

        self.obstacle_list = new_object_list
        return dead_object_list


def main(args=None):
    rclpy.init(args=args)

    node = KFHungarianTracker()
    node.get_logger().info("start spining tracker node...")

    rclpy.spin(node)

    rclpy.shutdown()


if __name__ == "__main__":
    main()
