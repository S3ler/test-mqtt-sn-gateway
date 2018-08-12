cmake_minimum_required(VERSION 3.6)
project(test-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 14)

# see: https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()

include_directories(${CMAKE_BINARY_DIR}/googletest-src/googlemock/include)
include_directories(${CMAKE_BINARY_DIR}/googletest-src/googletest/include)

# Add global command line argument variables t_argv and t_argv to gtest_main.cc
execute_process (
        COMMAND bash -c "if ! grep -q \"t_argv;\" ${gtest_SOURCE_DIR}/src/gtest_main.cc; then \n \
        sed '/include \"gtest/a char \\*\\*t_argv;\n' ${gtest_SOURCE_DIR}/src/gtest_main.cc  > ${gtest_SOURCE_DIR}/src/gtest_main_temp1.cc \n \
        sed '/\\*\\*t_argv;/a int t_argc;\n' ${gtest_SOURCE_DIR}/src/gtest_main_temp1.cc > ${gtest_SOURCE_DIR}/src/gtest_main_temp2.cc \n \
        sed '/testing::InitGoogleTest(&argc, argv);/a t_argc = argc;\n' ${gtest_SOURCE_DIR}/src/gtest_main_temp2.cc > ${gtest_SOURCE_DIR}/src/gtest_main_temp1.cc \n \
        sed '/t_argc = argc;/a t_argv = argv;\n' ${gtest_SOURCE_DIR}/src/gtest_main_temp1.cc > ${gtest_SOURCE_DIR}/src/gtest_main_temp2.cc \n \
        mv ${gtest_SOURCE_DIR}/src/gtest_main_temp1.cc ${gtest_SOURCE_DIR}/src/gtest_main_temp.cc \n \
        mv ${gtest_SOURCE_DIR}/src/gtest_main_temp2.cc ${gtest_SOURCE_DIR}/src/gtest_main_temp.cc \n \
        mv ${gtest_SOURCE_DIR}/src/gtest_main_temp.cc ${gtest_SOURCE_DIR}/src/gtest_main.cc \n \
        fi"
)

set(BUILD_RPI OFF)
if(${BUILD_RPI})
#target_compile_definitions(lib-linux-mqtt-sn-gateway PRIVATE -DRH_PLATFORM=RH_PLATFORM_RASPI)
add_definitions(-DRH_PLATFORM=RH_PLATFORM_RASPI)
endif(${BUILD_RPI})

set(BUILD_LORA OFF)
if(${BUILD_LORA})
add_definitions(-DRH_PLATFORM=RH_PLATFORM_RPI)
endif(${BUILD_LORA})

set(TO_TEST_IMPLEMENTATION_FOLDER linux-mqtt-sn-gateway)

#add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
add_definitions(-DGATEWAY_TRANSMISSION_PROTOCOL_UDP)
#add_definitions(-DFakeClientTransmissionProtocol=FakeClientTransmissionProtocol_SERIAL)
add_definitions(-DFakeClientTransmissionProtocol=FakeClientTransmissionProtocol_UDP)

set(TEST_FILES
        src/google_test_main_arguments.h
        src/compliance/LinuxUdpGateway_Connect_Test.cpp
        #src/integration/General_Check.cpp
)

add_executable(runAll_Tests ${TEST_FILES})


include_directories(src/lib/testclients)

include_directories(src/implementation/linux-mqtt-sn-gateway/src/arduino-linux-abstraction/src)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/core-mqtt-sn-gateway/src)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/linux)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTPacket/src)
if(${BUILD_RPI})
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/mqtt-sn-sockets)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/mqtt-sn-sockets/RadioHead)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/mqtt-sn-sockets/RadioHead/RHutil)
endif(${BUILD_RPI})
if(${BUILD_LORA})
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/mqtt-sn-sockets)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/mqtt-sn-sockets/RadioHeadRpi)
#include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/mqtt-sn-sockets/RadioHeadRpi/RHutil)
endif(${BUILD_LORA})


add_subdirectory(src/lib/testclients)
add_subdirectory(src/implementation/${TO_TEST_IMPLEMENTATION_FOLDER})

if(${BUILD_RPI})
target_link_libraries(runAll_Tests lib-rh-nrf24-socket bcm2835)
endif(${BUILD_RPI})

if(${BUILD_LORA})
target_link_libraries(runAll_Tests lib-rh96-socket wiringPi)
endif(${BUILD_LORA})

target_link_libraries(runAll_Tests
        lib-${TO_TEST_IMPLEMENTATION_FOLDER}
        PahoLinuxTestMqttMessageHandler
        LinuxMqttSnClientFake
        gtest gtest_main gmock gmock_main)
