cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)


include_directories(pahomqttembeddedc/MQTTClient/src/linux)
include_directories(pahomqttembeddedc/MQTTClient/src/)
include_directories(pahomqttembeddedc/MQTTPacket/src)

file(GLOB PAHO_SOURCE_FILES
        pahomqttembeddedc/MQTTPacket/src/*.c
        pahomqttembeddedc/MQTTPacket/src/*.h
        pahomqttembeddedc/MQTTClient/src/*.cpp
        pahomqttembeddedc/MQTTClient/src/*.h
        pahomqttembeddedc/MQTTClient/src/linux/*.cpp
        pahomqttembeddedc/MQTTClient/src/linux/*.h
        )

set(PAHO_TEST_SOURCE_FILES
        PahoMqttTestMessageHandler.cpp
        MqttReceiverInterface.h
        MqttReceiverMock.cpp
        MqttReceiverMock.h
        MqttPublish.cpp
        MqttPublish.h
        )

add_library(PahoLinuxTestMqttMessageHandler ${PAHO_SOURCE_FILES} ${PAHO_TEST_SOURCE_FILES})



set(FAKECLIENT_SOURCE_FILES PahoMqttTestMessageHandler.h
        LinuxUdpClientFake.h
        LinuxUdpClientFake.cpp
        MqttSnReceiverInterface.cpp
        MqttSnReceiverInterface.h
        MockMqttSnReceiver.cpp
        MockMqttSnReceiver.h
        mqttsn_test_messages.h
        FakeClientSockets/FakeSocketInterface.h
        )

set(FAKESOCKET_SOURCE_FILES
        FakeClientSockets/FakeUdpSocket.h
        FakeClientSockets/FakeUdpSocket.cpp
        FakeClientSockets/FakeBluetoothLeSocket.h
        FakeClientSockets/FakeBluetoothLeSocket.cpp
        )

# TODO add python dynamically instead of this hard coded include_directories
include_directories(/usr/include/python2.7)

add_library(LinuxMqttSnClientFake ${FAKECLIENT_SOURCE_FILES} ${FAKESOCKET_SOURCE_FILES})
target_link_libraries(LinuxMqttSnClientFake python2.7)