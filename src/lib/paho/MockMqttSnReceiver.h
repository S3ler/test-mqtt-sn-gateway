//
// Created by bele on 12.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H
#define TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H


#include <gmock/gmock-generated-function-mockers.h>
#include "MqttSnReceiverInterface.h"

class MqttSnReceiverMock : public MqttSnReceiverInterface {
public:
    MOCK_METHOD1(receive_pingreq,
    void(message_header *pHeader));
    MOCK_METHOD1(receive_pingresp,
    void(message_header *pHeader));
    MOCK_METHOD1(receive_connect,
    void(msg_connect *pConnect));
    MOCK_METHOD1(receive_connack,
    void(msg_connack *pConnack));
    MOCK_METHOD1(receive_willtopicreq,
    void(message_header *pHeader));
    MOCK_METHOD1(receive_willtopic,
    void(msg_willtopic *pWilltopic));
    MOCK_METHOD1(receive_willmsgreq,
    void(message_header *pHeader));
    MOCK_METHOD1(receiver_willmsg,
    void(msg_willmsg *pWillmsg));
    MOCK_METHOD1(receiver_register,
    void(msg_register *pRegister));
    MOCK_METHOD1(receive_regack,
    void(msg_regack *pRegack));
    MOCK_METHOD1(receive_publish,
    void(msg_publish *pPublish));
    MOCK_METHOD1(receive_puback,
    void(msg_puback *pPuback));
    MOCK_METHOD1(receive_pubcomp,
    void(msg_pubcomp *pPubcomp));
    MOCK_METHOD1(receive_pubrec,
    void(msg_pubrec *pPubrec));
    MOCK_METHOD1(receive_pubrel,
    void(msg_pubrel *pPubrel));
    MOCK_METHOD1(receive_subscribe,
    void(msg_subscribe *pSubscribe));
    MOCK_METHOD1(receive_suback,
    void(msg_suback *pSuback));
    MOCK_METHOD1(receive_unsubscribe,
    void(msg_unsubscribe *pUnsubscribe));
    MOCK_METHOD1(receive_unsuback,
    void(msg_unsuback *pUnsuback));
    MOCK_METHOD1(receive_disconnect,
    void(message_header *pHeader));
    MOCK_METHOD1(receive_willtopicresp,
    void(msg_willtopicresp *pWilltopicresp));
    MOCK_METHOD1(receive_willmsgresp,
    void(msg_willmsgresp *pWillmsgresp));
    MOCK_METHOD1(receive_any,
    void(uint8_t *pData));
};


#endif //TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H
