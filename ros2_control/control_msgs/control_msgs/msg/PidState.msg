std_msgs/Header header
builtin_interfaces/Duration timestep

# error = target - state
float64 error
# derivative of error
float64 error_dot

# equals error
float64 p_error
# weighted integral of error
float64 i_error
# equals derivative of error
float64 d_error

# proportional gain
float64 p_term
# integral gain
float64 i_term
# derivative gain
float64 d_term
# upper integral clamp.
float64 i_max
# lower integral clamp.
float64 i_min

# output of the PID controller
float64 output
