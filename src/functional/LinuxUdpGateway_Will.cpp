//
// Created by bele on 24.04.17.
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

class LinuxUdpGateway_Will : public ::testing::Test {
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
        mqttsn_con << "timeout 1" << std::endl;
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
        receiver_topics.push_back(std::string("/some/will/topic"));

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
    LinuxUdpGateway_Will() : Test() {

    }

    virtual ~LinuxUdpGateway_Will() {

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


TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration60_timeout_WillAfter90received) {

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

    // expected - incoming publish
    const char *topic = "/some/will/topic";
    const char *data = "some will message";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    std::this_thread::sleep_for(std::chrono::milliseconds(90 * 1000));
    std::cout << std::endl;

}

TEST_F(LinuxUdpGateway_Will, ConnectWithoutWillDuration60_timeout_noWillAfter120Received) {

    test_connack expected_connack(TEST_ACCEPTED);
    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, false, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    EXPECT_CALL(mqtt_receiver, receive(_)).Times(0).WillOnce(Return(true));

    std::this_thread::sleep_for(std::chrono::milliseconds(120 * 1000));
    std::cout << std::endl;

}

TEST_F(LinuxUdpGateway_Will,
       ConnectWithtWillDisconnectConnectCleanSessionWithoutWill_timeout_noWillAfter120Received) {
    test_connack expected_connack(TEST_ACCEPTED);
    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    EXPECT_CALL(mqtt_receiver, receive(_)).Times(0).WillOnce(Return(true));

    std::this_thread::sleep_for(std::chrono::milliseconds(120 * 1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration0_noTimeout_noWillAfter120Received) {

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

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", 0, true, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    EXPECT_CALL(mqtt_receiver, receive(_)).Times(0).WillOnce(Return(true));

    std::this_thread::sleep_for(std::chrono::milliseconds(120 * 1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration1_timeout_WillAfter2Received) {
    uint16_t duration = 1;
    uint16_t after_duration = 2;

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

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", duration, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // expected - incoming publish
    const char *topic = "/some/will/topic";
    const char *data = "some will message";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    std::this_thread::sleep_for(std::chrono::milliseconds(after_duration * 1000 + 1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration10_timeout_WillAfter15Received) {
    uint16_t duration = 10;
    uint16_t after_duration = 15;

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

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", duration, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // expected - incoming publish
    const char *topic = "/some/will/topic";
    const char *data = "some will message";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    std::this_thread::sleep_for(std::chrono::milliseconds(after_duration * 1000 + 1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration30_timeout_WillAfter45Received) {
    uint16_t duration = 30;
    uint16_t after_duration = 45;

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

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", duration, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // expected - incoming publish
    const char *topic = "/some/will/topic";
    const char *data = "some will message";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    std::this_thread::sleep_for(std::chrono::milliseconds(after_duration * 1000 + 1000));
    std::cout << std::endl;
}


TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration59_timeout_WillAfter89Received) {
    uint16_t duration = 59;
    uint16_t after_duration = 89;

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

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", duration, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // expected - incoming publish
    const char *topic = "/some/will/topic";
    const char *data = "some will message";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    std::this_thread::sleep_for(std::chrono::milliseconds(after_duration * 1000 + 1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration61_timeout_WillAfter67received) {
    uint16_t duration = 61;
    uint16_t after_duration = 67;

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

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", duration, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // expected - incoming publish
    const char *topic = "/some/will/topic";
    const char *data = "some will message";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    std::this_thread::sleep_for(std::chrono::milliseconds(after_duration * 1000 + 1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will, ConnectWithWillDuration90_timeout_WillAfter99received) {
    uint16_t duration = 90;
    uint16_t after_duration = 120;

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

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", duration, true, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // expected - incoming publish
    const char *topic = "/some/will/topic";
    const char *data = "some will message";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    std::this_thread::sleep_for(std::chrono::milliseconds(after_duration * 1000 + 1000));
    std::cout << std::endl;
}
//TODO what happens with empty will message?

// TODO multiple connect to gateway, with will, without will (combinations) test if will is retained or not etc.

// no clean session
// connect(60, no will) => disconnect => connect(60, will) => timeout => will received
// connect(60, will1) => disconnect => connect(60, will2) => timeout => will2 received
// connect(60, will) => disconnect => connect(60, no will) => timeout => no will received
// connect(60, will) => disconnect => connect(60, no will) => timeout => no will received

// connect(60, will) => disconnect => connect(60, no will) => timeout => no will received

// 6.3 Clean session -

// without prior connect
// CleanSession=true, Will=true => timeout => will received
// CleanSession=true, Will=false => timeout => no will received
// CleanSession=false, Will=true => timeout => will received
// CleanSession=false, Will=false => timeout => will received

// with prior connect
// CleanSession=true, Will=true => disconnect/reconnect => CleanSession=true, Will=true => second will received
// etc

// no will => will update => timeout => will received
// will => will update => timeout => will update received
// max / min / empty etc
// TODO implement will update functional tests
// TODO implement topic update => new topic received (not old)
// TODO implement message update => new message received (not old)
// TODO implement message update & topic update => new message & topic received (not old)

