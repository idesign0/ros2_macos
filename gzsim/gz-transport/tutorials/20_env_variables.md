\page envvars Environment Variables

Next Tutorial: \ref development
Previous Tutorial: \ref logging

## Overview

Similar to how you can programatically customize the behavior of your
nodes or specify some options when you advertise a topic, it is possible to
use an environment variable to tweak the behavior of Ignition Transport.
Below are descriptions of the available environment variables:

* **IGN_DISCOVERY_MSG_PORT**
    * *Value allowed*: Any non-negative number in range [0-65535]. In practice
    you should use the range [1024-65535].
    * *Description*: UDP port used for message discovery. The default value is
    10317.
* **IGN_DISCOVERY_MULTICAST_IP**
    * *Value allowed*: Any multicast IP address
    * *Description*: Multicast IP address used for communicating all the
    discovery messages. The default value is 239.255.0.7.
* **IGN_DISCOVERY_SRV_PORT**
    * *Value allowed*: Any non-negative number in range [0-65535]. In practice
    you should use the range [1024-65535].
    * *Description*: UDP port used for service discovery. The default value is
    10318.
* **IGN_IP**
    * *Value allowed*: Any local IP address
    * *Description*: When you have
    multiple IP addresses for a computer and need to force Ignition
    Transport to use a particular one. This setting is only required if you
    advertise a topic or a service. If you are only subscribed to topics or
    requesting services you don't need to use this option because the
    discovery service will try all the available network interfaces during
    the search of the topic/service.
* **IGN_PARTITION**
    * *Value allowed*: Any partition value
    * *Default value*: `<HOSTNAME>:<USERNAME>` where `<HOSTNAME>` is the 
    hostname of the current machine and `<USERNAME>` is the name of the
    user launching the node.
    * *Description*: Specifies a partition name for all the nodes declared
    inside this process. Note that an alternative partition name  declared
    programatically and  passed to the constructor of a Node class will take
    priority over *IGN_PARTITION*.
* **IGN_RELAY**
    * *Value allowed*: Colon delimited list of IP addresses
    * *Description*: Connects the nodes
    of two or more different networks separated by routers. Routers do not
    forward UDP multicast traffic, so the regular discovery protocol only works
    when nodes are within the same local network. If you feel the need to
    connect these networks, make sure to have one node pointing to the IP
    address of another node from the other network. Note that only one IP_RELAY
    link is needed for bidirectional communication between nodes of two
    different networks.
* **IGN_TRANSPORT_LOG_SQL_PATH**
    * *Value allowed*: Any path
    * *Description*: Path to the SQL files used by logging. This does not
    normally need to be set. It is useful to developers who are testing changes
    to the schema, and it is used by unit tests.
* **IGN_TRANSPORT_PASSWORD**
    * *Value allowed*: Any string value
    * *Description*: A password, used in combination with
    *IGN_TRANSPORT_USERNAME*, for basic authentication. Authentication is
    enabled when both *IGN_TRANSPORT_USERNAME* and *IGN_TRANSPORT_PASSWORD*
    are specified.
* **IGN_TRANSPORT_RCVHWM**
    * *Value allowed*: Any non-negative number.
    * *Description*: Specifies the capacity of the buffer (High Water Mark)
    that stores incoming Ignition Transport messages. Note that this is a global
    queue shared by all subscribers within the same process. A value of 0 means
    "infinite" capacity. As you can guess, there's no such thing as an infinite
    buffer, so your buffer will grow until you run out of memory (and probably
    crash). If your buffer reaches the maximum capacity data will be dropped.
    * *Default value*: 1000.
* **IGN_TRANSPORT_SNDHWM**
    * *Value allowed*: Any non-negative number.
    * *Description*: Specifies the capacity of the buffer (High Water Mark)
    that stores outgoing Ignition Transport messages. Note that this is a global
    queue shared by all publishers within the same process. A value of 0 means
    "infinite" capacity. As you can guess, there's no such thing as an infinite
    buffer, so your buffer will grow until you run out of memory (and probably
    crash). If your buffer reaches the maximum capacity data will be dropped.
    * *Default value*: 1000.
* **IGN_TRANSPORT_TOPIC_STATISTICS**
    * *Value allowed*: 1/0
    * *Description*: Enable topic statistics. A value of 1 will enable topic
    statistics by sending metadata with each message. A node must
    additionally turn on statistics for a topic in order to produce results.
    The publish and subscriber must use the same value, otherwise they won't
    be able to communicate.
    * *Default value*: 0
* **IGN_TRANSPORT_USERNAME**
    * *Value allowed*: Any string value
    * *Description*: A username, used in combination with
    *IGN_TRANSPORT_PASSWORD*, for basic authentication. Authentication is
    enabled when both *IGN_TRANSPORT_USERNAME* and *IGN_TRANSPORT_PASSWORD*
    are specified.
* **IGN_VERBOSE**
    * *Value allowed*: 1/0
    * *Description*: Show debug information.
