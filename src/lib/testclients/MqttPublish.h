//
// Created by bele on 11.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_MQTTPUBLISH_H
#define TEST_MQTT_SN_GATEWAY_MQTTPUBLISH_H

#include <string>
#include <vector>

class MqttPublish{
public:
    std::string topic;
    std::vector<uint8_t> data;

    MqttPublish(const char* topic, const uint8_t * payload, uint16_t payload_length):topic(std::string(topic)){
        for (uint16_t i = 0; i < payload_length; i++) {
            data.push_back(*(payload + i));
        }
    }
};


#endif //TEST_MQTT_SN_GATEWAY_MQTTPUBLISH_H
