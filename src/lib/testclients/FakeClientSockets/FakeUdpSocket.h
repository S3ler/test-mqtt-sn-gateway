//
// Created by bele on 03.07.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_FAKEUDPSOCKET_H
#define TEST_MQTT_SN_GATEWAY_FAKEUDPSOCKET_H


#include <monetary.h>
#include <cstdint>
#include <global_defines.h>
#include "FakeSocketInterface.h"
#include <global_defines.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "MqttSnReceiverInterface.h"
#include <atomic>
#include <unistd.h>

class LinuxUdpClientFake;

#define BUFLEN 255
#define PORT 8888

class FakeUdpSocket : public FakeSocketInterface {
private:
    //const int BUFLEN = 255;
    struct sockaddr_in si_other, sin;
    uint16_t fromport = 60000;
    int timout_seconds = 2;
    int timout_micro_s = 0;
    int s = -1, i;
    socklen_t slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    int socket_descriptor;

    device_address address;

    device_address broadcast_address;

    LinuxUdpClientFake *fakeClient;

    device_address *gw_address = nullptr;

public:

    virtual void setFakeClient(LinuxUdpClientFake *fake);

    virtual bool isDisconnected();

    virtual ssize_t send(const uint8_t *buf, uint8_t len);

    virtual void connect(device_address *address);

    virtual void disconnect();

    virtual void loop();

private:
    device_address getDevice_address(sockaddr_in *addr);

};


#endif //TEST_MQTT_SN_GATEWAY_FAKEUDPSOCKET_H
