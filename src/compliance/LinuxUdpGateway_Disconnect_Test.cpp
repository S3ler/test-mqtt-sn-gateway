



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


ACTION_P(check_connack, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.return_code, arg0->return_code);
}

ACTION_P(check_pubrel, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.msg_id, arg0->msg_id);
}

ACTION_P(check_regack, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.msg_id, arg0->msg_id);
    ASSERT_EQ(expected.topic_id, arg0->topic_id);
    ASSERT_EQ(expected.return_code, arg0->return_code);
}

ACTION_P(check_suback, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.flags, arg0->flags);
    ASSERT_EQ(expected.msg_id, arg0->msg_id);
    ASSERT_EQ(expected.topic_id, arg0->topic_id);
    ASSERT_EQ(expected.return_code, arg0->return_code);
}

ACTION_P(check_disconnect, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
}

ACTION_P(check_publish, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.flags, arg0->flags);
    ASSERT_EQ(expected.topic_id, arg0->topic_id);
    ASSERT_EQ(expected.message_id, arg0->message_id);
    ASSERT_THAT(std::vector<uint8_t>(arg0->data, arg0->data + (arg0->length - 7)),
                ::testing::ElementsAreArray(expected.data, expected.length - 7));
}

ACTION_P3(InvokeUnrelatedFunction, classPointer, pointerToMemberFunc, first) {
    (classPointer->*pointerToMemberFunc)(first);
}

ACTION_P5(InvokeUnrelatedFunction, classPointer, pointerToMemberFunc, first, second, third) {
    (classPointer->*pointerToMemberFunc)(first, second, third);
}

class LinuxUdpGateway_Disconnect : public ::testing::Test {
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
        mqttsn_con << "gatewayid 1" << std::endl;
        mqttsn_con << "timeout 10" << std::endl;
        mqttsn_con << "advertise 900" << std::endl;
        mqttsn_con.close();

    }

    void create_predefined_topic() const {
        std::ofstream topics_pre(_rootPath + "/TOPICS.PRE");
        for (auto &&predefinedTopic : predefined_topics) {
            topics_pre << predefinedTopic << std::endl;
        }

        topics_pre.close();
    }

    void delete_predefined_topic() const {
        std::remove((_rootPath + "/TOPICS.PRE").c_str());
    }

    virtual void TearDown() {
        stopAllLoops();

        stop_broker();
        delete_config_files();

        delete_predefined_topic();
    }


    void delete_config_files() {
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

        delete_config_files();


        predefined_topics.push_back(std::string("50 /unsubscribed/client/topic/name"));
        predefined_topics.push_back(std::string("20 /another/predefined/topic"));
        receiver_topics.push_back(std::string("/unsubscribed/client/topic/name"));
        // receiver_topics.push_back(std::string("/register/by/client/topic/name"));

        create_configuration_files();
        create_predefined_topic();

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


        connect_mqtt_sn_client();

        // wait until all message are exchanged
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void connect_mqtt_sn_client()  {

        test_connack expected_connack(TEST_ACCEPTED);
        EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
        mqtt_sn_sender.send_connect("Mqtt SN Testclient", UINT16_MAX, false, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        const char *topic = "/register/by/client/topic/name";

        uint16_t expected_msg_id = 5;
        uint16_t expected_topic_id = 1;
        test_regack expected_regack(expected_topic_id, expected_msg_id, TEST_ACCEPTED);
        EXPECT_CALL(mqtt_sn_receiver, receive_regack(_)).WillOnce(check_regack(expected_regack));

        uint16_t msg_id = 5;
        uint16_t topic_id = 0;
        mqtt_sn_sender.send_register(topic_id, msg_id, topic);

    }

    void stopAllLoops() {
        mqtt_sn_sender.stop_loop();
        mqtt_client.stop_loop();
        gateway.stop_loop();
    }

public:
    LinuxUdpGateway_Disconnect() : Test() {

    }

    virtual ~LinuxUdpGateway_Disconnect() {

    }

};


TEST_F(LinuxUdpGateway_Disconnect, Disconnect_withoutDuration_return_disconnect) {

    test_disconnect expected_disconnect;
    EXPECT_CALL(mqtt_sn_receiver,receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    mqtt_sn_sender.send_disconnect();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Disconnect, Disconnect_withoutDuration_tooLong_return_disconnect) {

    test_disconnect expected_disconnect;
    EXPECT_CALL(mqtt_sn_receiver,receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    mqtt_sn_sender.send_disconnect(3);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Disconnect, Disconnect_withDuration_tooShort_return_disconnect) {

    test_disconnect expected_disconnect;
    EXPECT_CALL(mqtt_sn_receiver,receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    mqtt_sn_sender.send_disconnect(600,3);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Disconnect, Disconnect_withDuration_return_disconnect) {

    test_disconnect expected_disconnect;
    EXPECT_CALL(mqtt_sn_receiver,receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    mqtt_sn_sender.send_disconnect(600,4);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}


TEST_F(LinuxUdpGateway_Disconnect, Disconnect_withDuration_tooLong_return_disconnect) {

    test_disconnect expected_disconnect;
    EXPECT_CALL(mqtt_sn_receiver,receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    mqtt_sn_sender.send_disconnect(600,5);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Disconnect, Disconnect_withDuration_min_return_disconnect) {

    test_disconnect expected_disconnect;
    EXPECT_CALL(mqtt_sn_receiver,receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    mqtt_sn_sender.send_disconnect(0,4);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Disconnect, Disconnect_withDuration_maxg_return_disconnect) {

    test_disconnect expected_disconnect;
    EXPECT_CALL(mqtt_sn_receiver,receive_disconnect(_)).WillOnce(check_disconnect(expected_disconnect));

    mqtt_sn_sender.send_disconnect(UINT16_MAX,4);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << std::endl;
}
