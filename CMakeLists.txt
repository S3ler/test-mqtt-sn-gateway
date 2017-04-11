cmake_minimum_required(VERSION 3.6)
project(test-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 11)
          
set(TO_TEST_IMPLEMENTATION_FOLDER linux-mqtt-sn-gateway)

set(TEST_FILES
        src/publish/Publish_Check.cpp
	)

add_executable(runAll_Tests ${TEST_FILES})

include_directories(src/lib/googletest/googlemock/include)
include_directories(src/lib/googletest/googletest/src)
include_directories(src/lib/googletest/googletest/include)
include_directories(src/lib/paho)

include_directories(src/implementation/linux-mqtt-sn-gateway/src/arduino-linux-abstraction/src)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/core-mqtt-sn-gateway/src)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/linux)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTPacket/src)



add_subdirectory(src/lib/googletest)
add_subdirectory(src/lib/paho)
add_subdirectory(src/implementation/${TO_TEST_IMPLEMENTATION_FOLDER})

target_link_libraries(runAll_Tests lib-${TO_TEST_IMPLEMENTATION_FOLDER}  gtest gtest_main gmock gmock_main)
