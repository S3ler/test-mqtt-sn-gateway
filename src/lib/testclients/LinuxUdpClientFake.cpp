#include <iostream>
#include <zconf.h>
#include <thread>
#include <FakeClientSockets/FakeUdpSocket.h>


void LinuxUdpClientFake::send_searchgw(uint8_t radius) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_searchgw msg(radius);

    if (sendto(socket_descriptor, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Radius" << std::endl;
    }
}


void LinuxUdpClientFake::send_connect(const char *client_id, uint16_t duration, bool clean_session, bool will) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_connect msg(will, clean_session, PROTOCOL_ID, duration, client_id);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Publish" << std::endl;
    }

}

void LinuxUdpClientFake::send_willtopic(const char *willtopic, uint8_t qos, bool retain) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }
    test_willtopic msg((char *) willtopic, qos, retain);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Willtopic" << std::endl;
    }
}

void LinuxUdpClientFake::send_willmsg(const uint8_t *willmsg, uint8_t willmsg_len) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_willmsg msg(willmsg, willmsg_len);
    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Willtopic" << std::endl;
    }
}

void LinuxUdpClientFake::send_publish(bool dup, int8_t qos, bool retain, bool short_topic, uint16_t topic_id,
                                      uint16_t msg_id, const uint8_t *data, uint8_t data_length) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_publish msg(dup, qos, retain, short_topic, topic_id, msg_id, data, data_length);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Publish" << std::endl;
    }

}


void LinuxUdpClientFake::send_puback(uint8_t topic_id, uint16_t msg_id, return_code_test return_code) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_puback msg(topic_id, msg_id, return_code);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}

void LinuxUdpClientFake::send_pubrec(uint16_t msg_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_pubrec msg(msg_id);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}

void LinuxUdpClientFake::send_pubrel(uint16_t msg_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_pubrel msg(msg_id);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}

void LinuxUdpClientFake::send_pubcomp(uint16_t msg_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_pubcomp msg(msg_id);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}


void LinuxUdpClientFake::send_register(uint16_t topic_id, uint16_t msg_id, const char *topic_name) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_register msg(topic_id, msg_id, topic_name);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Register" << std::endl;
    }
}

void LinuxUdpClientFake::send_subscribe(bool dup, uint8_t qos, bool retain, bool will, bool clean_session,
                                        topic_id_type_test topic_id_type,
                                        uint16_t msg_id, const char *topic_name, uint16_t topic_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_subscribe msg(dup, qos, retain, will, clean_session, topic_id_type, msg_id, topic_name, topic_id);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Subscribe" << std::endl;
    }
}


void LinuxUdpClientFake::send_unsubscribe(topic_id_type_test topic_id_type, uint16_t msg_id, const char *topic_name,
                                          uint16_t topic_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_unsubscribe msg(topic_id_type, msg_id, topic_name, topic_id);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Unubscribe" << std::endl;
    }
}

void LinuxUdpClientFake::send_disconnect() {
    this->send_disconnect(2);
}


void LinuxUdpClientFake::send_disconnect(uint8_t length) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_disconnect msg;
    msg.length = length;

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Disconnect" << std::endl;
    }
}

void LinuxUdpClientFake::send_disconnect(uint16_t duration, uint8_t length) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_disconnect msg(duration);
    msg.length = length;

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Disconnect" << std::endl;
    }
}

void LinuxUdpClientFake::send_pingreq() {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_pingreq msg;

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Pingreq" << std::endl;
    }
}

void LinuxUdpClientFake::send_pingreq(const char* client_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_pingreq_wakeup msg(client_id);

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Pingreq" << std::endl;
    }
}

