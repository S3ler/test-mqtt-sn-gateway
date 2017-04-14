#include <iostream>
#include <zconf.h>
#include <thread>
#include "LinuxUdpClientFake.h"

void
LinuxUdpClientFake::send_connect(device_address *target_address, const char *client_id, uint16_t duration,
                                 bool clean_session,
                                 bool will, bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }

    msg_connect msg(will, clean_session, PROTOCOL_ID, duration, client_id);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Publish" << std::endl;
    }
    if (close_connection) {
        disconnect();
    }
}


bool
LinuxUdpClientFake::receive_connack(device_address *target_address, return_code_t return_code, bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                msg_connack *msg = (msg_connack *) buf;
                if (msg->length == 3 && msg->type == MQTTSN_CONNACK && msg->return_code == return_code) {
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }
    if (close_connection) {
        disconnect();
    }
    return result;
}


device_address LinuxUdpClientFake::getDevice_address(sockaddr_in *addr) const {
    device_address address;
    memset(&address, 0, sizeof(address));
    {
        uint32_t ip_address = addr->sin_addr.s_addr;
        uint16_t port = addr->sin_port;

        memcpy(&address.bytes, &ip_address, sizeof(ip_address));
        if (port == 0) {
            port = PORT;
        }
        memcpy(&address.bytes[sizeof(ip_address)], &port, sizeof(port));

    }
    return address;
}

void LinuxUdpClientFake::send_disconnect(device_address *target_address, bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }

    message_header msg;
    msg.to_disconnect();

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Disconnect" << std::endl;
    }
    if (close_connection) {
        disconnect();
    }
}


bool LinuxUdpClientFake::receive_disconnect(device_address *target_address, bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                message_header *msg = (message_header *) buf;
                if (msg->length == 2 && msg->type == MQTTSN_DISCONNECT) {
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }

    if (close_connection) {
        disconnect();
    }
    return result;
}


void LinuxUdpClientFake::send_publish(device_address *target_address, const uint8_t *data, uint8_t data_length,
                                      uint16_t topic_id, int8_t qos, bool short_topic, bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }

    msg_publish msg(false, qos, false, short_topic, topic_id, 0, data, data_length);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Publish" << std::endl;
    }

    if (close_connection) {
        disconnect();
    }
}

void LinuxUdpClientFake::send_register(device_address *target_address, uint16_t msg_id, const char *topic_name,
                                       bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }
    msg_register msg(0, msg_id, topic_name);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Register" << std::endl;
    }
    if (close_connection) {
        disconnect();
    }
}

bool LinuxUdpClientFake::receive_regack(device_address *target_address, uint16_t msg_id, uint16_t *new_topic_id,
                                        return_code_t return_code, bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                msg_regack *msg = (msg_regack *) buf;
                if (msg->length == 7 && msg->type == MQTTSN_REGACK && msg->message_id == msg_id &&
                    msg->return_code == return_code) {
                    if (msg->topic_id == 0) {
                        result = false;
                    }
                    *new_topic_id = msg->topic_id;
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }

    if (close_connection) {
        disconnect();
    }
    return result;
}


void LinuxUdpClientFake::connect(device_address *address) {

    memcpy(&this->address, address, sizeof(device_address));

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    // si_other.sin_port = htons(PORT);

    uint32_t ip_address = 0;
    memcpy(&ip_address, &address->bytes, sizeof(ip_address));
    si_other.sin_addr.s_addr = ip_address;

    uint16_t port = 0;
    memcpy(&port, &address->bytes[sizeof(uint32_t)], sizeof(port));
    si_other.sin_port = port;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        std::cout << "socket" << std::endl;
        exit(1);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(fromport);
    sin.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) == -1) {
        std::cout << "bind" << std::endl;
        exit(1);
    }

    // set timout
    struct timeval tv;
    tv.tv_sec = timout_seconds;
    tv.tv_usec = timout_micro_s;


    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval)) == -1) {
        std::cout << "set timeout" << std::endl;
        exit(1);
    }

    // enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        std::cout << "broadcastEnable" << std::endl;
        exit(1);
    }
}

void LinuxUdpClientFake::disconnect() {
    close(s);
    s = -1;
}

bool LinuxUdpClientFake::receive_puback(device_address *target_address, uint16_t topic_id, return_code_t return_code,
                                        bool close_connection) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                msg_puback *msg = (msg_puback *) buf;
                if (msg->length == 7 && msg->type == MQTTSN_PUBACK && msg->topic_id == topic_id &&
                    msg->message_id == 0 &&
                    msg->return_code == return_code) {
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }

    if (close_connection) {
        disconnect();
    }
    return result;
}

