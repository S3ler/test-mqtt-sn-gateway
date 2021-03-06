#include <iostream>
#include "PahoMqttTestMessageHandler.h"

#define MQTT_LOG 1

void messageArrived1(MQTT::MessageData &md);

PahoMqttTestMessageHandler *global__receiver = nullptr;

void messageArrived1(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;
    char topic_name_buffer[1024];
    memset(topic_name_buffer, 0, 1024);
    if (md.topicName.lenstring.len < 1024) {
        memcpy(topic_name_buffer, md.topicName.lenstring.data, md.topicName.lenstring.len);
        global__receiver->receive_publish(topic_name_buffer, (uint8_t *) message.payload,
                                          (uint32_t) message.payloadlen);
    }
}


bool PahoMqttTestMessageHandler::begin() {
    if (this->receiver == nullptr) {
        return false;
    }
    ipstack = IPStack();
    client = new MQTT::Client<IPStack, Countdown>(ipstack);
    return true;
}

void PahoMqttTestMessageHandler::setServer(uint8_t *ip, uint16_t port) {
    this->hostname = nullptr;
    this->ip_address = 0;
    ip_address |= (*ip++) << 24;
    ip_address |= (*ip++) << 16;
    ip_address |= (*ip++) << 8;
    ip_address |= (*ip) << 0;
    this->port = port;
}


void PahoMqttTestMessageHandler::setServer(const char *hostname, uint16_t port) {
    this->ip_address = -1;
    this->hostname = hostname;
    this->port = port;
}


bool PahoMqttTestMessageHandler::connect(const char *id) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    } else if (ip_address != -1) {
        rc = ipstack.connect((uint32_t) ip_address, port);
    } else {
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *) id;
    rc = client->connect(data);

    return rc == 0;
}

bool PahoMqttTestMessageHandler::connect(const char *id, const char *user, const char *pass) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    } else if (ip_address != -1) {
        rc = ipstack.connect((uint32_t) ip_address, port);
    } else {
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.clientID.cstring = (char *) id;

    data.username.cstring = (char *) user;
    data.password.cstring = (char *) pass;
    rc = client->connect(data);

    return rc == 0;
}

bool PahoMqttTestMessageHandler::connect(const char *id, const char *willTopic, uint8_t willQos, bool willRetain,
                                         const uint8_t *willMessage, const uint16_t willMessageLength) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    } else if (ip_address != -1) {
        rc = ipstack.connect((uint32_t) ip_address, port);
    } else {
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.clientID.cstring = (char *) id;

    data.willFlag = 1;
    data.will.topicName.cstring = (char *) willTopic;
    data.will.message.cstring = (char *) willMessage;
    if (willQos == 0) {
        data.will.qos = MQTT::QOS0;
    } else if (willQos == 1) {
        data.will.qos = MQTT::QOS1;
    } else if (willQos == 2) {
        data.will.qos = MQTT::QOS2;
    } else {
        return false;
    }
    //data.will.retained = (unsigned char) willRetain; // TODO check
    rc = client->connect(data);

    return rc == 0;
}

bool PahoMqttTestMessageHandler::connect(const char *id, const char *user, const char *pass, const char *willTopic,
                                         uint8_t willQos, bool willRetain, const uint8_t *willMessage,
                                         const uint16_t willMessageLength) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    } else if (ip_address != -1) {
        rc = ipstack.connect((uint32_t) ip_address, port);
    } else {
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;

    data.clientID.cstring = (char *) id;

    data.username.cstring = (char *) user;
    data.password.cstring = (char *) pass;

    data.willFlag = 1;
    data.will.topicName.cstring = (char *) willTopic;
    data.will.message.cstring = (char *) willMessage;
    if (willQos == 0) {
        data.will.qos = MQTT::QOS0;
    } else if (willQos == 1) {
        data.will.qos = MQTT::QOS1;
    } else if (willQos == 2) {
        data.will.qos = MQTT::QOS2;
    } else {
        return false;
    }
    data.will.retained = (unsigned char) willRetain; // TODO check
    rc = client->connect(data);

    return rc == 0;
}

void PahoMqttTestMessageHandler::disconnect() {
    client->disconnect();
    ipstack.disconnect();
}

bool PahoMqttTestMessageHandler::publish(const char *topic, const uint8_t *payload, uint16_t plength, uint8_t qos,
                                         bool retained) {
    MQTT::Message message;
    if (qos == 0) {
        message.qos = MQTT::QOS0;
    } else if (qos == 1) {
        message.qos = MQTT::QOS1;
    } else if (qos == 2) {
        message.qos = MQTT::QOS2;
    } else {
        return false;
    }
    message.payload = (void *) payload;
    message.payloadlen = plength;

    message.retained = retained;
    message.dup = false;

    int rc = client->publish(topic, message);
    return rc == 0;
}

bool PahoMqttTestMessageHandler::subscribe(const char *topic, uint8_t qos) {
    MQTT::QoS _qos;
    if (qos == 0) {
        _qos = MQTT::QOS0;
    } else if (qos == 1) {
        _qos = MQTT::QOS1;
    } else if (qos == 2) {
        _qos = MQTT::QOS2;
    } else {
        return false;
    }

    int rc = client->subscribe(topic, _qos, messageArrived1);
    return rc == 0;
}


bool PahoMqttTestMessageHandler::unsubscribe(const char *topic) {
    if (topic == nullptr) {
        return true;
    }
    if (strlen(topic) == 0) {
        return true;
    }
    int rc = client->unsubscribe(topic);
    return rc == 0;
}

bool PahoMqttTestMessageHandler::receive_publish(char *topic, uint8_t *payload, uint32_t length) {
    MqttPublish publish((const char *) topic, (const uint8_t *) payload, (uint16_t)length);
    this->receiver->receive(publish);
    return false;
}

bool PahoMqttTestMessageHandler::start_loop() {
    this->thread = std::thread(&PahoMqttTestMessageHandler::loop, this);
    this->thread.detach();
}

void PahoMqttTestMessageHandler::loop() {
    while (!error && !stopped) {
        if (client->isConnected()) {
            client->yield(300);
        } else {
            error = true;
        }
    }
    if (client->isConnected()) {
        client->yield(300);
    }
}

bool PahoMqttTestMessageHandler::stop_loop() {
    this->stopped = true;
    return stopped;
}

void PahoMqttTestMessageHandler::setReceiver(MqttReceiverInterface *pMock) {
    this->receiver = pMock;
    global__receiver = this;
}

