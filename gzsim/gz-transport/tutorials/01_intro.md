\page introduction Introduction

Next Tutorial: \ref installation

## What is Ignition Transport?

Ignition Transport is an open source communication library that allows sharing
data between clients. In our context, a client is called a node. Nodes might
be running within the same process on the same machine or on separate machines located
around the world. Ignition Transport is multi-platform (Linux, Mac OS X, and
Windows), so all the low level details, such as data alignment or endianness, are
hidden for you.

Ignition Transport uses [Google Protocol buffers]
(https://developers.google.com/protocol-buffers/?hl=en) as the data type for
communicating between nodes. Users can define their own messages using the
Protobuf utils, and then exchange them between the nodes. Ignition Transport
discovers, serializes and delivers messages to the destinations using a
combination of custom code and [ZeroMQ] (http://zeromq.org/).

## What programming language can I use with Ignition Transport?

C++ is the native implementation and so far the only way to use the library.
We hope to offer different wrappers for the most popular languages in the future.
