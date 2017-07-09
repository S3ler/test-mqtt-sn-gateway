# test-mqtt-sn-gateway

Modification in gtest main:

    char **t_argv;
    int t_argc;
    
    GTEST_API_ int main(int argc, char **argv) {
        printf("Running main() from gtest_main.cc\n");
        testing::InitGoogleTest(&argc, argv);
        t_argc = argc;
        t_argv = argv;
        return RUN_ALL_TESTS();
    }

Copy src/lib/testclients/FakeClientSockets/py_nusperipheral.py next to your output file.
Example: Building with Clion
Output directory is: cmake-debug-debug
Target is: runAll_Tests
copy src/lib/testclients/FakeClientSockets/py_nusperipheral.py to cmake-debug-debug/py_nusperipheral.py.
runAll_Tests output file is in this directory
If you do not do this you will get something like this:
ERROR: this mock object (used in test LinuxUdpGateway_Publish_Check.PredefinedTopic_QoS_M1_Publish) should be deleted but never is. Its address is @0x7e8de8.
ERROR: 1 leaked mock object found at program exit.
Todo:
wirte documentation with following points:
overview: wich tests are made
and what are the preconditions for each tests, and which tests are overlapping