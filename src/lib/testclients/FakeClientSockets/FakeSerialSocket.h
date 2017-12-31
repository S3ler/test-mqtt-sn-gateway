//
// Created by bele on 30.12.17.
//

#ifndef TEST_MQTT_SN_GATEWAY_FAKESERIALSOCKET_H
#define TEST_MQTT_SN_GATEWAY_FAKESERIALSOCKET_H


#include "FakeSocketInterface.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

class FakeSerialSocket : public FakeSocketInterface {
private:
    LinuxUdpClientFake *fakeClient;
    char *portname = "/dev/ttyUSB0";
    int fd = -1;
    std::mutex mutex;
    device_address address;
    device_address broadcast_address;

    device_address from_address;
    uint8_t data[1024];
    uint16_t data_length = 0;
public:
    void setFakeClient(LinuxUdpClientFake *fakeClient) override;

    bool isDisconnected() override;

    ssize_t send(const uint8_t *buf, uint8_t len) override;

    void connect(device_address *address) override;

    void disconnect() override;

    void loop() override;

    // FROM: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c Date: 30.12.207
    int set_interface_attribs(int fd, int speed, int parity);

    // FROM: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c Date: 30.12.207
    void set_blocking(int fd, int should_block);

    void waitForOKIdle();

    void waitFor(char *waitFor);

    void waitForOkSendAddress();

    void readSendAddress();

    void waitForOkSendData();

    void readData();

    void waitForOkAwaitAddress();

    void sendAddress(device_address address);

    void waitForOkAwaitData();

    void sendData(const uint8_t *buf, uint8_t len);

    void waitForOkSending();

    bool parseLong(const char *str, long *val);
};


#endif //TEST_MQTT_SN_GATEWAY_FAKESERIALSOCKET_H
