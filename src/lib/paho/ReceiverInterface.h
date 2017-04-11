//
// Created by bele on 11.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_RECEIVERINTERFACE_H
#define TEST_MQTT_SN_GATEWAY_RECEIVERINTERFACE_H

#include <global_defines.h>
#include "PahoMqttTestMessageHandler.h"
#include "MqttPublish.h"


class ReceiverInterface {
public:
    virtual ~ReceiverInterface() {};
    virtual bool receive(MqttPublish source)=0;

};
#endif //TEST_MQTT_SN_GATEWAY_RECEIVERINTERFACE_H
