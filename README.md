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
    
Todo:
wirte documentation with following points:
overview: wich tests are made
and what are the preconditions for each tests, and which tests are overlapping