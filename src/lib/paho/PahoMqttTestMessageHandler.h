#ifndef GATEWAY_PAHOMQTTTESTMESSAGEHANDLER_H
#define GATEWAY_PAHOMQTTTESTMESSAGEHANDLER_H

#include <MQTTClient.h>
#include <linux.cpp>
#include <netinet/in.h>
#include <thread>
#include <MqttReceiverInterface.h>

class MqttReceiverInterface;



class PahoMqttTestMessageHandler {
private:

public:
     bool begin();

     void setServer(uint8_t *ip, uint16_t port);

     void setServer(const char* hostname, uint16_t port);

     bool connect(const char *id);

     bool connect(const char *id, const char *user, const char *pass);

     bool
    connect(const char *id, const char *willTopic, uint8_t willQos, bool willRetain, const uint8_t *willMessage,
            const uint16_t willMessageLength);

     bool
    connect(const char *id, const char *user, const char *pass, const char *willTopic, uint8_t willQos, bool willRetain,
            const uint8_t *willMessage, const uint16_t willMessageLength);

     void disconnect();

     bool publish(const char *topic, const uint8_t *payload, uint16_t plength, uint8_t qos, bool retained);

     bool subscribe(const char *topic, uint8_t qos);

     bool unsubscribe(const char *topic);

     bool receive_publish(char *topic, uint8_t *payload, uint32_t length);

     bool start_loop();

    void loop();

    bool stop_loop();

    IPStack ipstack;
    MQTT::Client<IPStack, Countdown, 512, 5> *client;
    const char *hostname = nullptr;
    uint16_t port = 0;

    void setReceiver(MqttReceiverInterface *pMock);

private:
    int64_t ip_address = -1;
    bool error = false;
    bool stopped= false;
    std::thread thread;
    MqttReceiverInterface *receiver = nullptr;
};


#endif //GATEWAY_PAHOMQTTTESTMESSAGEHANDLER_H
