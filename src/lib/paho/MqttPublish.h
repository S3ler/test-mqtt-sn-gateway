//
// Created by bele on 11.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_MQTTPUBLISH_H
#define TEST_MQTT_SN_GATEWAY_MQTTPUBLISH_H

#include <string>

class MqttPublish{
public:
    std::string topic;
    std::string data;

    MqttPublish(const char* topic, const char* data):topic(std::string(topic)),data(std::string(data)){

    }
};


#endif //TEST_MQTT_SN_GATEWAY_MQTTPUBLISH_H
