\page logging Logging

Next Tutorial: \ref envvars
Previous Tutorial: \ref relay

## Overview

In this tutorial, we are going to describe the process of recording and
playing back a collection of messages.

Ignition Transport provides two mechanisms for logging: a C++ API and a set of
command line utilities as part of the optional `ign` CLI tool (available via
[Ignition Tools](https://github.com/ignitionrobotics/ign-tools)). We use
[SQLite3](https://www.sqlite.org) to create a file containing all the messages
recorded during a session. You can imagine it as a container where all the
original messages have been efficiently stored and timestamped.

Let's create a directory for compiling and running our examples:

```{.sh}
mkdir ~/ign_transport_tutorial
cd ~/ign_transport_tutorial

```

## Record

Download the [record.cc](https://github.com/ignitionrobotics/ign-transport/raw/main/example/record.cc)
file within the `ign_transport_tutorial` folder and open it with your favorite editor:

```{.cpp}
#include <cstdint>
#include <iostream>
#include <regex>

#include <gz/transport/Node.hh>
#include <gz/transport/log/Recorder.hh>

//////////////////////////////////////////////////
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " OUTPUT.tlog\n";
    return -1;
  }

  gz::transport::log::Recorder recorder;

  // Record all topics
  const int64_t addTopicResult = recorder.AddTopic(std::regex(".*"));
  if (addTopicResult < 0)
  {
    std::cerr << "An error occurred when trying to add topics: "
              << addTopicResult << "\n";
    return -1;
  }

  // Begin recording, saving received messages to the given file
  const auto result = recorder.Start(argv[1]);
  if (gz::transport::log::RecorderError::SUCCESS != result)
  {
    std::cerr << "Failed to start recording: " << static_cast<int64_t>(result)
              << "\n";
    return -2;
  }

  std::cout << "Press Ctrl+C to finish recording.\n  Recording... "
            << std::endl;

  // Wait until the interrupt signal is sent.
  gz::transport::waitForShutdown();

  recorder.Stop();

  std::cout << "\nRecording finished!" << std::endl;
}
```

### Walkthrough

```{.cpp}
#include <gz/transport/log/Recorder.hh>
```

The `Recorder.hh` header contains all the recording functionality. Make sure
that is included in your source code.

```{.cpp}
gz::transport::log::Recorder recorder;

// Record all topics
const int64_t addTopicResult = recorder.AddTopic(std::regex(".*"));
if (addTopicResult < 0)
{
  std::cerr << "An error occurred when trying to add topics: "
            << addTopicResult << "\n";
  return -1;
}
```

The next step requires instantiation of a `log::Recorder` object and
specification of the topics that we're interested in logging. The `AddTopic()`
function accepts a regular expression to set the appropriate topic filter.
In our example, we are recording all topics.

```{.cpp}
// Begin recording, saving received messages to the given file
const auto result = recorder.Start(argv[1]);
```

The `Start()` method starts recording messages. Note that the function accepts
a parameter with the name of the log file.

```{.cpp}
// Wait until the interrupt signal is sent.
gz::transport::waitForShutdown();

recorder.Stop();
```

In our example, we are logging messages until the user hits `CTRL-C`. The
function `gz::transport::waitForShutdown()` captures the appropriate
signal and blocks the execution until that event occurs. Then, `recorder.Stop()`
stops the log recording as expected.

## Play back

Download the [playback.cc](https://github.com/ignitionrobotics/ign-transport/raw/main/example/playback.cc)
file within the `ign_transport_tutorial` folder and open it with your favorite
editor:

```{.cpp}
#include <cstdint>
#include <iostream>
#include <regex>
#include <gz/transport/log/Playback.hh>

//////////////////////////////////////////////////
int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " INPUT.tlog\n";
    return -1;
  }

  gz::transport::log::Playback player(argv[1]);

  // Playback all topics
  const int64_t addTopicResult = player.AddTopic(std::regex(".*"));
  if (addTopicResult == 0)
  {
    std::cout << "No topics to play back\n";
    return 0;
  }
  else if (addTopicResult < 0)
  {
    std::cerr << "Failed to advertise topics: " << addTopicResult
              << "\n";
    return -1;
  }

  // Begin playback
  const auto handle = player.Start();
  if (!handle)
  {
    std::cerr << "Failed to start playback\n";
    return -2;
  }

  // Wait until the player stops on its own
  std::cout << "Playing all messages in the log file\n";
  handle->WaitUntilFinished();
}
```

### Walkthrough

```{.cpp}
#include <gz/transport/log/Playback.hh>
```

The `Playback.hh` header contains all the log play back functionality. Make sure
that is included in your source code.

```{.cpp}
gz::transport::log::Playback player(argv[1]);

// Playback all topics
const int64_t addTopicResult = player.AddTopic(std::regex(".*"));
}
```

The next step requires to instantiate a `log::Playback` object and to specify
the path to the file containing the log. The `AddTopic()` function accepts
a regular expression to set the appropriate topic filter. In our example, we are
playing back all messages.

```{.cpp}
const auto handle = player.Start();
```

The `Start()` method starts message playback. Note that this is an
asynchronous function, the messages will be published without blocking the
current thread.

```{.cpp}
handle->WaitUntilFinished();
```

In this example we are not interested in doing anything else, besides playing
back messages. Therefore, we can use `WaitUntilFinished()` to block the current
thread until all messages have been published.

## Building the code

Download the [CMakeLists.txt](https://github.com/ignitionrobotics/ign-transport/raw/main/example/CMakeLists.txt)
file within the `ign_transport_tutorial` folder.

Once you have all your files, go ahead and create a `build/` directory within
the `ign_transport_tutorial` directory.

```{.sh}
mkdir build
cd build
```
Run `cmake` and build the code.

```{.sh}
cmake ..
make
```

## Running the examples

Open two new terminals and from your `build/` directory run the recorder.

From terminal 1:

```{.sh}
./log_record tutorial.tlog
Press Ctrl+C to finish recording.
  Recording...
```

From terminal 2, publish a message using `ign`:

```{.sh}
ign topic -t /foo -m ignition.msgs.StringMsg -p 'data:"Hello log"'
```

From terminal 1, hit `CTRL-C` in your recorder terminal to stop the recording.

Moving back to terminal 2, run a subscriber:

```{.sh}
ign topic -t /foo -e
```

And from terminal 1, playback your log file:

```{.sh}
./log_playback tutorial.tlog
```

You should receive one message in terminal 2:

```{.sh}
data: "Hello log"
```

## Using `ign` for recording and playing back

As an alternative to the C++ API, we could use the `ign` suite of command line
tools for recording and playing back messages.

Next is how you can record a set of messages using `ign`:

```{.sh}
ign log record --force --file tutorial.tlog
```

And here's how you can play back the previous log file using `ign`:

```{.sh}
ign log playback --file tutorial.tlog
```

For further options, try running:
```{.sh}
ign log record -h
```
and
```{.sh}
ign log playback -h
```
