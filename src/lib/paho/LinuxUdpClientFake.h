#ifndef MQTTSNGATEWAY_CLIENTFAKE_H
#define MQTTSNGATEWAY_CLIENTFAKE_H


#include <global_defines.h>
#include <mqttsn_messages.h>
#include<stdio.h>	//printf
#include<string.h> //memset
#include<arpa/inet.h>
// #include "mqttsn_messages.h"

#define BUFLEN 255
#define PORT 8888

class LinuxUdpClientFake {
public:
    //const int BUFLEN = 255;
    struct sockaddr_in si_other, sin;
    uint16_t fromport = 60000;
    int timout_seconds = 10;
    int timout_micro_s = 0;
    int s = -1, i;
    socklen_t slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    device_address address;

private:
    void connect(device_address *address);

    void disconnect();

public:

    void send_connect(device_address *target_address, const char *client_id, uint16_t duration, bool clean_session,
                      bool will, bool close_connection = true);

    bool receive_willtopicreq(device_address *target_address);

    void send_willtopic(device_address *target_address, const char *willtopic, uint8_t qos, bool retain);

    bool receive_willmsgreq(device_address *target_address);

    void send_willmsg(device_address *target_address, const char *willmsg);

    bool receive_connack(device_address *target_address, return_code_t return_code, bool close_connection = true);

    void send_disconnect(device_address *target_address, bool close_connection = true);

    bool receive_disconnect(device_address *target_address, bool close_connection = true);

    void send_publish(device_address *target_address, const uint8_t *data, uint8_t data_length, uint16_t topic_id,
                      int8_t qos, bool short_topic, bool close_connection = true);

    void send_register(device_address *target_address, uint16_t msg_id, const char *topic_name,
                       bool close_connection);

    bool receive_regack(device_address *target_address, uint16_t msg_id, uint16_t *new_topic_id,
                        return_code_t return_code, bool close_connection);

    bool receive_puback(device_address *target_address, uint16_t topic_id, return_code_t return_code,
                        bool close_connection);

    void
    send_subscribe(device_address *target_address, bool short_topic, uint16_t topic_id, uint16_t msg_id, uint8_t qos);

    /**
     * Receive wenn short_topic == true
     */
    bool receive_suback(device_address *target_address, uint16_t topic_id, uint16_t msg_id,uint8_t* granted_qos, return_code_t return_code);

    void send_subscribe(device_address *target_address, char *topic_name, uint16_t msg_id, uint8_t qos);

    /**
     * Receive wenn short_topic == false
     */
    bool
    receive_suback(device_address *target_address, uint16_t *new_topic_id, uint16_t msg_id,uint8_t* granted_qos, return_code_t return_code);

    /*
      void send_searchgw(device_address *address, uint8_t radius);

      void send_unsubscribe(device_address *address, uint16_t topic_id, uint16_t msg_id, bool short_topic, bool dup);

      void send_pingreq(device_address *address, uint8_t *bytes);

      void send_disconnect(device_address *address, uint16_t duration);
      */

    device_address getDevice_address(sockaddr_in *addr) const;

    bool receive_any_publish(device_address *target_address, uint8_t *data, uint8_t *data_length,
                                 uint16_t *topic_id, uint8_t *qos);
};


#endif //MQTTSNGATEWAY_CLIENTFAKE_H
