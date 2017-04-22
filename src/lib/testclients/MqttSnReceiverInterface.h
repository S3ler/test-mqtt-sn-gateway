//
// Created by bele on 12.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_MQTTSNRECEIVERINTERFACE_H
#define TEST_MQTT_SN_GATEWAY_MQTTSNRECEIVERINTERFACE_H

#include <cstdint>
#include "mqttsn_test_messages.h"

class MqttSnReceiverInterface {

public:
    virtual ~MqttSnReceiverInterface() {};

    virtual void receive_advertise(test_advertise* pAdvertise)=0;

    virtual void receive_searchgw(test_searchgw* pSearchgw)=0;

    virtual void receive_gwinfo(test_gwinfo* pGwinfo)=0;

    virtual void receive_connect(test_connect *pConnect)=0;

    virtual void receive_disconnect(test_disconnect *pDisconnect)=0;

    virtual void receive_connack(test_connack* pConnack)=0;

    virtual void receive_willtopicreq(test_willtopicreq *pWilltopicreq)=0;

    virtual void receive_willtopic(test_willtopic *pWilltopic)=0;

    virtual void receive_willmsgreq(test_willmsgreq *pWillmsgreq)=0;

    virtual void receiver_willmsg(test_willmsg *pWillmsg)=0;

    virtual void receive_publish(test_publish* pPublish)=0;

    virtual void receive_puback(test_puback *pPuback)=0;

    virtual void receive_pubrec(test_pubrec *pPubrec)=0;

    virtual void receive_pubrel(test_pubrel *pPubrel)=0;

    virtual void receive_pubcomp(test_pubcomp *pPubcomp)=0;

    virtual void receiver_register(test_register *pRegister)=0;

    virtual void receive_regack(test_regack *pRegack)=0;

    virtual void receive_subscribe(test_subscribe *pSubscribe)=0;

    virtual void receive_suback(test_suback *pSuback)=0;

    virtual void receive_unsubscribe(test_unsubscribe *pUnsubscribe)=0;

    virtual void receive_unsuback(test_unsuback *pUnsuback)=0;

    virtual void receive_any_message(uint16_t length, message_type_test type, uint8_t* data)=0;

};


#endif //TEST_MQTT_SN_GATEWAY_MQTTSNRECEIVERINTERFACE_H
