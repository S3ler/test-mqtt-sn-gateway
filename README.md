# test-mqtt-sn-gateway

## Modification in gtest main
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

Todo:
write documentation with following points:
overview: wich tests are made
and what are the preconditions for each tests, and which tests are overlapping

Future work: enhance SocketInterface to enable multiple technologies.

enhance device_address to: <Technologie><HardwareIdentifier><Address>
example-schema for IPv4:<IPv4><UDP|TCP><SocketPort><IP-Address><ConnectionPort>
example for IPv4 on a UDP Port 8888 with IP-Address 192.168.0.5 and ConnectionPort 9000:
 { 'I', 'P', 'v', '4', 'U', 'D', 'P', 0x22, 0xB8, 192, 168, 0, 5, 0x24, 0x28 }
 Of course everything needs to allign in the maximum size schema
