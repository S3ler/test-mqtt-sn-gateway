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
#define FLAG_TOPIC_RESERVED 0x03


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

enum topic_id_type_test : uint8_t {
    TEST_TOPIC_NAME = 0b00,
    TEST_PREDEFINED_TOPIC_ID = 0b01,
    TEST_SHORT_TOPIC_NAME = 0b10,
    TEST_RESERVED = 0b11,
};

struct test_header {
    uint8_t length;
    message_type_test type;
};


#pragma pack(push, 1)

struct test_pingreq {
    uint8_t length = 2;
    message_type_test type = TEST_MQTTSN_PINGREQ;
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_pingreq_wakeup {
    uint8_t length = 2;
    message_type_test type = TEST_MQTTSN_PINGREQ;
    char client_id[24];

    test_pingreq_wakeup(const char* client_id){
        memset(this, 0, sizeof(test_pingreq_wakeup));
        uint8_t client_id_length = (uint8_t) (strlen(client_id) + 1);
        this->length = (uint8_t) (2 + client_id_length);
        this->type = TEST_MQTTSN_PINGREQ;
        memcpy(this->client_id, client_id, client_id_length);
    }

};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_pingresp {
    uint8_t length = 2;
    message_type_test type = TEST_MQTTSN_PINGRESP;
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_advertise {
    uint8_t length = 5;
    message_type_test type = TEST_MQTTSN_ADVERTISE;
    uint8_t gw_id;
    uint16_t duration;

    test_advertise(uint8_t gw_id, uint16_t duration) {
        memset(this, 0, sizeof(test_advertise));
        this->length = 5;
        this->type = TEST_MQTTSN_ADVERTISE;
        this->gw_id = gw_id;
        this->duration = duration;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_searchgw {
    uint8_t length = 3;
    message_type_test type = TEST_MQTTSN_SEARCHGW;
    uint8_t radius = 0;

    test_searchgw(uint8_t radius) {
        memset(this, 0, sizeof(test_searchgw));
        this->length = 3;
        this->type = TEST_MQTTSN_SEARCHGW;
        this->radius = radius;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_gwinfo {
    uint8_t length = 3;
    message_type_test type = TEST_MQTTSN_GWINFO;
    uint8_t gw_id = 0;
    uint8_t gw_addr[UINT16_MAX + UINT8_MAX];

    test_gwinfo(uint8_t gw_id, uint8_t *gw_addr, uint8_t gw_addr_len) {
        memset(this, 0, sizeof(test_gwinfo));
        this->type = TEST_MQTTSN_GWINFO;
        this->gw_id = gw_id;
        if (gw_addr_len == 0) {
            this->length = 3 + 0;
        } else {
            if (gw_addr_len > UINT16_MAX) {
                throw new std::invalid_argument("gatway address longer than UINT16_MAX");
            }
            this->length = (uint8_t) 3 + gw_addr_len;
            memcpy(this->gw_addr, gw_addr, gw_addr_len);
        }
    }

};

#pragma pack(pop)

struct test_connect {
    uint8_t length;
    message_type_test type;
    uint8_t flags;
    uint8_t protocol_id;
    uint16_t duration;
    uint8_t client_id[UINT16_MAX + UINT8_MAX];

    test_connect(bool will, bool clean_session, uint8_t protocol_id, uint16_t duration, const char *client_id) {
        memset(this, 0, sizeof(test_connect));

        uint8_t client_id_length = (uint8_t) strlen(client_id);
        if (client_id_length > UINT16_MAX) {
            throw new std::invalid_argument("client id longer than UINT16_MAX");
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
        memcpy(this->client_id, client_id, client_id_length);
    }

};

struct test_disconnect {
    uint8_t length = 2;
    message_type_test type = TEST_MQTTSN_DISCONNECT;
    uint16_t duration;

    test_disconnect() {
        memset(this, 0, sizeof(test_disconnect));
        length = 2;
        type = TEST_MQTTSN_DISCONNECT;
    }

    test_disconnect(uint16_t duration){
        memset(this, 0, sizeof(test_disconnect));
        length = 2;
        type = TEST_MQTTSN_DISCONNECT;
        this->duration = duration;
    }
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
    uint8_t will_topic[UINT16_MAX + UINT8_MAX];

    test_willtopic(char *willtopic, int8_t qos, bool retain) {
        memset(this, 0, sizeof(test_willtopic));

        uint8_t willtopic_length = (uint8_t) strlen(willtopic);
        if (willtopic_length > UINT16_MAX) {
            throw new std::invalid_argument("willtopic longer than UINT16_MAX");
        }
        this->length = 0;
        if (willtopic_length == 0) {
            length = (uint8_t) (3 + 0);
        } else {
            uint16_t tmp_length = (3 + 1 + willtopic_length);
            if (tmp_length > UINT8_MAX) {
                length = UINT8_MAX;
            }else{
                length = (uint8_t) tmp_length;
            }
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

        memcpy(this->will_topic, willtopic, willtopic_length);

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
            throw new std::invalid_argument("willmsg longer than UINT16_MAX");
        }
        this->type = TEST_MQTTSN_WILLMSG;
        uint16_t tmp_length = (uint16_t) (2 + willmsg_len);
        if (tmp_length > UINT8_MAX) {
            this->length = UINT8_MAX;
        }else{
            this->length = (uint8_t) tmp_length;
        }
        memcpy(this->willmsg, willmsg, willmsg_len);
    }
};

#pragma pack(push, 1)

struct test_publish {
    uint8_t length = 7;
    message_type_test type = TEST_MQTTSN_PUBLISH;
    uint8_t flags;
    uint16_t topic_id;
    uint16_t message_id;
    uint8_t data[UINT16_MAX + UINT8_MAX];

    test_publish(bool dup, int8_t qos, bool retain, bool short_topic, uint16_t topic_id, uint16_t msg_id,
                 const uint8_t *payload, uint8_t payload_len) : topic_id(topic_id), message_id(msg_id) {
        memset(this, 0, sizeof(test_publish));
        if (payload_len > UINT16_MAX) {
            throw new std::invalid_argument("payload longer than UINT16_MAX");
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

    test_puback(uint16_t topic_id, uint16_t msg_id, return_code_test return_code) {
        memset(this, 0, sizeof(test_puback));
        this->length = 7;
        this->type = TEST_MQTTSN_PUBACK;
        this->topic_id = topic_id;
        this->msg_id = msg_id;
        this->return_code = return_code;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_pubrec {
    uint8_t length = 4;
    message_type_test type = TEST_MQTTSN_PUBREC;
    uint16_t msg_id;

    test_pubrec(uint16_t msg_id) {
        memset(this, 0, sizeof(test_pubrec));
        this->length = 4;
        this->type = TEST_MQTTSN_PUBREC;
        this->msg_id = msg_id;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_pubrel {
    uint8_t length = 4;
    message_type_test type = TEST_MQTTSN_PUBREL;
    uint16_t msg_id;

    test_pubrel(uint16_t msg_id) {
        memset(this, 0, sizeof(test_pubrel));
        this->length = 4;
        this->type = TEST_MQTTSN_PUBREL;
        this->msg_id = msg_id;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_pubcomp {
    uint8_t length = 4;
    message_type_test type = TEST_MQTTSN_PUBCOMP;
    uint16_t msg_id;

    test_pubcomp(uint16_t msg_id) {
        memset(this, 0, sizeof(test_pubcomp));
        this->length = 4;
        this->type = TEST_MQTTSN_PUBREC;
        this->msg_id = msg_id;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_register {
    uint8_t length = 6;
    message_type_test type = TEST_MQTTSN_REGISTER;
    uint16_t topic_id;
    uint16_t msg_id;
    uint8_t topic_name[UINT16_MAX + UINT8_MAX];


    test_register(uint16_t topic_id, uint16_t msg_id, const char *topic_name) :
            topic_id(topic_id), msg_id(msg_id) {
        memset(this, 0, sizeof(test_register));
        uint8_t topic_name_length = (uint8_t) strlen(topic_name);
        if (topic_name_length > UINT16_MAX) {
            throw new std::invalid_argument("topic name longer than UINT16_MAX");
        }
        this->length = 0;
        if (topic_name_length == 0) {
            length = (uint8_t) (6 + 0);
        } else {
            uint16_t tmp_length = (uint16_t) ((6 + 1) + topic_name_length);
            if (tmp_length > UINT8_MAX) {
                length = UINT8_MAX;
            }else{
                length = (uint8_t) tmp_length;
            }
        }

        type = TEST_MQTTSN_REGISTER;

        this->topic_id = topic_id;
        this->msg_id = msg_id;
        memcpy(this->topic_name, topic_name, topic_name_length);
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_regack {
    uint8_t length = 7;
    message_type_test type = TEST_MQTTSN_REGACK;
    uint16_t topic_id;
    uint16_t msg_id;
    return_code_test return_code;

    test_regack(uint16_t topic_id, uint16_t msg_id, return_code_test return_code) :
            topic_id(topic_id), msg_id(msg_id), return_code(return_code) {
        memset(this, 0, sizeof(test_regack));
        this->length = 7;
        this->type = TEST_MQTTSN_REGACK;
        this->topic_id = topic_id;
        this->msg_id = msg_id;
        this->return_code = return_code;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_subscribe {
    uint8_t length = 5;
    message_type_test type = TEST_MQTTSN_SUBSCRIBE;
    uint8_t flags;
    uint16_t msg_id;
    uint8_t topic_name[UINT16_MAX + UINT8_MAX];

    test_subscribe(bool dup, int8_t qos, bool retain, bool will, bool clean_session, topic_id_type_test topic_id_type,
                   uint16_t msg_id, const char *topic_name, uint16_t topic_id) {
        memset(this, 0, sizeof(test_publish));

        uint16_t to_copy_length = 0;
        if (topic_id_type == TEST_TOPIC_NAME) {
            uint16_t topic_name_length = (uint8_t) strlen(topic_name);
            if (topic_name_length > UINT16_MAX) {
                throw new std::invalid_argument("topic_name longer than UINT16_MAX");
            }
            to_copy_length = topic_name_length;
            this->length = (uint8_t) (5 + 1 + to_copy_length);
        } else if (topic_id_type == TEST_PREDEFINED_TOPIC_ID || topic_id_type == TEST_SHORT_TOPIC_NAME) {
            to_copy_length = 2;
            this->length = (uint8_t) (5 + 0 + to_copy_length);
        }else {
            to_copy_length = 2;
            this->length = (uint8_t) (5 + 0 + to_copy_length);
        }

        this->type = TEST_MQTTSN_SUBSCRIBE;
        this->flags = 0x0;

        if (dup) {
            this->flags |= FLAG_DUP;
        }
        if (retain) {
            this->flags |= FLAG_RETAIN;
        }

        if (topic_id_type == TEST_TOPIC_NAME) {
            this->flags |= FLAG_TOPIC_NAME;
        } else if (topic_id_type == TEST_PREDEFINED_TOPIC_ID) {
            this->flags |= FLAG_TOPIC_PREDEFINED_ID;
        } else if (topic_id_type == TEST_SHORT_TOPIC_NAME) {
            this->flags |= FLAG_TOPIC_SHORT_NAME;
        } else {
            this->flags |= FLAG_TOPIC_RESERVED;
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

        if (retain) {
            this->flags |= FLAG_RETAIN;
        }

        if (will) {
            this->flags |= FLAG_WILL;
        }

        if (clean_session) {
            this->flags |= FLAG_CLEAN;
        }

        this->msg_id = msg_id;
        if (topic_id_type == TEST_TOPIC_NAME) {
            memcpy(this->topic_name, topic_name, to_copy_length);
        } else if (topic_id_type == TEST_PREDEFINED_TOPIC_ID || topic_id_type == TEST_SHORT_TOPIC_NAME) {
            memcpy(this->topic_name, &topic_id, to_copy_length);
        }
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_suback {
    uint8_t length = 8;
    message_type_test type = TEST_MQTTSN_SUBACK;
    uint8_t flags;
    uint16_t topic_id;
    uint16_t msg_id;
    return_code_test return_code;

    test_suback(int8_t qos, uint16_t topic_id, uint16_t msg_id, return_code_test return_code) :
            topic_id(topic_id), msg_id(msg_id), return_code(return_code) {
        memset(this, 0, sizeof(test_suback));

        this->length = 8;
        this->type = TEST_MQTTSN_SUBACK;

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
        this->msg_id = msg_id;
        this->return_code = return_code;
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_unsubscribe {
    uint8_t length = 5;
    message_type_test type = TEST_MQTTSN_UNSUBSCRIBE;
    uint8_t flags;
    uint16_t msg_id;
    uint8_t topic_name[UINT16_MAX + UINT8_MAX];

    test_unsubscribe(topic_id_type_test topic_id_type, uint16_t msg_id, const char *topic_name, uint16_t topic_id) {
        memset(this, 0, sizeof(test_unsubscribe));

        uint16_t to_copy_length = 0;
        if (topic_id_type == TEST_TOPIC_NAME) {
            uint16_t topic_name_length = (uint8_t) strlen(topic_name);
            if (topic_name_length > UINT16_MAX) {
                throw new std::invalid_argument("topic_name longer than UINT16_MAX");
            }
            to_copy_length = topic_name_length;
            this->length = (uint8_t) (5 + 1 + to_copy_length);
        } else if (topic_id_type == TEST_PREDEFINED_TOPIC_ID || topic_id_type == TEST_SHORT_TOPIC_NAME) {
            to_copy_length = 2;
            this->length = (uint8_t) (5 + 0 + to_copy_length);
        }

        this->type = TEST_MQTTSN_UNSUBSCRIBE;
        this->flags = 0x0;

        if (topic_id_type == TEST_TOPIC_NAME) {
            this->flags |= FLAG_TOPIC_NAME;
        } else if (topic_id_type == TEST_PREDEFINED_TOPIC_ID) {
            this->flags |= FLAG_TOPIC_PREDEFINED_ID;
        } else if (topic_id_type == TEST_SHORT_TOPIC_NAME) {
            this->flags |= FLAG_TOPIC_SHORT_NAME;
        } else {
            this->flags |= FLAG_TOPIC_RESERVED;
        }

        this->msg_id = msg_id;

        if (topic_id_type == TEST_TOPIC_NAME) {
            memcpy(this->topic_name, topic_name, to_copy_length);
        } else if (topic_id_type == TEST_PREDEFINED_TOPIC_ID || topic_id_type == TEST_SHORT_TOPIC_NAME) {
            memcpy(this->topic_name, &topic_id, to_copy_length);
        }
    }
};

#pragma pack(pop)

#pragma pack(push, 1)

struct test_unsuback {
    uint8_t length = 4;
    message_type_test type = TEST_MQTTSN_UNSUBACK;
    uint16_t msg_id;

    test_unsuback(uint16_t msg_id) : msg_id(msg_id) {
        memset(this, 0, sizeof(test_unsuback));
        this->length = 4;
        this->type = TEST_MQTTSN_UNSUBACK;
        this->msg_id = msg_id;
    }
};

#pragma pack(pop)

#endif //TEST_MQTT_SN_GATEWAY_TEST_MQTTSN_TEST_MESSAGES_H
