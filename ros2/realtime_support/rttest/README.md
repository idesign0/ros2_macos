# rttest

rttest is a minimal tool for instrumenting and running tests for synchronous real-time systems.
It provides utilities for measuring and plotting jitter, latency, and missed deadlines.
It also provides a library with macros for instrumenting code.
It is designed with real-time Linux-based systems in mind, such as [Preempt RT](https://wiki.linuxfoundation.org/realtime) kernel.

## Build instructions
Build from source:

```
mkdir build
cd build

cmake ..
sudo make install
```

Alternatively for a local install:

```
cmake .. -DCMAKE_INSTALL_PREFIX=<build folder>
make install
```

Build and run the example:

```
cd examples
mkdir build
cd build
cmake .. (-DCMAKE_INSTALL_PREFIX=<build folder>)
make
./example_loop
```

## Command line arguments

Passing `argc` and `argv` of an instrumented main function to `rttest_read_args` will enable command line arguments for the instrumented function.

-u Specify the update period.
Default units are microseconds.
Use the suffix "s" for seconds, "ms" for milliseconds, "us" for microseconds, and "ns" for nanoseconds.
Default update period is 1ms.

-m Set maximum stack prefault size for static stack prefaulting.
"b" for bytes, "kb" for kilobytes, "mb" for megabytes, "gb" for gigabytes (use with caution).
Default stack size is 1MB.

-d Set maximum heap prefault size for dynamic memory prefaulting.
"b" for bytes, "kb" for kilobytes, "mb" for megabytes, "gb" for gigabytes (use with caution).
Default heap size is 8192MB.

-i Specify how many iterations to run the real-time loop.
Specifying an iteration value less than or equal to 0 will cause rttest to run forever or until interrupted with Ctrl-C.
If running forever, rttest will not save a data buffer and writing results to a file will not work.
Default value is 1000.

-tp Set the thread priority of all threads launched by the test program.
Individual thread priority can be set using the `rttest_set_sched_priority` command.

-f Specify the name of the file for writing the collected data. Plot this data file using the `rttest_plot.py` script provided in `scripts`.
