cmake_minimum_required(VERSION 3.6)
project(test-mqtt-sn-gateway)

set(CMAKE_CXX_STANDARD 14)

set(BUILD_BLE OFF)
if(${BUILD_BLE})
# thank you: https://stackoverflow.com/questions/38515316/how-to-configure-dbus-dependencies-with-cmake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(DBus REQUIRED)
include_directories(${DBUS_INCLUDE_DIRS})

pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
endif(${BUILD_BLE})


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
        src/functional/LinuxUdpGateway_Function_Connect.cpp
)

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


add_subdirectory(src/lib/googletest)
add_subdirectory(src/lib/testclients)
add_subdirectory(src/implementation/${TO_TEST_IMPLEMENTATION_FOLDER})

if(${BUILD_BLE})
find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(src/implementation/${TO_TEST_IMPLEMENTATION_FOLDER}/src/Implementation/Ble/SimpleBluetoothLowEnergySocket/libBLE/)
endif(${BUILD_BLE})

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
