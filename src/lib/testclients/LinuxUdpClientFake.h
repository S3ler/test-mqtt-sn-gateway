#ifndef MQTTSNGATEWAY_CLIENTFAKE_H
#define MQTTSNGATEWAY_CLIENTFAKE_H


#include <global_defines.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "MqttSnReceiverInterface.h"
#include <atomic>
#include <thread>
#include <FakeClientSockets/FakeSocketInterface.h>
#include <FakeClientSockets/FakeUdpSocket.h>


#define UDP         1
#define TCP         2
#define BLE         3
#define ZIGBEE      4



class FakeUdpSocket;
class FakeSocketInterface;

class LinuxUdpClientFake {
public:
    std::atomic<bool> stopped{false};

    FakeSocketInterface* fakeSocket = nullptr;
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

    void send_disconnect();

    void send_disconnect(uint8_t length);

    void send_disconnect(uint16_t duration, uint8_t length);

    void send_pingreq();

    void send_pingreq(const char *client_id);

    void send_pingreq(const char *client_id, uint8_t length);


public:

    void loop();

    void start_loop();

    void stop_loop();

    void receive(uint8_t *data, uint16_t length);

    void set_gw_address(device_address *address);

    void setMqttSnReceiver(MqttSnReceiverInterface *receiverInterface);

    FakeSocketInterface* getFakeSocket();
private:

    MqttSnReceiverInterface *receiver = nullptr;
    std::thread thread;
    device_address *gw_address = nullptr;


};


#endif //MQTTSNGATEWAY_CLIENTFAKE_H
