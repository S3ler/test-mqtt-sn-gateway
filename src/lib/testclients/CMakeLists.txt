

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


set(SOURCE_FILES PahoMqttTestMessageHandler.h PahoMqttTestMessageHandler.cpp MqttReceiverInterface.h MqttReceiverMock.cpp MqttReceiverMock.h MqttPublish.cpp MqttPublish.h LinuxUdpClientFake.h LinuxUdpClientFake.cpp MqttSnReceiverInterface.cpp MqttSnReceiverInterface.h MockMqttSnReceiver.cpp MockMqttSnReceiver.h mqttsn_test_messages.h)
add_library(PahoLinuxTestMqttMessageHandler ${SOURCE_FILES} ${PAHO_SOURCE_FILES} )

