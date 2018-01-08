cmake_minimum_required(VERSION 3.6)
project(test-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 14)

# thank you: https://stackoverflow.com/questions/38515316/how-to-configure-dbus-dependencies-with-cmake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(DBus REQUIRED)
include_directories(${DBUS_INCLUDE_DIRS})

pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})


set(TO_TEST_IMPLEMENTATION_FOLDER linux-mqtt-sn-gateway)


add_definitions(-DFakeClientTransmissionProtocol=FakeClientTransmissionProtocol_SERIAL)

set(TEST_FILES

        src/google_test_main_arguments.h

        src/compliance/Publish_Check.cpp
        src/compliance/LinuxUdpGateway_Connect_Test.cpp
        src/compliance/LinuxUdpGateway_Register_Check.cpp
        src/compliance/LinuxUdpGateway_Unsubscribe_Check.cpp
        src/compliance/LinuxUdpGateway_SearchGw_Check.cpp
        src/compliance/LinuxUdpGateway_Subscribe_Check.cpp
        src/compliance/LinuxUdpGateway_Pingreq_Test.cpp
        #src/compliance/LinuxUdpGateway_WillUpdate_Test.cpp

        src/functional/LinuxUdpGateway_Advertise.cpp
        src/functional/LinuxUdpGateway_Functional_Subscribe_Check.cpp
        src/functional/LinuxUdpGateway_Publish.cpp
        #src/functional/LinuxUdpGateway_Will.cpp
        #src/functional/LinuxUdpGateway_WillUpdate.cpp
        src/compliance/LinuxUdpGateway_Disconnect_Test.cpp
        #src/functional/LinuxUdpGateway-Functional_Disconnect.cpp
        src/functional/LinuxUdpGateway_Sleep.cpp
        src/functional/LinuxUdpGateway_Function_Connect.cpp)

add_executable(runAll_Tests ${TEST_FILES})

include_directories(src/lib/googletest/googlemock/include)
include_directories(src/lib/googletest/googletest/src)
include_directories(src/lib/googletest/googletest/include)
include_directories(src/lib/testclients)

include_directories(src/implementation/linux-mqtt-sn-gateway/src/arduino-linux-abstraction/src)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/core-mqtt-sn-gateway/src)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/linux)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTClient/src/)
include_directories(src/implementation/linux-mqtt-sn-gateway/src/Implementation/paho/pahomqttembeddedc/MQTTPacket/src)



add_subdirectory(src/lib/googletest)
add_subdirectory(src/lib/testclients)
add_subdirectory(src/implementation/${TO_TEST_IMPLEMENTATION_FOLDER})

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(src/implementation/${TO_TEST_IMPLEMENTATION_FOLDER}/src/Implementation/Ble/SimpleBluetoothLowEnergySocket/libBLE/)

target_link_libraries(runAll_Tests
        lib-${TO_TEST_IMPLEMENTATION_FOLDER}
        PahoLinuxTestMqttMessageHandler
        LinuxMqttSnClientFake
        gtest gtest_main gmock gmock_main)
