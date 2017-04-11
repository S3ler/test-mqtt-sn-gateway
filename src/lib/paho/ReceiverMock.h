//
// Created by bele on 11.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_RECEIVERMOCK_H
#define TEST_MQTT_SN_GATEWAY_RECEIVERMOCK_H


#include <gmock/gmock-generated-function-mockers.h>
#include <gmock/gmock-generated-matchers.h>
#include "ReceiverInterface.h"


// MATCHER_P(received_equal, data, "") { return (memcmp(arg, data, data[0]) == 0); }

class ReceiverMock : public ReceiverInterface {
public:
    MOCK_METHOD1(receive, bool(MqttPublish source));
};


#endif //TEST_MQTT_SN_GATEWAY_RECEIVERMOCK_H
