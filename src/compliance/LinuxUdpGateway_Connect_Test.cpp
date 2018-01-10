//
// Created by bele on 14.04.17.
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

class LinuxUdpGateway_Connect_Test : public ::testing::Test {

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


    void stop_broker() {
        std::string command = "docker rm -f test-broker 2> /dev/null  1> /dev/null";
        std::system(command.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }


    void start_broker() {
        stop_broker();
#if defined(__arm__)
        std::string command = "docker run -d --name test-broker -p 1884:1883 fstehle/rpi-mosquitto 2> /dev/null 1> /dev/null";
#else
        std::string command = "docker run -d --name test-broker -p 1884:1883 jllopis/mosquitto:v1.4.10 2> /dev/null 1> /dev/null";
#endif
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
        mqttsn_con << "gatewayid 1" << std::endl;
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
    LinuxUdpGateway_Connect_Test() : Test() {

    }

    virtual ~LinuxUdpGateway_Connect_Test() {

    }

};

ACTION_P(check_disconnect, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
}

ACTION_P(check_connack, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.return_code, arg0->return_code);
}

ACTION_P(check_willtopicreq, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
}

ACTION_P(check_willmsg_req, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withoutwill_duration60_nocleansession_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withminimumclientidsize_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
    mqtt_sn_sender.send_connect("M", 60, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withmaximumclientidsize_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
    mqtt_sn_sender.send_connect("MqttSNTestclient23chars", 60, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withemptyclientid_return_disconnect) {

    test_disconnect expected_disconnect;

    EXPECT_CALL(mqtt_sn_receiver, receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));
    mqtt_sn_sender.send_connect("", 60, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withtoolongclientid_return_disconnect) {

    test_disconnect expected_disconnect;

    EXPECT_CALL(mqtt_sn_receiver, receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));
    mqtt_sn_sender.send_connect("MqttSNTestClientLongerThan23Characters", 60, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withoutwill_durationMin_nocleansession_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 0, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withoutwill_durationMax_nocleansession_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
    mqtt_sn_sender.send_connect("Mqtt SN Testclient", UINT16_MAX, false, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withoutwill_duration60_cleansession_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, false);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

ACTION_P2(InvokeUnrelatedFunction, classPointer, pointerToMemberFunc) {
    (classPointer->*pointerToMemberFunc)();
}

ACTION_P3(InvokeUnrelatedFunction, classPointer, pointerToMemberFunc, first) {
    (classPointer->*pointerToMemberFunc)(first);
}

ACTION_P4(InvokeUnrelatedFunction, classPointer, pointerToMemberFunc, first, second) {
    (classPointer->*pointerToMemberFunc)(first, second);
}

ACTION_P5(InvokeUnrelatedFunction, classPointer, pointerToMemberFunc, first, second, third) {
    (classPointer->*pointerToMemberFunc)(first, second, third);
}

ACTION_P6(InvokeUnrelatedFunction, classPointer, pointerToMemberFunc, first, second, third, fourth) {
    (classPointer->*pointerToMemberFunc)(first, second, third, fourth);
}


TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);
    test_willtopicreq expected_willtopicreq;
    test_willmsgreq expected_willmsgreq;

    const char *willtopic = "/some/will/topic";
    uint8_t qos = 0;
    bool retain = false;
    const char *willmsg = "some will message";

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    EXPECT_CALL(mqtt_sn_receiver, receive_willmsgreq(_))
            .WillOnce(DoAll(check_willmsg_req(expected_willmsgreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willmsg,
                                                    (const uint8_t *) willmsg, strlen(willmsg) + 1)));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_minimumWillTopicLength_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);
    test_willtopicreq expected_willtopicreq;
    test_willmsgreq expected_willmsgreq;

    const char *willtopic = "M";
    uint8_t qos = 0;
    bool retain = false;
    const char *willmsg = "some will message";

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    EXPECT_CALL(mqtt_sn_receiver, receive_willmsgreq(_))
            .WillOnce(DoAll(check_willmsg_req(expected_willmsgreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willmsg,
                                                    (const uint8_t *) willmsg, strlen(willmsg) + 1)));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_maximumWillTopicLength_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);
    test_willtopicreq expected_willtopicreq;
    test_willmsgreq expected_willmsgreq;

