//
// Created by bele on 17.04.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_TEST_MQTTSN_TEST_MESSAGES_H
#define TEST_MQTT_SN_GATEWAY_TEST_MQTTSN_TEST_MESSAGES_H

#include <string.h>
#include <stdexcept>

#define PROTOCOL_ID 0x01

#define FLAG_NO_FLAGS 0x00
#define FLAG_DUP 0x80
#define FLAG_QOS_0 0x00
#define FLAG_QOS_1 0x20
#define FLAG_QOS_2 0x40
#define FLAG_QOS_M1 0x60
#define FLAG_RETAIN 0x10
#define FLAG_WILL 0x08
#define FLAG_CLEAN 0x04
#define FLAG_TOPIC_NAME 0x00
#define FLAG_TOPIC_PREDEFINED_ID 0x01
#define FLAG_TOPIC_SHORT_NAME 0x02

enum return_code_test : uint8_t {
    TEST_ACCEPTED = 0x00,
    TEST_REJECTED_CONGESTION = 0x01,
    TEST_REJECTED_INVALID_TOPIC_ID = 0x02,
    TEST_REJECTED_NOT_SUPPORTED = 0x03
};

enum message_type_test : uint8_t {
    TEST_MQTTSN_ADVERTISE,
    TEST_MQTTSN_SEARCHGW,
    TEST_MQTTSN_GWINFO,
    TEST_MQTTSN_CONNECT = 0x04,
    TEST_MQTTSN_CONNACK,
    TEST_MQTTSN_WILLTOPICREQ,
    TEST_MQTTSN_WILLTOPIC,
    TEST_MQTTSN_WILLMSGREQ,
    TEST_MQTTSN_WILLMSG,
    TEST_MQTTSN_REGISTER,
    TEST_MQTTSN_REGACK,
    TEST_MQTTSN_PUBLISH,
    TEST_MQTTSN_PUBACK,
    TEST_MQTTSN_PUBCOMP,
    TEST_MQTTSN_PUBREC,
    TEST_MQTTSN_PUBREL,
    TEST_MQTTSN_SUBSCRIBE = 0x12,
    TEST_MQTTSN_SUBACK,
    TEST_MQTTSN_UNSUBSCRIBE,
    TEST_MQTTSN_UNSUBACK,
    TEST_MQTTSN_PINGREQ = 0x16,
    TEST_MQTTSN_PINGRESP,
    TEST_MQTTSN_DISCONNECT,
    TEST_MQTTSN_WILLTOPICUPD = 0x1a,
    TEST_MQTTSN_WILLTOPICRESP,
    TEST_MQTTSN_WILLMSGUPD,
    TEST_MQTTSN_WILLMSGRESP
};

struct test_header {
    uint8_t length;
    message_type_test type;
};

struct test_connect {
    uint8_t length;
    message_type_test type;
    uint8_t flags;
    uint8_t protocol_id;
    uint16_t duration;
    char client_id[UINT16_MAX + UINT8_MAX];

    test_connect(bool will, bool clean_session, uint8_t protocol_id, uint16_t duration, const char *client_id) {
        memset(this, 0, sizeof(test_connect));

        uint8_t client_id_length = (uint8_t) strlen(client_id);
        if (client_id_length > UINT16_MAX) {
            throw new std::invalid_argument("client id longer then UINT16_MAX");
        }
        if (client_id_length == 0) {
            length = ((uint8_t) (6 + 0));
        } else {
            length = ((uint8_t) (6 + 1)) + client_id_length;
        }
        type = TEST_MQTTSN_CONNECT;
        flags = 0x0;
        if (will) {
            flags |= FLAG_WILL;
        }
        if (clean_session) {
            flags |= FLAG_CLEAN;
        }
        this->protocol_id = protocol_id;
        this->duration = duration;
        strcpy(this->client_id, client_id);
    }

};

struct test_disconnect {
    uint8_t length = 2;
    message_type_test type = TEST_MQTTSN_DISCONNECT;
};

struct test_connack {
    uint8_t length = 3;
    message_type_test type = TEST_MQTTSN_CONNACK;
    return_code_test return_code;

    test_connack(return_code_test return_code) : return_code(return_code) {
        length = sizeof(test_connack);
        type = TEST_MQTTSN_CONNACK;
        this->return_code = return_code;
    }
};


