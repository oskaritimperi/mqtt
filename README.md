# mqtt

A simple MQTT library written in C.

# Building

Building the library requires CMake and a C compiler.

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

# Creating the single file library

Creating the single file library requires python (both 2 and 3 are ok).

Execute in the build directory you created above:

    $ cmake -DMQTT_AMALGAMATE=ON .
    $ make amalgamate

Now the amalgamation (`mqtt.c` and `mqtt.h`) is found in
`<project_root>/amalgamation` directory.

# Dumping sent/received data

Define `STREAM_HEXDUMP_READ` and `STREAM_HEXDUMP_WRITE` to make the library dump
read or written written data respectively to stdout. Nice for debugging.

# Logging

Define `LOG_LEVEL` to one of `DEBUG`, `INFO`, `WARNING` or `ERROR` to make the
library output logging information.

# Tools

There are publish/subscribe tools included in the `tools` directory and they are
built by default.
