#include <iostream>
#include <zconf.h>
#include <thread>
#include <FakeClientSockets/FakeUdpSocket.h>


void LinuxUdpClientFake::send_searchgw(uint8_t radius) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_searchgw msg(radius);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Radius" << std::endl;
    }
}


void LinuxUdpClientFake::send_connect(const char *client_id, uint16_t duration, bool clean_session, bool will) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_connect msg(will, clean_session, PROTOCOL_ID, duration, client_id);
    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Publish" << std::endl;
    }

}

void LinuxUdpClientFake::send_willtopic(const char *willtopic, uint8_t qos, bool retain) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }
    test_willtopic msg((char *) willtopic, qos, retain);
    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Willtopic" << std::endl;
    }
}

void LinuxUdpClientFake::send_willmsg(const uint8_t *willmsg, uint8_t willmsg_len) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_willmsg msg(willmsg, willmsg_len);
    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Willtopic" << std::endl;
    }
}

void LinuxUdpClientFake::send_publish(bool dup, int8_t qos, bool retain, bool short_topic, uint16_t topic_id,
                                      uint16_t msg_id, const uint8_t *data, uint8_t data_length) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_publish msg(dup, qos, retain, short_topic, topic_id, msg_id, data, data_length);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Publish" << std::endl;
    }

}


void LinuxUdpClientFake::send_puback(uint8_t topic_id, uint16_t msg_id, return_code_test return_code) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_puback msg(topic_id, msg_id, return_code);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}

void LinuxUdpClientFake::send_pubrec(uint16_t msg_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_pubrec msg(msg_id);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}

void LinuxUdpClientFake::send_pubrel(uint16_t msg_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_pubrel msg(msg_id);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}

void LinuxUdpClientFake::send_pubcomp(uint16_t msg_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_pubcomp msg(msg_id);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Puback" << std::endl;
    }
}


void LinuxUdpClientFake::send_register(uint16_t topic_id, uint16_t msg_id, const char *topic_name) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_register msg(topic_id, msg_id, topic_name);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Register" << std::endl;
    }
}

void LinuxUdpClientFake::send_subscribe(bool dup, uint8_t qos, bool retain, bool will, bool clean_session,
                                        topic_id_type_test topic_id_type,
                                        uint16_t msg_id, const char *topic_name, uint16_t topic_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_subscribe msg(dup, qos, retain, will, clean_session, topic_id_type, msg_id, topic_name, topic_id);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Subscribe" << std::endl;
    }
}


void LinuxUdpClientFake::send_unsubscribe(topic_id_type_test topic_id_type, uint16_t msg_id, const char *topic_name,
                                          uint16_t topic_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_unsubscribe msg(topic_id_type, msg_id, topic_name, topic_id);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
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
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_disconnect msg;
    msg.length = length;

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Disconnect" << std::endl;
    }
}

void LinuxUdpClientFake::send_disconnect(uint16_t duration, uint8_t length) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_disconnect msg(duration);
    msg.length = length;

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Disconnect" << std::endl;
    }
}

void LinuxUdpClientFake::send_pingreq() {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_pingreq msg;

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Pingreq" << std::endl;
    }
}

void LinuxUdpClientFake::send_pingreq(const char *client_id) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_pingreq_wakeup msg(client_id);

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Pingreq" << std::endl;
    }
}

void LinuxUdpClientFake::/**/send_pingreq(const char *client_id, uint8_t length) {
    if (this->gw_address == nullptr) {
        throw new std::invalid_argument("gateway address not set");
    }
    if (fakeSocket->isDisconnected()) {
        fakeSocket->connect(this->gw_address);
    }

    test_pingreq_wakeup msg(client_id);
    msg.length = 2 + length;

    if (fakeSocket->send((const uint8_t *) &msg, msg.length) == -1) {
        std::cout << "sendto()Pingreq" << std::endl;
    }
}


void LinuxUdpClientFake::loop() {
    while (!stopped) {
        fakeSocket->loop();
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


void LinuxUdpClientFake::set_gw_address(device_address *address) {
    this->gw_address = address;
}


void LinuxUdpClientFake::setMqttSnReceiver(MqttSnReceiverInterface *receiverInterface) {
    this->receiver = receiverInterface;
}

FakeSocketInterface *LinuxUdpClientFake::getFakeSocket() {
    return this->fakeSocket;
}

