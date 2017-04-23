//
// Created by bele on 20.04.17.
//

#include <gtest/gtest.h>
#include <LinuxGateway.h>
#include <MqttReceiverMock.h>
#include <LinuxUdpClientFake.h>
#include <MockMqttSnReceiver.h>
#include <libgen.h>
#include "../google_test_main_arguments.h"
#include <gmock/gmock-more-actions.h>


using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Args;
using ::testing::SaveArg;
using ::testing::Invoke;
using ::testing::SetArgPointee;
using ::testing::Field;
using ::testing::DoAll;

class LinuxUdpGateway_SearchGw_Check : public ::testing::Test {

public:
    LinuxGateway gateway;
    MqttReceiverMock mqtt_receiver;
    LinuxUdpClientFake mqtt_sn_sender;
    MqttSnReceiverMock mqtt_sn_receiver;
    PahoMqttTestMessageHandler mqtt_client;
    std::string _rootPath;


    device_address gw_address;
    uint8_t ip[4] = {127, 0, 0, 1};
    uint16_t port = 1884;

    std::list<std::string> predefined_topics;
    std::list<std::string> receiver_topics;

    uint8_t expected_gw_addr[6] = {192,168,178,33,184,0};
    uint8_t expected_gw_addr_len = 6;

    void stop_broker() {
        std::string command = "docker rm -f test-broker 2> /dev/null  1> /dev/null";
        std::system(command.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }


    void start_broker() {
        stop_broker();
        std::string command = "docker run -d --name test-broker -p 1884:1883 jllopis/mosquitto:v1.4.10 2> /dev/null 1> /dev/null";
        std::system(command.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void create_configuration_files() const {

        std::ofstream mqtt_con(_rootPath + "/MQTT.CON");
        mqtt_con << "brokeraddress 127.0.0.1" << std::endl;
        mqtt_con << "brokerport 1884" << std::endl;
        mqtt_con << "clientid mqtt-sn gateway 0x01" << std::endl;
        mqtt_con.close();


        std::ofstream mqttsn_con(_rootPath + "/MQTTSN.CON");
        mqttsn_con << "gatewayid 5" << std::endl;
        mqttsn_con << "timeout 10" << std::endl;
        mqttsn_con << "advertise 900" << std::endl;
        mqttsn_con.close();

        std::ofstream topics_pre(_rootPath + "/TOPICS.PRE");
        for (auto &&predefinedTopic : predefined_topics) {
            topics_pre << predefinedTopic << std::endl;
        }

        topics_pre.close();
    }

    virtual void TearDown() {
        stopAllLoops();

        stop_broker();

        std::remove((_rootPath + "/00000000.PUB").c_str());
        std::remove((_rootPath + "/00000000.REG").c_str());
        std::remove((_rootPath + "/00000000.SUB").c_str());
        std::remove((_rootPath + "/00000000.WIL").c_str());

        std::remove((_rootPath + "/MQTT.SUB").c_str());
        std::remove((_rootPath + "/CLIENTS").c_str());

        std::remove((_rootPath + "/MQTT.CON").c_str());
        std::remove((_rootPath + "/MQTTSN.CON").c_str());
        std::remove((_rootPath + "/TOPICS.PRE").c_str());

    }


    virtual void SetUp() {
        start_broker();
        std::string s(t_argv[0]);
        _rootPath = std::string(dirname((char *) s.c_str()));

        predefined_topics.push_back(std::string("50 /unsubscribed/client/topic/name"));
        predefined_topics.push_back(std::string("20 /another/predefined/topic"));
        receiver_topics.push_back(std::string("/unsubscribed/client/topic/name"));

        create_configuration_files();

        gateway.setRootPath((char *) _rootPath.c_str());
        mqtt_client.setReceiver(&mqtt_receiver);
        mqtt_sn_sender.setMqttSnReceiver(&mqtt_sn_receiver);

        gw_address.bytes[0] = 127;
        gw_address.bytes[1] = 0;
        gw_address.bytes[2] = 0;
        gw_address.bytes[3] = 1;

        uint16_t ntohs_port = ntohs(8888);
        memcpy(&gw_address.bytes[4], &ntohs_port, sizeof(uint16_t));

        ASSERT_TRUE(gateway.begin());
        gateway.start_loop();

        ASSERT_TRUE(mqtt_client.begin());
        mqtt_sn_sender.start_loop();

        mqtt_client.setServer((uint8_t *) &ip, port);
        ASSERT_TRUE(mqtt_client.connect("Test Client"));
        for (auto &&receiver_topic : receiver_topics) {
            ASSERT_TRUE(mqtt_client.subscribe(receiver_topic.c_str(), 0));
        }
        mqtt_client.start_loop();

        mqtt_sn_sender.set_gw_address(&gw_address);
    }

    void stopAllLoops() {
        mqtt_sn_sender.stop_loop();
        mqtt_client.stop_loop();
        gateway.stop_loop();
    }


public:
    LinuxUdpGateway_SearchGw_Check() : Test() {

    }

    virtual ~LinuxUdpGateway_SearchGw_Check() {

    }
};

ACTION_P(check_gwinfo, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.gw_id, arg0->gw_id);

    ASSERT_THAT(std::vector<uint8_t>(arg0->gw_addr,arg0->gw_addr+(arg0->length -3)),
                ::testing::ElementsAreArray(expected.gw_addr, expected.length-3));
}

TEST_F(LinuxUdpGateway_SearchGw_Check, SearchGW_returns_GWInfo) {

    uint8_t expected_gw_info = 5;
    test_gwinfo expected_gwinfo(expected_gw_info, expected_gw_addr, expected_gw_addr_len);

    EXPECT_CALL(mqtt_sn_receiver, receive_gwinfo(_)).WillRepeatedly(check_gwinfo(expected_gwinfo));

    uint8_t radius = 5;
    mqtt_sn_sender.send_searchgw(radius);

    // wait until all message are exchanged
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_SearchGw_Check, SearchGW_minRadius_returnsGWInfo) {

    uint8_t expected_gw_info = 5;
    test_gwinfo expected_gwinfo(expected_gw_info, expected_gw_addr, expected_gw_addr_len);

    EXPECT_CALL(mqtt_sn_receiver, receive_gwinfo(_)).WillRepeatedly(check_gwinfo(expected_gwinfo));

    uint8_t radius = 0;
    mqtt_sn_sender.send_searchgw(radius);

    // wait until all message are exchanged
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_SearchGw_Check, SearchGW_maxRadius_returnsGWInfo) {

    uint8_t expected_gw_info = 5;
    test_gwinfo expected_gwinfo(expected_gw_info, expected_gw_addr, expected_gw_addr_len);

    EXPECT_CALL(mqtt_sn_receiver, receive_gwinfo(_)).WillRepeatedly(check_gwinfo(expected_gwinfo));

    uint8_t radius = 255;
    mqtt_sn_sender.send_searchgw(radius);

    // wait until all message are exchanged
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << std::endl;
}