    const char *willtopic = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea taki";
    uint8_t qos = 0;
    bool retain = false;
    const char *willmsg = "some will message";

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    EXPECT_CALL(mqtt_sn_receiver, receive_willmsgreq(_))
            .WillOnce(DoAll(check_willmsg_req(expected_willmsgreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willmsg,
                                                    (const uint8_t *) willmsg, strlen(willmsg) + 1)));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_tooshortWillTopic_disconnect) {

    test_willtopicreq expected_willtopicreq;
    test_disconnect expected_disconnect;

    const char *willtopic = "";
    uint8_t qos = 0;
    bool retain = false;

    EXPECT_CALL(mqtt_sn_receiver, receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_tooLongWillTopic_disconnect) {

    test_willtopicreq expected_willtopicreq;
    test_disconnect expected_disconnect;

    const char *willtopic = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takim"; // 252 characters
    uint8_t qos = 0;
    bool retain = false;

    EXPECT_CALL(mqtt_sn_receiver, receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_emptyWillmessageLength_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);
    test_willtopicreq expected_willtopicreq;
    test_willmsgreq expected_willmsgreq;

    const char *willtopic = "/some/will/topic";
    uint8_t qos = 0;
    bool retain = false;
    const char *willmsg = "";

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    EXPECT_CALL(mqtt_sn_receiver, receive_willmsgreq(_))
            .WillOnce(DoAll(check_willmsg_req(expected_willmsgreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willmsg,
                                                    (const uint8_t *) willmsg, strlen(willmsg) + 1)));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_minimumWillmessageLength_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);
    test_willtopicreq expected_willtopicreq;
    test_willmsgreq expected_willmsgreq;

    const char *willtopic = "/some/will/topic";
    uint8_t qos = 0;
    bool retain = false;
    const char *willmsg = "M";

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    EXPECT_CALL(mqtt_sn_receiver, receive_willmsgreq(_))
            .WillOnce(DoAll(check_willmsg_req(expected_willmsgreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willmsg,
                                                    (const uint8_t *) willmsg, strlen(willmsg) + 1)));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Connect_Test, Connect_withwill_maximumWillmessageLength_return_accepted) {

    test_connack expected_connack(TEST_ACCEPTED);
    test_willtopicreq expected_willtopicreq;
    test_willmsgreq expected_willmsgreq;

    const char *willtopic("/some/will/topic");
    uint8_t qos = 0;
    bool retain = false;
    const char *willmsg = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takim"; // 253 = 252 characters + 1 (null-terminator)

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    EXPECT_CALL(mqtt_sn_receiver, receive_willmsgreq(_))
            .WillOnce(DoAll(check_willmsg_req(expected_willmsgreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willmsg,
                                                    (const uint8_t *) willmsg, strlen(willmsg) + 1)));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << std::endl;
}


TEST_F(LinuxUdpGateway_Connect_Test, DISABLED_Connect_withwill_tooLongWillmessageLength_disconnect) {
    // this test cannot be performed, the will message is no char string with null terminator so 255 and any message is a valid will message equal to a will message with the maximum will message size.

    test_disconnect expected_disconnect;
    test_willtopicreq expected_willtopicreq;
    test_willmsgreq expected_willmsgreq;

    const char *willtopic("/some/will/topic");
    uint8_t qos = 0;
    bool retain = false;
    const char *willmsg = "Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takima"; // 254 = 253 characters + 1 (null-terminator)

    EXPECT_CALL(mqtt_sn_receiver, receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    EXPECT_CALL(mqtt_sn_receiver, receive_willmsgreq(_))
            .WillOnce(DoAll(check_willmsg_req(expected_willmsgreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willmsg,
                                                    (const uint8_t *) willmsg, strlen(willmsg) + 1)));

    EXPECT_CALL(mqtt_sn_receiver, receive_willtopicreq(_))
            .WillOnce(DoAll(check_willtopicreq(expected_willtopicreq),
                            InvokeUnrelatedFunction(&mqtt_sn_sender, &LinuxUdpClientFake::send_willtopic,
                                                    willtopic, qos, retain)));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << std::endl;
}
