# kf_hungarian_tracker

This package implememts a multi-object tracker with Kalman filter and Hungarian algorithm. 
Hungarian algorithm is used to associate detection and previously known objects. 
For each object, a Kalman filter is maintained to track the position and velocity. 

### Hungarian algorithm

[Hungarian algorithm](https://en.wikipedia.org/wiki/Hungarian_algorithm) is a combinatorial optimization algorithm that solves the assignment problem in polynomial time. 
Here we use an implementation from `scipy.optimize.linear_sum_assignment`. The default cost function is Euclidean distance between two object's center. 
More cost functions like IoU of bounding boxes could be used.

### Parameters

Parameters can be set in `config/kf_hungarian.yaml`. For more information on parameters for Kalman filter, check out [KalmanFilter](https://docs.opencv.org/master/dd/d6a/classcv_1_1KalmanFilter.html) from OpenCV. 

| parameters       | Meaning        | Default |
| ---------------- | ------------- | ------- |
| global_frame     | transform from pointcloud frame to global frame,<br>None means message frame is global  | camera_link   |
| death_threshold  | maximum missing frames before deleting an obstacle  | 3 |
| top_down | whether project 3D points on ground plane, set to do top-down tracking | False |
| measurement_noise_cov | measurement noise for Kalman filter [x,y,z] | [1., 1., 1.] |
| error_cov_post | initial posteriori error estimate covariance matrix [x,y,z,vx,vy,vz] | [1., 1., 1., 10., 10., 10.] |
| process_noise_cov | model process noise covariance with estimated [acceleration](https://github.com/tony23545/navigation2_dynamic/blob/master/kf_hungarian_tracker/kf_hungarian_tracker/obstacle_class.py#L59) [ax,ay,az] | [2., 2., 0.5] |
| vel_filter | minimum and maximum velocity to filter obstacles [min,max] (m/s) | [0.1, 2.0] |
| height_filter | minimum and maximum height (z) to filter obstacles [min,max] (m) | [-2.0, 2.0] | 
| cost_filter | filter Hungarian assignments with cost greater than threshold (unit is `m` for Euclidean cost function) | 1.0 |
* The default units are `m` and `m/s`, could be changed according to the detection.
