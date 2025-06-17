\page security Security

Next Tutorial: \ref relay
Previous Tutorial: \ref services

## Overview

Ignition Transport's default mode of communication is unsecure, which means
no authentication or encryption is used. Unsecure communication is the
default because it is simple to implement and use, supports introspection,
and reduces third-party development effort.

There are many cases when security is required by an application. For
example, authentication may be needed to verify participants in a network.
Ignition Transport relies on security features available in ZeroMQ. The
available security features include authentication, and encryption.  

The remainder of the this tutorial describes how to setup and use security
with Ignition Transport.

## Authentication

Authentication relies on a username and password to limit access to
communication topics. Currently, authentication in Ignition Transport
operates at the process level. This means all topics within a process will
either use, or not use, authentication.

Two environment variables are used to enable authentications:

1. `IGN_TRANSPORT_USERNAME` : The username 
2. `IGN_TRANSPORT_PASSWORD` : The password 

When both `IGN_TRANSPORT_USERNAME` and `IGN_TRANSPORT_PASSWORD` are set,
the authentication is enabled for a process. Every publisher in a secure
process will require subscribers to provide the correct username and
password. Also, every subscriber will only connect to secure publishers. 

### Example

First, let's test unsecure communication. This example requires
[ign-tools](https://github.com/ignitionrobotics/ign-tools).

1. Open a terminal, and echo topic `/foo`.
```
ign topic -t /foo -e
```
2. Open a second terminal and publish a message on topic `/foo`.
```
ign topic -t /foo -m ignition.msgs.StringMsg -p 'data:"Unsecure message"'
```
3. The first terminal should see the following output.
```
data: "Unsecure message"
```

Now let's try a secure publisher and an unsecure subscriber.

1. Leave the first terminal running `ign topic -t /foo -e`.
2. Setup authentication in the second terminal:
```
export IGN_TRANSPORT_USERNAME=user
export IGN_TRANSPORT_PASSWORD=pass
```
3. Now publish a message in the second terminal:
```
ign topic -t /foo -m ignition.msgs.StringMsg -p 'data:"Secure message"'
```
4. The first terminal should not change, which indicates that subscriber was
   not able to authenticate with the secure publisher.

Finally, let's create secure subscriber.

1. Open a third terminal, and setup authentication in that terminal.
```
export IGN_TRANSPORT_USERNAME=user
export IGN_TRANSPORT_PASSWORD=pass
```
2. Echo the `/foo` topic in the secure third terminal.
```
ign topic -t /foo -e
```
3. Go back to the secure publisher in the second terminal, and re-run the
   publish command.
```
ign topic -t /foo -m ignition.msgs.StringMsg -p 'data:"Secure message"'
```
4. The third terminal, running the secure subscriber, should output the
   following.
```
data: "Secure message"
```
5. The unsecure subscriber in the first terminal should not change.

## Encryption

Coming soon.
