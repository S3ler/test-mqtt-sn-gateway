//
// Created by bele on 30.12.17.
//

#include <mutex>
#include <climits>
#include "FakeSerialSocket.h"

void FakeSerialSocket::setFakeClient(LinuxUdpClientFake *fakeClient) {
    this->fakeClient = fakeClient;
}

bool FakeSerialSocket::isDisconnected() {
    return fd == -1;;
}

ssize_t FakeSerialSocket::send(const uint8_t *buf, uint8_t len) {
    std::lock_guard<std::mutex> lock(mutex);
    write(fd, "SEND\n", 5);
    waitForOkAwaitAddress();
    sendAddress(address);
    waitForOkAwaitData();
    sendData(buf, len);
    waitForOkSending();
    waitForOKIdle();
    return len;
}

void FakeSerialSocket::connect(device_address *address) {
    memcpy(&this->address, address, sizeof(device_address));

    memset(&broadcast_address, 0x0, sizeof(device_address));
    broadcast_address.bytes[0] = UINT8_MAX;

    std::lock_guard<std::mutex> lock(mutex);

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        // error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
        return;
    }
    set_interface_attribs(fd, B9600, 0);  // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking(fd, true);                // set no blocking

    usleep(2000);               // wait until uC connects
    write(fd, "RESET\n", 7);    // reset uC
    usleep((7 + 25) * 100);     // sleep enough to transmit the 7 plus
    waitForOKIdle();
}

void FakeSerialSocket::disconnect() {
    std::lock_guard<std::mutex> lock(mutex);
    close(fd);
}

void FakeSerialSocket::loop() {

    if (fd < 0) {
        usleep(500);
        return;
    }

    if(fd > 0){
        std::lock_guard<std::mutex> lock(mutex);

        write(fd, "RECEIVE\n", 8);
        waitForOkSendAddress();
        readSendAddress();
        waitForOkSendData();
        readData();
        waitForOKIdle();
        if (memcmp(&from_address, &address, sizeof(device_address)) == 0) {
            fakeClient->receive(data, data_length);
        }
        if (memcmp(&broadcast_address, &address, sizeof(device_address)) == 0) {
            fakeClient->receive(data, data_length);
        }
    }

    usleep(500);
}

// FROM: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c Date: 30.12.207
int FakeSerialSocket::set_interface_attribs(int fd, int speed, int parity) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        //error_message ("error %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN] = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        //error_message ("error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

// FROM: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c Date: 30.12.207
void FakeSerialSocket::set_blocking(int fd, int should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        //error_message ("error %d from tggetattr", errno);
        return;
    }

    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        //error_message ("error %d setting term attributes", errno);
    }
}

void FakeSerialSocket::waitFor(char *waitFor) {
    char buffer[1024];
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // TODO remove while(true) loop
    while (true) {
        char c;
        ssize_t n = read(fd, &c, 1);
        buffer[buffer_read++] = c;
        if (c == '\n') {
            // line done
            if ((strlen(waitFor) == buffer_read) && (strcmp(buffer, waitFor) == 0)) {
                return;
            }
            buffer_read = 0;
            memset(buffer, 0x0, sizeof(buffer));
        }
    }
}

void FakeSerialSocket::waitForOKIdle() {
    char toCompare[] = "OK IDLE\n";
    waitFor(toCompare);
}

void FakeSerialSocket::waitForOkSendAddress() {
    char toCompare[] = "OK SEND_ADDRESS\n";
    waitFor(toCompare);
}

void FakeSerialSocket::waitForOkSendData() {
    char toCompare[] = "OK SEND_DATA\n";
    waitFor(toCompare);
}

void FakeSerialSocket::waitForOkAwaitAddress() {
    char toCompare[] = "OK AWAIT_ADDRESS\n";
    waitFor(toCompare);
}

