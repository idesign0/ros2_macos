std_msgs/Header header

# This message defines lowest level commands to be sent to the actuator(s).

float64[] position		# Position of the actuators in [rad] for angular actuators
						# and [m] for linear actuators.

float64[] velocity		# Velocities of the actuators in [rad/s] for angular actuators
						# and [m/s] for linear actuators.

float64[] normalized	# Everything that does not fit the above,
						# normalized between [-1 ... 1].