void LinuxUdpClientFake::/**/send_pingreq(const char *client_id, uint8_t length) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (s == -1) {
        connect(this->gw_address);
    }

    test_pingreq_wakeup msg(client_id);
    msg.length = 2+length;

    if (sendto(s, (const void *) &msg, msg.length, 0, (struct sockaddr *) &si_other, slen) == -1) {
        std::cout << "sendto()Pingreq" << std::endl;
    }
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

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
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
    tv.tv_sec = 0;  // 0 Secs Timeout
    tv.tv_usec = 200000;  // 200 ms Timeout


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

    // broadcast init
    static int port1 = 1234;

    static struct ip_mreq command;
    int loop = 1;
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port1);
    if ((socket_descriptor = socket(PF_INET,
                                    SOCK_DGRAM, 0)) == -1) {
        perror("socket()");
        exit(1);
        //exit(EXIT_FAILURE);
    }
    /* Mehr Prozessen erlauben, denselben Port zu nutzen */
    loop = 1;
    if (setsockopt(socket_descriptor,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &loop, sizeof(loop)) < 0) {
        perror("setsockopt:SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    if (bind(socket_descriptor,
             (struct sockaddr *) &sin,
             sizeof(sin)) < 0) {
        perror("bind");
        exit(1);


    }
    /* Broadcast auf dieser Maschine zulassen */
    loop = 1;
    if (setsockopt(socket_descriptor,
                   IPPROTO_IP,
                   IP_MULTICAST_LOOP,
                   &loop, sizeof(loop)) < 0) {
        perror("setsockopt:IP_MULTICAST_LOOP");
        exit(1);

    }


    /* Join the broadcast group: */
    command.imr_multiaddr.s_addr = inet_addr("224.0.0.0");
    command.imr_interface.s_addr = htonl(INADDR_ANY);
    if (command.imr_multiaddr.s_addr == -1) {
        perror("224.0.0.0 ist keine Multicast-Adresse\n");
        exit(1);
        //exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_descriptor,
                   IPPROTO_IP,
                   IP_ADD_MEMBERSHIP,
                   &command, sizeof(command)) < 0) {
        perror("setsockopt:IP_ADD_MEMBERSHIP");
        exit(1);

    }

    // set timout
    struct timeval udp_socket_timeval;
    udp_socket_timeval.tv_sec = 0;  // 0 Secs Timeout
    udp_socket_timeval.tv_usec = 200000;  // 200 ms Timeout


    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *) &udp_socket_timeval, sizeof(struct timeval)) ==
        -1) {
        exit(1);

    }

    // save bc address1 as value now
    struct sockaddr_in address1;
    memset(&address1, 0, sizeof(address1));
    address1.sin_family = AF_INET;
    address1.sin_addr.s_addr = inet_addr("224.0.0.0");
    address1.sin_port = htons(port1);

    device_address bc_address1 = this->getDevice_address(&address1);
    memset(&broadcast_address, 0, sizeof(device_address));
    memcpy(this->broadcast_address.bytes, bc_address1.bytes, sizeof(device_address));

}

void LinuxUdpClientFake::disconnect() {
    close(s);
    s = -1;
}

