# test-mqtt-sn-gateway
Test Project for the aggregating [linux-mqtt-sn-gateway](https://github.com/S3ler/linux-mqtt-sn-gateway).

## Getting Started - running
You need to have [docker](https://www.docker.com/) installed. During the tests we use start/stop [Mosquitto](https://hub.docker.com/_/eclipse-mosquitto/) with docker.

Clone the repository, initialize CMAKE with the Transmission Protocol (e.g. UDP), make rnAll_Test, run runAll_Test

    git clone --recursive https://github.com/S3ler/test-mqtt-sn-gateway.git
    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DTRANSMISSION_PROTOCOL=UDP
    make runAll_Tests
    ./runAll_Tests
    
done.

## Implementation Notes

### Modification in gtest main
We adept the gtest_main to have access to the command line arguments, especially the run-directory.

The following modification is automatically done by cmake to ${gtest_SOURCE_DIR}/src/gtest_main.cc

    #include <stdio.h>
    #include "gtest/gtest.h"
    char **t_argv;
    int t_argc;
    
    GTEST_API_ int main(int argc, char **argv) {
        printf("Running main() from gtest_main.cc\n");
        testing::InitGoogleTest(&argc, argv);
        t_argc = argc;
        t_argv = argv;
        return RUN_ALL_TESTS();
    }

##Todo:
 * write documentation with following points:
 * overview: wich tests are made
 * and what are the preconditions for each tests, and which tests are overlapping
