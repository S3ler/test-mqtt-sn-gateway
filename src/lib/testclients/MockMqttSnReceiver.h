//
// Created by bele on 12.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H
#define TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H


#include <gmock/gmock-generated-function-mockers.h>
#include "MqttSnReceiverInterface.h"

class MqttSnReceiverMock : public MqttSnReceiverInterface {
public:
    MOCK_METHOD1(receive_connect,
                 void(test_connect *pConnect));
    MOCK_METHOD1(receive_disconnect,
                 void(test_disconnect *pDisconnect));
    MOCK_METHOD1(receive_connack,
                 void(test_connack* pConnack));
    MOCK_METHOD1(receive_willtopicreq,
                 void(test_willtopicreq *pWilltopicreq));
    MOCK_METHOD1(receive_willtopic,
                 void(test_willtopic *pWilltopic));
    MOCK_METHOD1(receive_willmsgreq,
                 void(test_willmsgreq *pHeader));
    MOCK_METHOD1(receiver_willmsg,
                 void(test_willmsg *pWillmsgreq));
    MOCK_METHOD1(receive_publish,
                 void(test_publish *pPublish));
    MOCK_METHOD1(receive_puback,
                 void(test_puback *pPuback));
    MOCK_METHOD3(receive_any_message,
                 void(uint16_t length, message_type_test type, uint8_t* data));
};


#endif //TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H