void LinuxUdpClientFake::loop() {
    while (!stopped) {
        int recv_len;
        memset(&buf, 0, BUFLEN);
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
            device_address client_address = getDevice_address(&si_other);
            this->receive((uint8_t *) buf, (uint16_t) recv_len);
        }
        memset(&buf, 0, BUFLEN);
        if ((recv_len = recvfrom(socket_descriptor, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
            device_address client_address = getDevice_address(&si_other);
            this->receive((uint8_t *) buf, (uint16_t) recv_len);
        }
    }
}

void LinuxUdpClientFake::start_loop() {
    fakeSocket = new FakeUdpSocket();
    fakeSocket->setFakeClient(this);
    this->thread = std::thread(&LinuxUdpClientFake::loop, this);
    this->thread.detach();
    free(fakeSocket);
}

void LinuxUdpClientFake::stop_loop() {
    this->stopped = true;
}


void LinuxUdpClientFake::receive(uint8_t *data, uint16_t length) {
    if (receiver == nullptr) {
        throw new std::invalid_argument("mqtt sn receiver not initialized");
    }
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    test_header *header = (test_header *) data;
    switch (header->type) {
        case TEST_MQTTSN_ADVERTISE:
            this->receiver->receive_advertise((test_advertise *) data);
            break;
        case TEST_MQTTSN_SEARCHGW:
            this->receiver->receive_searchgw((test_searchgw *) data);
            break;
        case TEST_MQTTSN_GWINFO:
            this->receiver->receive_gwinfo((test_gwinfo *) data);
            break;
        case TEST_MQTTSN_CONNECT:
            this->receiver->receive_connect((test_connect *) data);
            break;
        case TEST_MQTTSN_DISCONNECT:
            this->receiver->receive_disconnect((test_disconnect *) data);
            break;
        case TEST_MQTTSN_CONNACK:
            this->receiver->receive_connack((test_connack *) data);
            break;
        case TEST_MQTTSN_WILLTOPICREQ:
            this->receiver->receive_willtopicreq((test_willtopicreq *) data);
            break;
        case TEST_MQTTSN_WILLTOPIC:
            this->receiver->receive_willtopic((test_willtopic *) data);
            break;
        case TEST_MQTTSN_WILLMSGREQ:
            this->receiver->receive_willmsgreq((test_willmsgreq *) data);
            break;
        case TEST_MQTTSN_WILLMSG:
            this->receiver->receiver_willmsg((test_willmsg *) data);
            break;
        case TEST_MQTTSN_PUBLISH:
            this->receiver->receive_publish((test_publish *) data);
            break;
        case TEST_MQTTSN_PUBACK:
            this->receiver->receive_puback((test_puback *) data);
            break;
        case TEST_MQTTSN_PUBREC:
            this->receiver->receive_pubrec((test_pubrec *) data);
            break;
        case TEST_MQTTSN_PUBREL:
            this->receiver->receive_pubrel((test_pubrel *) data);
            break;
        case TEST_MQTTSN_PUBCOMP:
            this->receiver->receive_pubcomp((test_pubcomp *) data);
            break;
        case TEST_MQTTSN_REGISTER:
            this->receiver->receiver_register((test_register *) data);
            break;
        case TEST_MQTTSN_REGACK:
            this->receiver->receive_regack((test_regack *) data);
            break;
        case TEST_MQTTSN_SUBSCRIBE:
            this->receiver->receive_subscribe((test_subscribe *) data);
            break;
        case TEST_MQTTSN_SUBACK:
            this->receiver->receive_suback((test_suback *) data);
            break;
        case TEST_MQTTSN_UNSUBSCRIBE:
            this->receiver->receive_unsubscribe((test_unsubscribe *) data);
            break;
        case TEST_MQTTSN_UNSUBACK:
            this->receiver->receive_unsuback((test_unsuback *) data);
            break;
        case TEST_MQTTSN_PINGREQ:
            this->receiver->receive_pingreq((test_pingreq *) data);
            break;
        case TEST_MQTTSN_PINGRESP:
            this->receiver->receive_pingresp((test_pingresp *) data);
            break;
            /*
           case TEST_MQTTSN_WILLTOPICUPD:
               //this->receiver->receive_willtopicudp((msg_willtopicudp *) data);
               // TODO
               throw new std::invalid_argument("not implemented yet");
               break;
           case TEST_MQTTSN_WILLTOPICRESP:
               this->receiver->receive_willtopicresp((msg_willtopicresp *) data);
               break;
           case TEST_MQTTSN_WILLMSGUPD:
               //this->receiver->receive_willmsgudp((msg_willmsgupd*) data);
               // TODO
               throw new std::invalid_argument("not implemented yet");
               break;
           case TEST_MQTTSN_WILLMSGRESP:
               this->receiver->receive_willmsgresp((msg_willmsgresp *) data);
               break;
                */
        default:
            this->receiver->receive_any_message((uint16_t) header->length, header->type, data);
            break;
    }
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

void LinuxUdpClientFake::set_gw_address(device_address *address) {
    this->gw_address = address;
}


void LinuxUdpClientFake::setMqttSnReceiver(MqttSnReceiverInterface *receiverInterface) {
    this->receiver = receiverInterface;
}

