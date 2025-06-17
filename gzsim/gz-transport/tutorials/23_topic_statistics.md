\page topicstatistics Topic Statistics

Previous Tutorial: \ref development

## Overview

Topic statistics provides a mechanism to quantify transport performance
between a publisher and subscriber. When enabled, a subscriber on a topic
can measure:

1. Number of dropped messages,
2. Publication statistics,
3. Reception statistics, and
4. Message age statistics.

Dropped messages are detected by the subscriber when an incoming message has
an ID that is larger than 1 plus the previous message ID. A message can be
dropped for a number of reasons. For example, a message can be dropped when
the publication and/or subscription buffers overflow.

Publication statistics capture information about when messages are sent by
a publisher. The average Hz rate, minimum and maximum times between
publications, and standard deviation of publication times are available.

Similarly, reception statistics capture information about when messages are
received by a subscriber. The average Hz rate, minimum and maximum times between receptions, and standard deviation of reception times are available.

Finally, message age statistics capture information between publication and
reception. The age of a message is the time between publication and
reception. We are ignoring clock discrepancies. The average, minimum, maximum, and standard deviation values of message age are available.

## Usage

The `IGN_TRANSPORT_TOPIC_STATISTICS` environment variable must be set to `1`
for both publishers and subscribers. Setting `IGN_TRANSPORT_TOPIC_STATISTICS` to `1` will change the wire protocol, which will prevent communication with nodes that have not set `IGN_TRANSPORT_TOPIC_STATISTICS` to `1`.

Additionally, a node on the subscriber side of a pub/sub relationship must
call `EnableStats`. For example:

```
if (!node.EnableStats(topic, true))
{
  std::cout << "Unable to enable stats\n";
}
```

A complete example can be found in the [subscriber_stats example program](https://github.com/ignitionrobotics/ign-transport/blob/main/example/subscriber_stats.cc).

With both `IGN_TRANSPORT_TOPIC_STATISTICS` set to `1` and a node
enabling topic statistics, then you will be able to echo statistic
information from the command line using `ign topic -et /statistics`.

It is possible to change the statistics output topic from `/statistics` to
one of your choosing by specifying a topic name when enabling topic
statistics. For example:

```
if (!node.EnableStats(topic, true, "/my_stats"))
{
  std::cout << "Unable to enable stats\n";
}
```

You can also change the statistics publication rate from 1Hz by specifying
the new Hz rate after the statistic's topic name:

```
if (!node.EnableStats(topic, true, "/my_stats", 100))
{
  std::cout << "Unable to enable stats\n";
}
```

### Example

If you have the Ignition Transport sources with the example programs built,
then you can test topic statistics by following these steps.

1. Terminal 1: `IGN_TRANSPORT_TOPIC_STATISTICS=1 ./examples/build/publisher`
1. Terminal 2: `IGN_TRANSPORT_TOPIC_STATISTICS=1 ./examples/build/subscriber_stats`
1. Terminal 3: `IGN_TRANSPORT_TOPIC_STATISTICS=1 ign topic -et /statistics`
