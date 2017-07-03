//
// Created by bele on 03.07.17.
//

#include <cstdint>
#include <cstddef>
#include <global_defines.h>
#include <iostream>
#include "FakeUdpSocket.h"

bool FakeUdpSocket::isConnected() {
    return s != -1;
}

ssize_t FakeUdpSocket::send(const uint8_t *buf, size_t len) {
    if(!isConnected())
    return (sendto(s, (const void *) &buf, len, 0, (struct sockaddr *) &si_other, slen) == -1);
}

void FakeUdpSocket::connect(device_address *address) {

    memcpy(&this->address, address, sizeof(device_address));

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    // si_other.sin_port = htons(PORT);

    uint32_t ip_address = 0;
    memcpy(&ip_address, &address->bytes, sizeof(ip_address));
    si_other.sin_addr.s_addr = ip_address;

    uint16_t port = 0;
    memcpy(&port, &address->bytes[sizeof(uint32_t)], sizeof(port));
    si_other.sin_port = port;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        std::cout << "socket" << std::endl;
        exit(1);
    }

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        std::cout << "socket" << std::endl;
        exit(1);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(fromport);
    sin.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *) &sin, sizeof(struct sockaddr_in)) == -1) {
        std::cout << "bind" << std::endl;
        exit(1);
    }

    // set timout
    struct timeval tv;
    tv.tv_sec = 0;  // 0 Secs Timeout
    tv.tv_usec = 200000;  // 200 ms Timeout


    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval)) == -1) {
        std::cout << "set timeout" << std::endl;
        exit(1);
    }

    // enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        std::cout << "broadcastEnable" << std::endl;
        exit(1);
    }

    // broadcast init
    static int port1 = 1234;

    static struct ip_mreq command;
    int loop = 1;
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port1);
    if ((socket_descriptor = socket(PF_INET,
                                    SOCK_DGRAM, 0)) == -1) {
        perror("socket()");
        exit(1);
        //exit(EXIT_FAILURE);
    }
    /* Mehr Prozessen erlauben, denselben Port zu nutzen */
    loop = 1;
    if (setsockopt(socket_descriptor,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &loop, sizeof(loop)) < 0) {
        perror("setsockopt:SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    if (bind(socket_descriptor,
             (struct sockaddr *) &sin,
             sizeof(sin)) < 0) {
        perror("bind");
        exit(1);


    }
    /* Broadcast auf dieser Maschine zulassen */
    loop = 1;
    if (setsockopt(socket_descriptor,
                   IPPROTO_IP,
                   IP_MULTICAST_LOOP,
                   &loop, sizeof(loop)) < 0) {
        perror("setsockopt:IP_MULTICAST_LOOP");
        exit(1);

    }


    /* Join the broadcast group: */
    command.imr_multiaddr.s_addr = inet_addr("224.0.0.0");
    command.imr_interface.s_addr = htonl(INADDR_ANY);
    if (command.imr_multiaddr.s_addr == -1) {
        perror("224.0.0.0 ist keine Multicast-Adresse\n");
        exit(1);
        //exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_descriptor,
                   IPPROTO_IP,
                   IP_ADD_MEMBERSHIP,
                   &command, sizeof(command)) < 0) {
        perror("setsockopt:IP_ADD_MEMBERSHIP");
        exit(1);

    }

    // set timout
    struct timeval udp_socket_timeval;
    udp_socket_timeval.tv_sec = 0;  // 0 Secs Timeout
    udp_socket_timeval.tv_usec = 200000;  // 200 ms Timeout


    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *) &udp_socket_timeval, sizeof(struct timeval)) ==
        -1) {
        exit(1);

    }

    // save bc address1 as value now
    struct sockaddr_in address1;
    memset(&address1, 0, sizeof(address1));
    address1.sin_family = AF_INET;
    address1.sin_addr.s_addr = inet_addr("224.0.0.0");
    address1.sin_port = htons(port1);

    device_address bc_address1 = this->getDevice_address(&address1);
    memset(&broadcast_address, 0, sizeof(device_address));
    memcpy(this->broadcast_address.bytes, bc_address1.bytes, sizeof(device_address));

}

void FakeUdpSocket::disconnect() {
    close(s);
    s = -1;
}

void FakeUdpSocket::loop() {
    while (!stopped) {
        int recv_len;
        memset(&buf, 0, BUFLEN);
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
            device_address client_address = getDevice_address(&si_other);
            fakeClient->receive((uint8_t *) buf, (uint16_t) recv_len);
        }
        memset(&buf, 0, BUFLEN);
        if ((recv_len = recvfrom(socket_descriptor, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
            device_address client_address = getDevice_address(&si_other);
            fakeClient->receive((uint8_t *) buf, (uint16_t) recv_len);
        }
    }
}

device_address FakeUdpSocket::getDevice_address(sockaddr_in *addr) {
    device_address address;
    memset(&address, 0, sizeof(address));
    {
        uint32_t ip_address = addr->sin_addr.s_addr;
        uint16_t port = addr->sin_port;

        memcpy(&address.bytes, &ip_address, sizeof(ip_address));
        if (port == 0) {
            port = PORT;
        }
        memcpy(&address.bytes[sizeof(ip_address)], &port, sizeof(port));

    }
    return address;
}

void FakeUdpSocket::setFakeClient(LinuxUdpClientFake *fakeClient) {
    this->fakeClient = fakeClient;
}
