//
// Created by bele on 12.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_MQTTSNRECEIVERINTERFACE_H
#define TEST_MQTT_SN_GATEWAY_MQTTSNRECEIVERINTERFACE_H


#include <cstdint>
#include <mqttsn_messages.h>

class MqttSnReceiverInterface {

public:
    // virtual virtual void receive_advertise(msg_advertise *pAdvertise)= 0;

    virtual ~MqttSnReceiverInterface() {};

    virtual void receive_pingreq(message_header *pHeader)=0;

    virtual void receive_pingresp(message_header *pHeader)=0;

    virtual void receive_connect(msg_connect *pConnect)=0;

    virtual void receive_connack(msg_connack *pConnack)=0;

    virtual void receive_willtopicreq(message_header *pHeader)=0;

    virtual void receive_willtopic(msg_willtopic *pWilltopic)=0;

    virtual void receive_willmsgreq(message_header *pHeader)=0;

    virtual void receiver_willmsg(msg_willmsg *pWillmsg)=0;

    virtual void receiver_register(msg_register *pRegister)=0;

    virtual void receive_regack(msg_regack *pRegack)=0;

    virtual void receive_publish(msg_publish *pPublish)=0;

    virtual void receive_puback(msg_puback *pPuback)=0;

    virtual void receive_pubcomp(msg_pubcomp *pPubcomp)=0;

    virtual void receive_pubrec(msg_pubrec *pPubrec)=0;

    virtual void receive_pubrel(msg_pubrel *pPubrel)=0;

    virtual void receive_subscribe(msg_subscribe *pSubscribe)=0;

    virtual void receive_suback(msg_suback *pSuback)=0;

    virtual void receive_unsubscribe(msg_unsubscribe *pUnsubscribe)=0;

    virtual void receive_unsuback(msg_unsuback *pUnsuback)=0;

    virtual void receive_disconnect(message_header *pHeader)=0;

    virtual void receive_willtopicresp(msg_willtopicresp *pWilltopicresp)=0;

    virtual void receive_willmsgresp(msg_willmsgresp *pWillmsgresp)=0;

    virtual void receive_any(uint8_t *pData)=0;
};


#endif //TEST_MQTT_SN_GATEWAY_MQTTSNRECEIVERINTERFACE_H
