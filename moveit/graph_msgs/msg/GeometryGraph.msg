# A reference coordinate frame and timestamp
std_msgs/Header header

# 3D spacial graph
geometry_msgs/Point[] nodes

# This vector should be the same length as nodes, above, and represents all the connecting nodes
Edges[] edges
