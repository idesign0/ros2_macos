import numpy as np
import cv2
import uuid
from unique_identifier_msgs.msg import UUID


class ObstacleClass:
    """wrap a kalman filter and extra information for one single obstacle

    State space is 3D (x, y, z) by default, if you want to work on 2D (for example top-down view), you can simply make z a constant value and independent of x, y.

    Arrtibutes:
        position: 3d position of center point, numpy array with shape (3, 1)
        velocity: 3d velocity of center point, numpy array with shape (3, 1)
        kalman: cv2.KalmanFilter
        dying: count missing frames for this obstacle, if reach threshold, delete this obstacle
    """

    def __init__(
        self,
        obstacle_msg,
        top_down,
        measurement_noise_cov,
        error_cov_post,
        process_noise_cov,
    ):
        """Initialize with an Obstacle msg and an assigned id"""
        self.msg = obstacle_msg

        uuid_msg = UUID()
        uuid_msg.uuid = list(uuid.uuid4().bytes)
        self.msg.uuid = uuid_msg

        position = np.array(
            [
                [
                    obstacle_msg.position.x,
                    obstacle_msg.position.y,
                    obstacle_msg.position.z,
                ]
            ]
        ).T  # shape 3*1
        velocity = np.array(
            [
                [
                    obstacle_msg.velocity.x,
                    obstacle_msg.velocity.y,
                    obstacle_msg.velocity.z,
                ]
            ]
        ).T

        # check aganist state space dimension, top_down or not
        if top_down:
            measurement_noise_cov[2] = 0.0
            error_cov_post[2] = 0.0
            error_cov_post[5] = 0.0
            process_noise_cov[2] = 0.0

        # setup kalman filter
        self.kalman = cv2.KalmanFilter(
            6, 3
        )  # 3d by default, 6d state space and 3d observation space
        self.kalman.measurementMatrix = np.array(
            [[1, 0, 0, 0, 0, 0], [0, 1, 0, 0, 0, 0], [0, 0, 1, 0, 0, 0]], np.float32
        )
        self.kalman.measurementNoiseCov = np.diag(measurement_noise_cov).astype(
            np.float32
        )
        self.kalman.statePost = np.concatenate([position, velocity]).astype(np.float32)
        self.kalman.errorCovPost = np.diag(error_cov_post).astype(np.float32)

        self.dying = 0
        self.top_down = top_down
        self.process_noise_cov = process_noise_cov

    def predict(self, dt):
        """update F and Q matrices, call KalmanFilter.predict and store position and velocity"""

        # construct new transition matrix
        """
        F = 1, 0, 0, dt, 0,  0
            0, 1, 0, 0,  dt, 0
            0, 0, 1, 0,  0,  dt
            0, 0, 0, 1,  0,  0
            0, 0, 0, 0,  1,  0
            0, 0, 0, 0,  0,  1
        """
        F = np.eye(6).astype(np.float32)
        F[0, 3] = dt
        F[1, 4] = dt
        F[2, 5] = dt

        # construct new process conv matrix
        """assume constant velocity, and obstacle's acceleration has noise in x, y, z direction ax, ay, az
        Q = dt4*ax/4, 0,        0,        dt3*ax/2, 0,        0
            0,        dt4*ay/4, 0,        0,        dt3*ay/2, 0
            0,        0,        dt4*az/4, 0,        0,        dt3*az/2
            dt3*ax/2, 0,        0,        dt2*ax,   0,        0
            0,        dt3*ay/2, 0,        0,        dt2*ay,   0
            0,        0,        dt3*az/2, 0,        0,        dt2*az
        """
        dt2 = dt**2
        dt3 = dt * dt2
        dt4 = dt2**2

        Q = np.array([[dt4*self.process_noise_cov[0]/4, 0, 0, dt3*self.process_noise_cov[0]/2, 0, 0],
                      [0, dt4*self.process_noise_cov[1]/4, 0, 0, dt3*self.process_noise_cov[1]/2, 0],
                      [0, 0, dt4*self.process_noise_cov[2]/4, 0, 0, dt3*self.process_noise_cov[2]/2],
                      [dt3*self.process_noise_cov[0]/2, 0, 0, dt2*self.process_noise_cov[0], 0, 0],
                      [0, dt3*self.process_noise_cov[1]/2, 0, 0, dt2*self.process_noise_cov[1], 0],
                      [0, 0, dt3*self.process_noise_cov[2]/2, 0, 0, dt2*self.process_noise_cov[2]]]).astype(np.float32)

        self.kalman.transitionMatrix = F
        self.kalman.processNoiseCov = Q
        self.kalman.predict()
        self.msg.position.x = float(self.kalman.statePre[0][0])
        self.msg.position.y = float(self.kalman.statePre[1][0])
        self.msg.position.z = float(self.kalman.statePre[2][0])
        self.msg.velocity.x = float(self.kalman.statePre[3][0])
        self.msg.velocity.y = float(self.kalman.statePre[4][0])
        self.msg.velocity.z = float(self.kalman.statePre[5][0])

    def correct(self, detect_msg):
        """extract position as measurement and update KalmanFilter"""
        if self.top_down:
            detect_msg.position.z = 0.0
        measurement = np.array(
            [[detect_msg.position.x, detect_msg.position.y, detect_msg.position.z]]
        ).T.astype(np.float32)
        self.kalman.correct(measurement)
        self.msg.position.x = float(self.kalman.statePost[0][0])
        self.msg.position.y = float(self.kalman.statePost[1][0])
        self.msg.position.z = float(self.kalman.statePost[2][0])
        self.msg.velocity.x = float(self.kalman.statePost[3][0])
        self.msg.velocity.y = float(self.kalman.statePost[4][0])
        self.msg.velocity.z = float(self.kalman.statePost[5][0])
        self.msg.size = detect_msg.size
        self.msg.position_covariance = detect_msg.position_covariance
        self.msg.velocity_covariance = detect_msg.velocity_covariance

    def distance(self, other_msg):
        """measurement distance between two obstacles, dy default it's Euler distance between centers
        you can extent the Obstacle msg to include more features like class or uncertainty and include in the distance function"""
        position = np.array(
            [[self.msg.position.x, self.msg.position.y, self.msg.position.z]]
        ).T
        other_position = np.array(
            [[other_msg.position.x, other_msg.position.y, other_msg.position.z]]
        ).T
        return np.linalg.norm(position - other_position)
