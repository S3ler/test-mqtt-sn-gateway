#ifndef MQTTSNGATEWAY_CLIENTFAKE_H
#define MQTTSNGATEWAY_CLIENTFAKE_H


#include <global_defines.h>
#include<stdio.h>
#include<string.h>
#include<arpa/inet.h>
#include "MqttSnReceiverInterface.h"
#include <atomic>

#define BUFLEN 255
#define PORT 8888

class LinuxUdpClientFake {
public:
    //const int BUFLEN = 255;
    struct sockaddr_in si_other, sin;
    uint16_t fromport = 60000;
    int timout_seconds = 2;
    int timout_micro_s = 0;
    int s = -1, i;
    socklen_t slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    device_address address;

    std::atomic<bool> stopped{false};

public:

    void send_searchgw(uint8_t radius);

    void send_connect(const char *client_id, uint16_t duration, bool clean_session, bool will);

    void send_willtopic(const char *willtopic, uint8_t qos, bool retain);

    void send_willmsg(const uint8_t *willmsg, uint8_t willmsg_len);

    void send_publish(bool dup, int8_t qos, bool retain, bool short_topic, uint16_t topic_id,
                      uint16_t msg_id, const uint8_t *data, uint8_t data_length);

    void send_puback(uint8_t topic_id, uint16_t msg_id, return_code_test return_code);

    void send_pubcomp(uint16_t msg_id);

    void send_pubrel(uint16_t msg_id);

    void send_pubrec(uint16_t msg_id);

    void send_register(uint16_t topic_id, uint16_t msg_id, const char *topic_name);

    void send_subscribe(bool dup, uint8_t qos, bool retain, bool will, bool clean_session,
                        topic_id_type_test topic_id_type,
                        uint16_t msg_id, const char *topic_name, uint16_t topic_id);

    void send_unsubscribe(topic_id_type_test topic_id_type, uint16_t msg_id, const char *topic_name, uint16_t topic_id);

private:
    void connect(device_address *address);

    void disconnect();

public:

    void loop();

    void start_loop();

    void stop_loop();

    void receive(uint8_t *data, uint16_t length);

    device_address getDevice_address(sockaddr_in *addr) const;

    void set_gw_address(device_address *address);

    void setMqttSnReceiver(MqttSnReceiverInterface *receiverInterface);


    MqttSnReceiverInterface *receiver = nullptr;
    std::thread thread;
    device_address *gw_address = nullptr;




};


#endif //MQTTSNGATEWAY_CLIENTFAKE_H
