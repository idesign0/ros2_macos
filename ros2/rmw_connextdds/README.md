# ROS 2 Middleware Layer for RTI Connext DDS

This repository contains two novel implementations of the [ROS 2](https://docs.ros.org/en/rolling)
RMW layer which allow developers to deploy their ROS applications on top of
[RTI Connext DDS Professional](https://www.rti.com/products/connext-dds-professional)
and [RTI Connext DDS Micro](https://www.rti.com/products/connext-dds-micro).

The repository provides two RMW packages:

- `rmw_connextdds`

- `rmw_connextddsmicro`

Package `rmw_connextdds` is meant to be a replacement for [`rmw_connext_cpp`](https://github.com/ros2/rmw_connext).
This new implementation resolves several performance issues, and it improves out-of-the-box
interoperability with DDS applications.

*The repository is undergoing stabilization, with some features still in
active development.
Please consider reporting any [issue](https://github.com/rticommunity/rmw_connextdds/issues)
that you may experience, while monitoring the repository for frequent updates.*

For any questions or feedback, feel free to reach out to robotics@rti.com.

## Quick Start

1. Load ROS into the shell environment (Rolling if using the `master` branch,
   see [Support for different ROS 2 Releases](#support-for-different-ros-2-releases))

    ```sh
    source /opt/ros/rolling/setup.bash
    ```

2. Configure RTI Connext DDS Professional and/or RTI Connext DDS Micro on your
   system (see [Requirements](#rti-connext-dds-requirements)). Make the installation(s)
   available via environment variables, e.g. by using the provided
   `rtisetenv_<architecture>.bash` script (replace `~/rti_connext_dds-6.0.1` with
   the path of your Connext installation):

   ```sh
    source ~/rti_connext_dds-6.0.1/resource/scripts/rtisetenv_x64Linux4gcc7.3.0.bash
    export CONNEXTDDS_DIR=${NDDSHOME}
    ```

3. Create an overlay directory and clone the repository:

    ```sh
    mkdir -p ~/ros2_connextdds/src/ros2
    cd ~/ros2_connextdds
    git clone https://github.com/rticommunity/rmw_connextdds.git src/ros2/rmw_connextdds
    ```

4. Build the RMW:

    ```sh
    colcon build --symlink-install
    ```

5. Load the generated environment script:

    ```sh
    source ~/ros2_connextdds/install/setup.bash
    ```

6. Run ROS applications with RTI Connext DDS Professional:

    ```sh
    RMW_IMPLEMENTATION=rmw_connextdds ros2 run demo_nodes_cpp talker
    ```

7. Run ROS applications with RTI Connext DDS Micro:

    ```sh
    RMW_IMPLEMENTATION=rmw_connextddsmicro ros2 run demo_nodes_cpp talker
    ```

## Support for different ROS 2 Releases

`rmw_connextdds`, and `rmw_connextddsmicro` support multiple versions of ROS 2.

The following table summarizes which branch of the repository should be
checked out in order to compile the RMW implementations for a specific ROS 2
release:

|ROS 2 Release|Branch|Status|
|-------------|------|------|
|Rolling      |`master`|Developed|
|Foxy         |`foxy`|LTS (May 2023)|
|Eloquent     |`eloquent`|EOL (Nov 2020)|
|Dashing      |`dashing`|LTS (May 2021)|

Branch `master` is actively developed and maintained. It is used to create
other branches for specific ROS 2 releases (starting from Galactic).

Branches marked as `LTS` will receive updates for critical bug fixes and
important patches only (until they reach `EOL`).

Branches marked as `EOL` will not receive any future updates.

## RTI Connext DDS Requirements

Both RMW packages require the appropriate version of RTI Connext DDS to be
available on the build and target systems.

`rmw_connextdds` requires RTI Connext DDS Professional (version 5.3.1 or later),
while `rmw_connextddsmicro` requires RTI Connext DDS Micro (version 3 or later).

The installations must be made available via environment variables. If no
valid installation is detected, the packages will be skipped and not be built.

|RMW|RTI Product|Environment Variable(s)|Required|Default|
|---|-----------|-----------------------|--------|-------|
|`rmw_connextdds`|RTI Connext DDS Professional 5.3.1, or 6.x|`CONNEXTDDS_DIR`, or `NDDSHOME`|Yes|None|
|`rmw_connextddsmicro`|RTI Connext DDS Micro 3.x |`RTIMEHOME`|No (if RTI Connext DDS Professional 6.x is available)|Guessed from contents of RTI Connext DDS Professional installation (6.x only, 5.3.1 users must specify `RTIMEHOME`).|

### Multiple versions of RTI Connext DDS Professional

Package `rti_connext_dds_cmake_module` will first check variable
`${CONNEXTDDS_DIR}`, and then fall back to `${NDDSHOME}` to determine the
location of the RTI Connext DDS Professional libraries used by
`rmw_connextdds`

This behavior allows users of the old Connext RMW (`rmw_connext_cpp`) who
have installed RTI Connext DDS Professional 5.3.1 via the `apt` package
`rti-connext-dds-5.3.1`, to have both that version, and a more recent one
(e.g. 6.0.1) installed on their system, but configured via different variables.

If `rmw_connext_cpp` is installed via debian package
`ros-<version>-rmw-connext-cpp`, variable `${NDDSHOME}` will always be
hard-coded to the install location of the `apt` package
(`/opt/rti.com/rti_connext_dds-5.3.1`).

In this case, you can use `${CONNEXTDDS_DIR}` to point to a Connext 6.x
installation, making sure to source script
`rti_connext_dds-6.x.x/resource/scripts/rtisetenv_<architecture>.bash` after
loading your ROS installation, so that the Connext 6.x libraries and paths will
be found first in the relevant environement variables (e.g. `${LD_LIBRARY_PATH}`).

If you encounter any errors with selecting your desired Connext installation,
consider uninstalling `rmw_connext_cpp` and `connext_cmake_module`
(e.g. `sudo apt remove ros-<version>-rmw-connext-cpp ros-<version>-connext-cmake-module`).

## Runtime Configuration

In addition to standard configuration facilities provided by the ROS2 RMW
interface, `rmw_connextdds`, and `rmw_connextddsmicro` support the additional
configuration of some aspects of their runtime behavior via custom environment
variables.

- [RMW_CONNEXT_CYCLONE_COMPATIBILITY_MODE](#RMW_CONNEXT_CYCLONE_COMPATIBILITY_MODE)
- [RMW_CONNEXT_DISABLE_LARGE_DATA_OPTIMIZATIONS](#RMW_CONNEXT_DISABLE_LARGE_DATA_OPTIMIZATIONS)
- [RMW_CONNEXT_DISABLE_FAST_ENDPOINT_DISCOVERY](#RMW_CONNEXT_DISABLE_FAST_ENDPOINT_DISCOVERY)
- [RMW_CONNEXT_ENDPOINT_QOS_OVERRIDE_POLICY](#RMW_CONNEXT_ENDPOINT_QOS_OVERRIDE_POLICY)
- [RMW_CONNEXT_INITIAL_PEERS](#RMW_CONNEXT_INITIAL_PEERS)
- [RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE](#RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE)
- [RMW_CONNEXT_PARTICIPANT_QOS_OVERRIDE_POLICY](#RMW_CONNEXT_PARTICIPANT_QOS_OVERRIDE_POLICY)
- [RMW_CONNEXT_REQUEST_REPLY_MAPPING](#RMW_CONNEXT_REQUEST_REPLY_MAPPING)
- [RMW_CONNEXT_UDP_INTERFACE](#RMW_CONNEXT_UDP_INTERFACE)
- [RMW_CONNEXT_USE_DEFAULT_PUBLISH_MODE](#RMW_CONNEXT_USE_DEFAULT_PUBLISH_MODE)

### RMW_CONNEXT_CYCLONE_COMPATIBILITY_MODE

Enable different policies to improve interoperability with `rmw_cyclonedds_cpp`.

By default, ROS2 applications using `rmw_connextdds` will be able to communicate
with those using `rmw_cyclonedds_cpp` only via ROS2 publishers and subscribers,
while ROS2 clients and services will not interoperate across vendors.

The reason for this incompatibility lies in `rmw_cyclonedds_cpp`'s use of a custom
mapping for propagating request metadata between clients and services.

When this "compatibility mode" is enabled, `rmw_connextdds` (and `rmw_connextddsmicro`)
will use this non-standard profile in order to interoperate with `rmw_cyclonedds_cpp`,
instead of using one the two standard profiles defined by the DDS-RPC specification
(see [RMW_CONNEXT_REQUEST_REPLY_MAPPING](#rmw_connext_request_reply_mapping)).

### RMW_CONNEXT_DISABLE_LARGE_DATA_OPTIMIZATIONS

By default, `rmw_connextdds` will try to detect the use of "large data" types,
and automatically optimize the QoS of DDS DataWriters and DataReaders
using these types, to improve out of the box performance on reliable streams.

These optimizations will be applied to any endpoint whose type has a serialized
size of at least 1MB (configured by a compile-time limit).

`rmw_connextdds` will modify a "large data" endpoint's RTPS reliability
protocol parameters to more quickly recover samples, which typically improves
performance in the presence of very fragmented data, but it might also
end up increasing network traffic unnecessarily, particularly if data is not
exchanged at a fast periodic pace.

Variable `RMW_CONNEXT_DISABLE_LARGE_DATA_OPTIMIZATIONS` may be used to disable
these automatic optimizations, and revert to Connext's default behavior.

### RMW_CONNEXT_DISABLE_FAST_ENDPOINT_DISCOVERY

By default, `rmw_connextdds` modifies the QoS of its DomainParticipant to enable
the optimizations defined by RTI Connext DDS' built-in QoS snippet
`Optimization.Discovery.Endpoint.Fast`.

These optimizations speed up the discovery process between different applications
but they also introduce an overhead in network traffic, which might be undesirable
for larger systems.

Variable `RMW_CONNEXT_DISABLE_FAST_ENDPOINT_DISCOVERY` may be used to disable
these automatic optimizations, and to leave the DomainParticipant's QoS to
its defaults.

### RMW_CONNEXT_ENDPOINT_QOS_OVERRIDE_POLICY

When this variable is not set or set to `always`, the QoS settings specified in
the default profile will be used and the ros QoS profile will be applied on top
of it. You can use topic filters in XML profile files to have different defaults
for different topics, but you have to use the mangled topic names
(see [ROS topic mangling conventions](#ros-topic-mangling-conventions)).

In case this variable is set to `never`, the QoS settings will be loaded from
the default profile as before but the ros QoS profile will be ignored.
Be aware of configuring the QoS of rcl topics (`rt/rosout`, `rt/parameter_events`,
etc.) and the rmw internal topic `ros_discovery_info` correctly.

This variable can also be set to `dds_topics: <regex>`, e.g.:
 `dds_topics: rt/my_topic|rt/my_ns/another_topic`.
In that case, QoS settings for topics matching the provided regex will be
loaded in the same way as the `never` policy, and the ones that don't match
will be loaded in the same way as the `always` policy.

#### ROS topic mangling conventions

ROS mangles topic names in the following way:

- Topics are prefixed with `rt`. e.g.: `/my/fully/qualified/ros/topic` is converted to `rt/my/fully/qualified/ros/topic`.
- The service request topics are prefixed with `rq` and suffixed with `Request`. e.g.: `/my/fully/qualified/ros/service` request topic is `rq/my/fully/qualified/ros/serviceRequest`.
- The service response topics are prefixed with `rr` and suffixed with `Response`. e.g.: `/my/fully/qualified/ros/service` response topic is `rr/my/fully/qualified/ros/serviceResponse`.

### RMW_CONNEXT_INITIAL_PEERS

Variable `RMW_CONNEXT_INITIAL_PEERS` can be used to specify a list of
comma-separated values of "address locators" that the DomainParticipant created
by the RMW will use to try to make contact with remote peer applications
during the DDS discovery phase.

The values will be parsed, trimmed, and stored in QoS field
`DDS_DomainParticipantQos::discovery::initial_peers`, overwriting any
value it previously contained.

While both `rmw_connextdds` and `rmw_connextddsmicro` will honor this variable,
[equivalent, and more advanced, functionality is already available in RTI Connext DDS](https://community.rti.com/static/documentation/connext-dds/6.0.1/doc/manuals/connext_dds/html_files/RTI_ConnextDDS_CoreLibraries_UsersManual/Content/UsersManual/ConfigPeersListUsed_inDiscov.htm),
for example using variable `NDDS_DISCOVERY_PEERS`.

For this reason, only users of `rmw_connextddsmicro` should consider specifying
`RMW_CONNEXT_INITIAL_PEERS`.

For example, `rmw_connextddsmicro` will use `lo` as its default UDP network
interface (see [RMW_CONNEXT_UDP_INTERFACE](#RMW_CONNEXT_UDP_INTERFACE)),
which will prevent it from accessing the default discovery peer
(multicast address `239.255.0.1`).
The default peer configuration will also prevent the DomainParticipant from
carrying out discovery over the built-in shared-memory transport.
To enable discovery over this transport, in addition to
the default multicast peer:

```sh
RMW_IMPLEMENTATION=rmw_connextddsmicro \
RMW_CONNEXT_INITIAL_PEERS="_shmem://, 239.255.0.1" \
  ros2 run demo_nodes_cpp listener
```

### RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE

ROS2 applications using `rmw_connextdds` will not be able to interoperate with
applications using the previous RMW implementation for RTI Connext DDS, `rmw_connext_cpp`,
unless variable `RMW_CONNEXT_LEGACY_RMW_COMPATIBILITY_MODE` is used to enable
a "compatibility" mode with these older implementation.

In particular, when this mode is enabled, `rmw_connextdds` will revert to adding
a suffix (`_`) to the end of the names of the attributes of the ROS2 data types
propagated via DDS discovery.

### RMW_CONNEXT_PARTICIPANT_QOS_OVERRIDE_POLICY

Control how `rmw_connextdds` will override the default DomainParticipantQos obtained
from Connext.

If this variable is unspecified, or set to `all`, then `rmw_connextdds` will modify
the default DomainParticipantQos with settings derived from ROS 2 options (e.g.
"localhost only", or "node enclave"), and some additional optimizations meant to
improve the out of the box experiene (e.g. speed up endpoint discovery, and increase
the size of type information shared via discovery).

If the variable is set to `basic`, then only those settings associated with ROS 2
options will be modified.

If the variable is set to `never`, then no settings will be modified and the
DomainParticipantQos will be used as is.

Note that values `basic` and `never` will disable the same endpoint discovery
optimizations controlled by [RMW_CONNEXT_DISABLE_FAST_ENDPOINT_DISCOVERY](#RMW_CONNEXT_DISABLE_FAST_ENDPOINT_DISCOVERY).

### RMW_CONNEXT_REQUEST_REPLY_MAPPING

The [DDS-RPC specification](https://www.omg.org/spec/DDS-RPC/About-DDS-RPC/)
defines two profiles for mapping "request/reply" interactions over DDS messages
(e.g. ROS2 clients and services):

- the *basic* profile conveys information about the originator of a request as
  an inline payload, serialized before the actual request/reply payloads.

- The *extended* profile relies on DDS' metadata to convey request/reply
  information out of band.

By default, `rmw_connextdds` uses the *extended* profile when sending requests
from a ROS2 client to a service, while `rmw_connextddsmicro` uses the *basic* one.

Variable `RMW_CONNEXT_REQUEST_REPLY_MAPPING` can be used to select the actual
profile used at runtime. Either `"basic"` or `"extended"` may be specified.

At the moment, the *extended* profile is only available with `rmw_connextdds`.
In this configuration, `rmw_connextdds` will interoperate with `rmw_fastrtps_cpp`,
e.g.:

```sh
RMW_IMPLEMENTATION=rmw_connextdds \
  ros2 run demo_nodes_cpp add_two_ints_server

RMW_IMPLEMENTATION=rmw_fastrtps_cpp \
  ros2 run demo_nodes_cpp add_two_ints_client
```

When using the *basic* profile, `rmw_connextdds` will interoperate with
`rmw_connextddsmicro`, e.g.:

```sh
RMW_IMPLEMENTATION=rmw_connextdds \
RMW_CONNEXT_REQUEST_REPLY_MAPPING=basic \
  ros2 run demo_nodes_cpp add_two_ints_server

RMW_IMPLEMENTATION=rmw_connextddsmicro \
RMW_CONNEXT_INITIAL_PEER=localhost \
  ros2 run demo_nodes_cpp add_two_ints_client
```

Use variable [RMW_CONNEXT_CYCLONE_COMPATIBILITY_MODE](#RMW_CONNEXT_CYCLONE_COMPATIBILITY_MODE)
to enable interoperability with `rmw_cyclonedds_cpp` using a non-standard version
of the *basic* profile, e.g.:

```sh
RMW_IMPLEMENTATION=rmw_connextdds \
RMW_CONNEXT_CYCLONE_COMPATIBILITY_MODE=y \
  ros2 run demo_nodes_cpp add_two_ints_server

RMW_IMPLEMENTATION=rmw_cyclonedds_cpp \
  ros2 run demo_nodes_cpp add_two_ints_client
```

### RMW_CONNEXT_UDP_INTERFACE

RTI Connext DDS Micro requires applications to explicitly configure the network
interface to use for UDPv4 communication.

`rmw_connextddsmicro` makes the arbitrary decision of using `lo` as the default
interface.

This is undesireable if non-local communication is required, and/or if the
default DDS multicast peer (`239.255.0.1`) is to be used.

Variable `RMW_CONNEXT_UDP_INTERFACE` may be used to customize the network interface
actually used by RTI Connext DDS Micro's UDPv4 transport, e.g. to use `eth0`:

```sh
RMW_IMPLEMENTATION=rmw_connextddsmicro \
RMW_CONNEXT_UDP_INTERFACE=eth0 \
  ros2 run demo_nodes_cpp listener
```

This variable is not used by `rmw_connextdds`.

### RMW_CONNEXT_USE_DEFAULT_PUBLISH_MODE

`rmw_connextdds` will always set `DDS_DataWriterQos::publish_mode::kind` of
any DataWriter it creates to `DDS_ASYNCHRONOUS_PUBLISH_MODE_QOS`, in order to
enable out of the box support for "large data".

This behavior might not be always desirable, and it can be disabled by setting
`RMW_CONNEXT_USE_DEFAULT_PUBLISH_MODE` to a non-empty value.

This variable is not used by `rmw_connextddsmicro`, since it doesn't
automatically override `DDS_DataWriterQos::publish_mode::kind`.
