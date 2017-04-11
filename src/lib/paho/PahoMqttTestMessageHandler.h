#ifndef GATEWAY_PAHOMQTTTESTMESSAGEHANDLER_H
#define GATEWAY_PAHOMQTTTESTMESSAGEHANDLER_H

#include <MQTTClient.h>
#include <linux.cpp>
#include <netinet/in.h>

class PahoMqttTestMessageHandler {
private:

public:
    virtual bool begin();

    virtual void setServer(uint8_t *ip, uint16_t port);

    virtual void setServer(const char* hostname, uint16_t port);

    virtual bool connect(const char *id);

    virtual bool connect(const char *id, const char *user, const char *pass);

    virtual bool
    connect(const char *id, const char *willTopic, uint8_t willQos, bool willRetain, const uint8_t *willMessage,
            const uint16_t willMessageLength);

    virtual bool
    connect(const char *id, const char *user, const char *pass, const char *willTopic, uint8_t willQos, bool willRetain,
            const uint8_t *willMessage, const uint16_t willMessageLength);

    virtual void disconnect();

    virtual bool publish(const char *topic, const uint8_t *payload, uint16_t plength, uint8_t qos, bool retained);

    virtual bool subscribe(const char *topic, uint8_t qos);

    virtual bool unsubscribe(const char *topic);

    virtual bool receive_publish(char *topic, uint8_t *payload, uint32_t length);

    virtual bool loop();

    IPStack ipstack;
    MQTT::Client<IPStack, Countdown, 512, 5> *client;
    const char *hostname = nullptr;
    uint16_t port = 0;
private:
    int64_t ip_address = -1;
};


#endif //GATEWAY_PAHOMQTTTESTMESSAGEHANDLER_H
