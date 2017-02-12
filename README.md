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

Now the amalgamation (`mqtt.c` and `mqtt.h`) is found in `<project_root>/amalgamation` directory.
