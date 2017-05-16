//
// Created by bele on 12.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H
#define TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H


#include <gmock/gmock-generated-function-mockers.h>
#include "MqttSnReceiverInterface.h"

class MqttSnReceiverMock : public MqttSnReceiverInterface {
public:
    MOCK_METHOD1(receive_advertise,
                void(test_advertise* pAdvertise));
    MOCK_METHOD1(receive_searchgw,
                 void(test_searchgw* pSearchgw));
    MOCK_METHOD1(receive_gwinfo,
                 void(test_gwinfo* pGwinfo));
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
    MOCK_METHOD1(receive_pubrec,
                 void(test_pubrec *pPubrec));
    MOCK_METHOD1(receive_pubrel,
                 void(test_pubrel *pPubrel));
    MOCK_METHOD1(receive_pubcomp,
                 void(test_pubcomp *pPubcomp));
    MOCK_METHOD1(receiver_register,
                 void(test_register *pRegister));
    MOCK_METHOD1(receive_regack,
                 void(test_regack *pRegack));
    MOCK_METHOD1(receive_subscribe,
                 void(test_subscribe *pSubscribe));
    MOCK_METHOD1(receive_suback,
                 void(test_suback *pSuback));
    MOCK_METHOD1(receive_unsubscribe,
                 void(test_unsubscribe *pUnsubscribe));
    MOCK_METHOD1(receive_unsuback,
                 void(test_unsuback *pUnsuback));
    MOCK_METHOD1(receive_pingreq,
                 void(test_pingreq *pPingreq));
    MOCK_METHOD1(receive_pingresp,
                 void(test_pingresp *pPingresp));
    MOCK_METHOD3(receive_any_message,
                 void(uint16_t length, message_type_test type, uint8_t* data));

};


#endif //TEST_MQTT_SN_GATEWAY_MOCKMQTTSNRECEIVER_H