bool LinuxUdpClientFake::receive_willtopicreq(device_address *target_address) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                message_header *msg = (message_header *) buf;
                if (msg->length == 2 && msg->type == MQTTSN_WILLTOPICREQ) {
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }

    return result;
}

bool LinuxUdpClientFake::receive_willmsgreq(device_address *target_address) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                message_header *msg = (message_header *) buf;
                if (msg->length == 2 && msg->type == MQTTSN_WILLMSGREQ) {
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }
    return result;
}

void
LinuxUdpClientFake::send_willtopic(device_address *target_address, const char *willtopic, uint8_t qos, bool retain) {
    if (s == -1) {
        connect(target_address);
    }
    msg_willtopic msg(willtopic, qos, retain);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()willtopic" << std::endl;
    }
}

void LinuxUdpClientFake::send_willmsg(device_address *target_address, const char *willmsg) {
    if (s == -1) {
        connect(target_address);
    }
    msg_willmsg msg((const uint8_t *) willmsg, strlen(willmsg) + 1);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()willtopic" << std::endl;
    }
}

void
LinuxUdpClientFake::send_subscribe(device_address *target_address, bool short_topic, uint16_t topic_id, uint16_t msg_id,
                                   uint8_t qos) {
    if (s == -1) {
        connect(target_address);
    }
    msg_subscribe_shorttopic msg(short_topic, topic_id, msg_id, qos, false);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()subscribe" << std::endl;
    }
}


void
LinuxUdpClientFake::send_subscribe(device_address *target_address, char *topic_name, uint16_t msg_id, uint8_t qos) {
    if (s == -1) {
        connect(target_address);
    }
    msg_subscribe_topicname msg(topic_name, msg_id, qos, false);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()subscribe" << std::endl;
    }
}

bool LinuxUdpClientFake::receive_suback(device_address *target_address, uint16_t topic_id, uint16_t msg_id,
                                        uint8_t *granted_qos,
                                        return_code_t return_code) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                msg_suback *msg = (msg_suback *) buf;
                if (msg->length == 8 && msg->type == MQTTSN_SUBACK && msg->return_code == return_code &&
                    msg->topic_id == topic_id && msg->message_id == msg_id) {
                    int8_t qos = 0;
                    if ((msg->flags & FLAG_QOS_2) == FLAG_QOS_2) {
                        qos = 2;
                    } else if ((msg->flags & FLAG_QOS_1) == FLAG_QOS_1) {
                        qos = 1;
                    } else {
                        qos = 0;
                    }
                    *granted_qos = (uint8_t) qos;
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }
    return result;
}

bool LinuxUdpClientFake::receive_suback(device_address *target_address, uint16_t *new_topic_id, uint16_t msg_id,
                                        uint8_t *granted_qos,
                                        return_code_t return_code) {

    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                msg_suback *msg = (msg_suback *) buf;
                if (msg->length == 8 && msg->type == MQTTSN_SUBACK && msg->return_code == return_code &&
                    msg->message_id == msg_id) {
                    *new_topic_id = msg->topic_id;
                    int8_t qos = 0;
                    if ((msg->flags & FLAG_QOS_M1) == FLAG_QOS_M1) {
                        qos = -1;
                    } else if ((msg->flags & FLAG_QOS_2) == FLAG_QOS_2) {
                        qos = 2;
                    } else if ((msg->flags & FLAG_QOS_1) == FLAG_QOS_1) {
                        qos = 1;
                    } else {
                        qos = 0;
                    }
                    *granted_qos = (uint8_t) qos;
                    result = true;
                }
            }
        }

    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }
    return result;
}

bool LinuxUdpClientFake::receive_any_publish(device_address *target_address, uint8_t *data, uint8_t *data_length,
                                             uint16_t *topic_id, uint8_t *qos) {
    if (s == -1) {
        connect(target_address);
    }
    bool result = false;
    int recv_len;
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            if (memcmp(&client_address, target_address, sizeof(device_address)) == 0) {
                msg_publish *msg = (msg_publish *) buf;
                int8_t _qos = 0;
                if ((msg->flags & FLAG_QOS_M1) == FLAG_QOS_M1) {
                    _qos = -1;
                } else if ((msg->flags & FLAG_QOS_2) == FLAG_QOS_2) {
                    _qos = 2;
                } else if ((msg->flags & FLAG_QOS_1) == FLAG_QOS_1) {
                    _qos = 1;
                } else {
                    _qos = 0;
                }
                *qos = (uint8_t) _qos;
                *topic_id = msg->topic_id;
                *data_length = msg->length - 7;
                memcpy(data, msg->data, *data_length);
                result = true;
            }
        }
    } else {
        std::cout << "recvfrom()" << std::endl;
        exit(1);
    }
    return result;
}

