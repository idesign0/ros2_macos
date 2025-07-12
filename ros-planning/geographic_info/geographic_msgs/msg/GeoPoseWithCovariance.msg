# Geographic pose, using the WGS 84 reference ellipsoid.
#
# Orientation uses the East-North-Up (ENU) frame of reference.
# (But, what about singularities at the poles?)

GeoPose pose

# Row-major representation of the 6x6 covariance matrix
# The orientation parameters use a fixed-axis representation.
# In order, the parameters are:
# (Lat, Lon, Alt, rotation about E (East) axis, rotation about N (North) axis, rotation about U (Up) axis)
float64[36] covariance