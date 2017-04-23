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
    