void LinuxUdpClientFake::setMqttSnReceiver(MqttSnReceiverInterface *receiverInterface) {
    this->receiver = receiverInterface;
}

void LinuxUdpClientFake::start_loop() {
    this->thread = std::thread(&LinuxUdpClientFake::loop, this);
    this->thread.detach();
}

void LinuxUdpClientFake::stop_loop() {
    this->stopped = true;
}

void LinuxUdpClientFake::loop() {
    while (!stopped) {
        int recv_len;
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
            device_address client_address = getDevice_address(&si_other);
            this->receive((uint8_t *) buf, (uint16_t) recv_len);
        }
    }
}

void LinuxUdpClientFake::receive(uint8_t *data, uint16_t length) {
    if (receiver == nullptr) {
        throw new std::invalid_argument("mqtt sn receiver not initialized");
    }
    message_header *header = (message_header *) data;
    switch (header->type) {
        /*
        case MQTTSN_ADVERTISE:
            break;
        case MQTTSN_SEARCHGW:
            break;
        */
        case MQTTSN_CONNECT:
            this->receiver->receive_connect((msg_connect *) data);
            break;
        case MQTTSN_CONNACK:
            this->receiver->receive_connack((msg_connack *) header);
            break;
        case MQTTSN_WILLTOPICREQ:
            this->receiver->receive_willtopicreq(header);
            break;
        case MQTTSN_WILLTOPIC:
            this->receiver->receive_willtopic((msg_willtopic *) data);
            break;
        case MQTTSN_WILLMSGREQ:
            this->receiver->receive_willmsgreq(header);
            break;
        case MQTTSN_WILLMSG:
            this->receiver->receiver_willmsg((msg_willmsg *) data);
            break;
        case MQTTSN_REGISTER:
            this->receiver->receiver_register((msg_register *) data);
            break;
        case MQTTSN_REGACK:
            this->receiver->receive_regack((msg_regack *) data);
            break;
        case MQTTSN_PUBLISH:
            this->receiver->receive_publish((msg_publish *) data);
            break;
        case MQTTSN_PUBACK:
            this->receiver->receive_puback((msg_puback *) data);
            break;
        case MQTTSN_PUBCOMP:
            this->receiver->receive_pubcomp((msg_pubcomp *) data);
            break;
        case MQTTSN_PUBREC:
            this->receiver->receive_pubrec((msg_pubrec *) data);
            break;
        case MQTTSN_PUBREL:
            this->receiver->receive_pubrel((msg_pubrel *) data);
            break;
        case MQTTSN_SUBSCRIBE:
            this->receiver->receive_subscribe((msg_subscribe *) data);
            break;
        case MQTTSN_SUBACK:
            this->receiver->receive_suback((msg_suback *) data);
            break;
        case MQTTSN_UNSUBSCRIBE:
            this->receiver->receive_unsubscribe((msg_unsubscribe *) data);
            break;
        case MQTTSN_UNSUBACK:
            this->receiver->receive_unsuback((msg_unsuback *) data);
            break;
        case MQTTSN_PINGREQ:
            this->receiver->receive_pingreq(header);
            break;
        case MQTTSN_PINGRESP:
            this->receiver->receive_pingresp(header);
            break;
        case MQTTSN_DISCONNECT:
            this->receiver->receive_disconnect(header);
            break;
        case MQTTSN_WILLTOPICUPD:
            //this->receiver->receive_willtopicudp((msg_willtopicudp *) data);
            // TODO
            throw new std::invalid_argument("not implemented yet");
            break;
        case MQTTSN_WILLTOPICRESP:
            this->receiver->receive_willtopicresp((msg_willtopicresp *) data);
            break;
        case MQTTSN_WILLMSGUPD:
            //this->receiver->receive_willmsgudp((msg_willmsgupd*) data);
            // TODO
            throw new std::invalid_argument("not implemented yet");
            break;
        case MQTTSN_WILLMSGRESP:
            this->receiver->receive_willmsgresp((msg_willmsgresp *) data);
            break;
        default:
            this->receiver->receive_any_message((message_type) header->type);
            break;
    }
}



