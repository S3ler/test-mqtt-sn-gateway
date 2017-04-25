
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-cardinalities.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-more-actions.h>
#include <gmock/gmock-generated-matchers.h>
#include <vector>
#include <chrono>
#include <thread>
#include <PahoMqttTestMessageHandler.h>
#include <libgen.h>
#include <MqttReceiverMock.h>
#include <LinuxUdpClientFake.h>
#include <MockMqttSnReceiver.h>
#include "../implementation/linux-mqtt-sn-gateway/src/Implementation/LinuxGateway.h"
#include "../google_test_main_arguments.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Args;
using ::testing::SaveArg;
using ::testing::Invoke;
using ::testing::SetArgPointee;
using ::testing::Field;


/*
Modification in gtest main:
char **t_argv;
int t_argc;

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");
  testing::InitGoogleTest(&argc, argv);
  t_argc = argc;
  t_argv = argv;
  return RUN_ALL_TESTS();
}
 */

class LinuxUdpGateway_Publish_Check : public ::testing::Test {

protected:

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

        std::ofstream topics_pre(_rootPath + "/TOPICS.PRE");
        for (auto &&predefinedTopic : predefined_topics) {
            topics_pre << predefinedTopic << std::endl;
        }
        //topics_pre << "50 /unsubscribed/client/topic/name" << std::endl;
        //topics_pre << "20 /another/predefined/topic" << std::endl;
        topics_pre.close();
    }

    virtual void TearDown() {
        stopAllLoops();

        stop_broker();
        //std::system(("cd " + _rootPath + " && rm *.PRE && rm *.CON && rm *.REG && rm *.SUB && rm *.WIL && rm *.PUB && rm CLIENTS").c_str());
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
    LinuxUdpGateway_Publish_Check() : Test() {

    }

    virtual ~LinuxUdpGateway_Publish_Check() {

    }

};

ACTION_P(check_connack, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.return_code, arg0->return_code);
}

ACTION_P(check_puback, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.topic_id, arg0->topic_id);
    ASSERT_EQ(expected.msg_id, arg0->msg_id);
    ASSERT_EQ(expected.return_code, arg0->return_code);
}



TEST_F(LinuxUdpGateway_Publish_Check, PredefinedTopic_QoS_M1_Publish) {
    // expected - incoming publish
    const char *topic = "/unsubscribed/client/topic/name";
    const char *data = "some qos m1 data";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy( data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                        Field(&MqttPublish::topic, topic))));

    // when -  send publish with qos -1
    mqtt_sn_sender.send_publish(false, (int8_t) -1, false, false, (uint16_t) 50, 0, (const uint8_t *) data,
                            (uint8_t) (strlen(data) + 1));

    // wait until all message are exchanged
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Publish_Check, PredefinedTopic_QoS_0_Publish) {

    test_connack expected_connack(TEST_ACCEPTED);
    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));

    // expected - incoming publish
    const char *topic = "/unsubscribed/client/topic/name";
    const char *data = "some qos 0 data";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy( data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));


    mqtt_sn_sender.send_connect("Mqtt SN Testclient", UINT16_MAX, false, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // when -  send publish with qos 0
    mqtt_sn_sender.send_publish(false, (int8_t) 0, false, false, (uint16_t) 50, 0, (const uint8_t *) data,
                            (uint8_t) (strlen(data) + 1));

    // wait until all message are exchanged
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Publish_Check, PredefinedTopic_QoS_1_Publish) {

    uint16_t msg_id = 10;
    uint16_t topic_id = 50;
    test_puback expected_puback(topic_id, msg_id, TEST_ACCEPTED);
    test_connack expected_connack(TEST_ACCEPTED);

    EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
    EXPECT_CALL(mqtt_sn_receiver, receive_puback(_)).WillOnce(check_puback(expected_puback));

    // expected - incoming publish
    const char *topic = "/unsubscribed/client/topic/name";
    const char *data = "some qos 0 data";

    std::string data_string(data);
    std::vector<uint8_t> data_vector;
    std::copy( data_string.begin(), data_string.end(), std::back_inserter(data_vector));
    data_vector.push_back('\0');

    EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                             Field(&MqttPublish::topic, topic))));

    mqtt_sn_sender.send_connect("Mqtt SN Testclient", UINT16_MAX, false, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // when -  send publish with qos 0
    mqtt_sn_sender.send_publish(false, (int8_t) 1, false, false, topic_id, msg_id, (const uint8_t *) data,
                            (uint8_t) (strlen(data) + 1));

    // wait until all message are exchanged
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Publish_Check, PredefinedTopic_QoS_2_Publish) {

    // TODO implement me
    ASSERT_TRUE(false);
    std::cout << std::endl;
}