void FakeSerialSocket::waitForOkAwaitData() {
    char toCompare[] = "OK AWAIT_DATA\n";
    waitFor(toCompare);
}

void FakeSerialSocket::waitForOkSending() {
    char toCompare[] = "OK SENDING\n";
    waitFor(toCompare);
}

void FakeSerialSocket::sendAddress(device_address address) {
    write(fd, "ADDRESS", 7);
    for (uint16_t i = 0; i < sizeof(device_address); i++) {
        write(fd, " ", 1);
        std::string s = std::to_string(address.bytes[i]);
        write(fd, s.c_str(), s.length());
    }
    write(fd, "\n", 1);
}

void FakeSerialSocket::sendData(const uint8_t *buf, uint8_t len) {
    write(fd, "DATA\n", 8);
    for (uint16_t i = 0; i < len; i++) {
        write(fd, " ", 1);
        std::string s = std::to_string(buf[i]);
        write(fd, s.c_str(), s.length());
    }
    write(fd, "\n", 1);
}

void FakeSerialSocket::readSendAddress() {
    char buffer[1024];
    char* buffer_p = buffer;
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // TODO remove while(true) loop
    while (true) {
        char c;
        ssize_t n = read(fd, &c, 1);
        buffer[buffer_read++] = c;
        if (c == '\n') {
            break;
        }
        // TODO overflow
    }

    char *token = strsep(&buffer_p, " ");
    if (token == NULL) {
        memset(&from_address, 0x0, sizeof(device_address));
        return;
    }
    if (memcmp(token, "DATA", strlen("ADDRESS")) != 0) {
        memset(&from_address, 0x0, sizeof(device_address));
        return;
    }

    memset(&from_address, 0x0, sizeof(device_address));
    uint16_t destination_address_length = 0;

    while ((token = strsep(&buffer_p, " ")) != NULL) {
        long int number = 0;
        if (!parseLong(token, &number)) {
            memset(&from_address, 0x0, sizeof(device_address));
            return;
        }

        if (number > UINT8_MAX || number < 0) {
            memset(&from_address, 0x0, sizeof(device_address));
            return;
        }
        if (destination_address_length + 1 > sizeof(device_address)) {
            memset(&from_address, 0x0, sizeof(device_address));
            return;
        }
        from_address.bytes[destination_address_length++] = (uint8_t) number;
    }
    if (destination_address_length != sizeof(device_address)) {
        memset(&from_address, 0x0, sizeof(device_address));
    }

}

void FakeSerialSocket::readData() {

    char buffer[1024];
    char* buffer_p  = buffer;
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // TODO remove while(true) loop
    while (true) {
        char c;
        ssize_t n = read(fd, &c, 1);
        buffer[buffer_read++] = c;
        if (c == '\n') {
            break;
        }
        // TODO overflow
    }

    char *token = strsep(&buffer_p, " ");
    if (token == NULL) {
        memset(&data, 0x0, sizeof(data));
        data_length = 0;
        return;
    }
    if (memcmp(token, "DATA", strlen("DATA")) != 0) {
        memset(&data, 0x0, sizeof(data));
        data_length = 0;
        return;
    }


    memset(&data, 0x0, sizeof(data));
    data_length = 0;

    while ((token = strsep(&buffer_p, " ")) != NULL) {
        long int number = 0;
        if (!parseLong(token, &number)) {
            memset(&data, 0x0, sizeof(data));
            data_length = 0;
            return;
        }

        if (number > UINT8_MAX || number < 0) {
            memset(&data, 0x0, sizeof(data));
            data_length = 0;
            return;
        }
        data[data_length++] = (uint8_t) number;
    }

}

bool FakeSerialSocket::parseLong(const char *str, long *val) {
    char *temp;
    bool rc = true;
    errno = 0;
    *val = strtol(str, &temp, 0);

    if (temp == str || (*temp != '\0' && *temp != '\n') ||
        ((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
        rc = false;

    return rc;
}