struct test_willtopicreq {
    uint8_t length = 2;
    message_type_test type = TEST_MQTTSN_WILLTOPICREQ;
};

struct test_willtopic {
    uint8_t length;
    message_type_test type;
    uint8_t flags;
    char will_topic[UINT16_MAX + UINT8_MAX];

    test_willtopic(char *willtopic, int8_t qos, bool retain) {
        memset(this, 0, sizeof(test_willtopic));

        uint8_t willtopic_length = (uint8_t) strlen(willtopic);
        if (willtopic_length > UINT16_MAX) {
            throw new std::invalid_argument("willtopic longer then UINT16_MAX");
        }
        this->length = 0;
        if (willtopic_length == 0) {
            length = (uint8_t) (3 + 0);
        } else {
            length = (uint8_t) (3 + 1) + willtopic_length;
        }

        type = TEST_MQTTSN_WILLTOPIC;
        this->flags = 0;
        if (retain) {
            this->flags |= FLAG_RETAIN;
        }
        if (qos == 0) {
            this->flags |= FLAG_QOS_0;
        } else if (qos == 1) {
            this->flags |= FLAG_QOS_1;
        } else if (qos == 2) {
            this->flags |= FLAG_QOS_2;
        }

        strcpy(this->will_topic, willtopic);

    }
};

struct test_willmsgreq {
    uint8_t length = 2;
    message_type_test type = TEST_MQTTSN_WILLMSGREQ;
};

struct test_willmsg {
    uint8_t length;
    message_type_test type;
    uint8_t willmsg[UINT16_MAX + UINT8_MAX];

    test_willmsg(const uint8_t *willmsg, uint16_t willmsg_len) {
        memset(this, 0, sizeof(test_willmsg));
        if (willmsg_len > UINT16_MAX) {
            throw new std::invalid_argument("willmsg longer then UINT16_MAX");
        }
        this->type = TEST_MQTTSN_WILLMSG;
        this->length = (uint8_t) (2 + willmsg_len);
        memcpy(this->willmsg, willmsg, willmsg_len);
    }
};

#pragma pack(push, 1)

struct test_publish {
    uint8_t length;
    message_type_test type;
    uint8_t flags;
    uint16_t topic_id;
    uint16_t message_id;
    uint8_t data[UINT16_MAX + UINT8_MAX];

    test_publish(bool dup, int8_t qos, bool retain, bool short_topic, uint16_t topic_id, uint16_t msg_id,
                 const uint8_t *payload, uint8_t payload_len) : topic_id(topic_id), message_id(msg_id) {
        memset(this, 0, sizeof(test_publish));
        if (payload_len > UINT16_MAX) {
            throw new std::invalid_argument("payload longer then UINT16_MAX");
        }
        this->length = ((uint8_t) 7) + payload_len;
        this->type = TEST_MQTTSN_PUBLISH;
        this->flags = 0x0;
        if (dup) {
            this->flags |= FLAG_DUP;
        }
        if (retain) {
            this->flags |= FLAG_RETAIN;
        }
        if (short_topic) {
            this->flags |= FLAG_TOPIC_SHORT_NAME;
        } else {
            this->flags |= FLAG_TOPIC_PREDEFINED_ID;
        }
        if (qos == 0) {
            this->flags |= FLAG_QOS_0;
        } else if (qos == 1) {
            this->flags |= FLAG_QOS_1;
        } else if (qos == 2) {
            this->flags |= FLAG_QOS_2;
        } else if (qos == -1) {
            this->flags |= FLAG_QOS_M1;
        }
        this->topic_id = topic_id;
        this->message_id = msg_id;
        memcpy(this->data, payload, payload_len);
    }
};

#pragma pack(pop)
#pragma pack(push, 1)

struct test_puback {
    uint8_t length = 7;
    message_type_test type = TEST_MQTTSN_PUBACK;
    uint16_t topic_id;
    uint16_t msg_id;
    return_code_test return_code;

    test_puback(uint16_t topic_id, uint16_t msg_id, return_code_test return_code)
            : topic_id(topic_id), msg_id(msg_id), return_code(return_code) {
        length = 7;
        type = TEST_MQTTSN_PUBACK;
    }
};
#pragma pack(pop)

#endif //TEST_MQTT_SN_GATEWAY_TEST_MQTTSN_TEST_MESSAGES_H
