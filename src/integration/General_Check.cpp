
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
using ::testing::DoAll;
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

ACTION_P(check_puback, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.topic_id, arg0->topic_id);
    ASSERT_EQ(expected.msg_id, arg0->msg_id);
    ASSERT_EQ(expected.return_code, arg0->return_code);
}

ACTION_P(check_gwinfo, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.gw_id, arg0->gw_id);

    ASSERT_THAT(std::vector<uint8_t>(arg0->gw_addr, arg0->gw_addr + (arg0->length - 3)),
                ::testing::ElementsAreArray(expected.gw_addr, expected.length - 3));
}

ACTION_P(check_advertise, expected) {
    ASSERT_EQ(expected.length, arg0->length);
    ASSERT_EQ(expected.type, arg0->type);
    ASSERT_EQ(expected.gw_id, arg0->gw_id);
    ASSERT_EQ(expected.duration, arg0->duration);
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
        /*
        std::string command = "docker rm -f test-broker 2> /dev/null  1> /dev/null";
        std::system(command.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
         */
    }


    void start_broker() {
        /*
        stop_broker();
#if defined(__arm__)
        std::string command = "docker run -d --name test-broker -p 1884:1883 fstehle/rpi-mosquitto 2> /dev/null 1> /dev/null";
#else
        std::string command = "docker run -d --name test-broker -p 1884:1883 jllopis/mosquitto:v1.4.10 2> /dev/null 1> /dev/null";
#endif
        std::system(command.c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
         */
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
        mqttsn_con << "advertise 120" << std::endl;
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
    LinuxUdpGateway_Publish_Check() : Test() {

    }

    virtual ~LinuxUdpGateway_Publish_Check() {

    }

public:
    void search_gateway() {
        SearchGW_maxRadius_returnsGWInfo();
    }

    void connect_with_will() {
        ConnectWithWillDuration60();
    }

    void publish_predefined() {
        PredefinedTopic_QoS_0_Publish();
        PredefinedTopic_QoS_1_Publish();
    }

    void register_topic() {

    }

    void publish_to_registered_topic() {

    }

    void timoeut_client() {
        Timout_WillAfter90received();
    }

    void await_gateway_adverstisment() {
        Receive_Advertise();
    }

    void publish_without_connection() {
        PredefinedTopic_QoS_M1_Publish();
    }

    void connect_without_will() {
        Connect_withoutwill_duration60();
    }

    void subscribe_and_receive_will() {

    }

    void sleep_client() {

    }

    void wake_up_client_and_receive_publishes() {

    }

    void connect_from_sleep() {

    }

    void disconnect() {

    }

    void ConnectWithWillDuration60() {

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

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, true);

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    void Connect_withoutwill_duration60() {

        test_connack expected_connack(TEST_ACCEPTED);

        EXPECT_CALL(mqtt_sn_receiver, receive_connack(_)).WillOnce(check_connack(expected_connack));
        mqtt_sn_sender.send_connect("Mqtt SN Testclient", 60, true, false);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << std::endl;
    }

    void Timout_WillAfter90received() {

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
        std::cout << std::flush;
    }

    void PredefinedTopic_QoS_M1_Publish() {
        // expected - incoming publish
        const char *topic = "/unsubscribed/client/topic/name";
        const char *data = "some qos m1 data";

        std::string data_string(data);
        std::vector<uint8_t> data_vector;
        std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
        data_vector.push_back('\0');

        EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                                 Field(&MqttPublish::topic, topic))));

        // when -  send publish with qos -1
        mqtt_sn_sender.send_publish(false, (int8_t) -1, false, false, (uint16_t) 50, 0, (const uint8_t *) data,
                                    (uint8_t) (strlen(data) + 1));

        // wait until all message are exchanged
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        std::cout << std::flush;
    }

    void PredefinedTopic_QoS_0_Publish() {

        // expected - incoming publish
        const char *topic = "/unsubscribed/client/topic/name";
        const char *data = "some qos 0 data";

        std::string data_string(data);
        std::vector<uint8_t> data_vector;
        std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
        data_vector.push_back('\0');

        EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                                 Field(&MqttPublish::topic, topic))));

        // when -  send publish with qos 0
        mqtt_sn_sender.send_publish(false, (int8_t) 0, false, false, (uint16_t) 50, 0, (const uint8_t *) data,
                                    (uint8_t) (strlen(data) + 1));

        // wait until all message are exchanged
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << std::flush;
    }

    void PredefinedTopic_QoS_1_Publish() {

        uint16_t msg_id = 10;
        uint16_t topic_id = 50;
        test_puback expected_puback(topic_id, msg_id, TEST_ACCEPTED);

        EXPECT_CALL(mqtt_sn_receiver, receive_puback(_)).WillOnce(check_puback(expected_puback));

        // expected - incoming publish
        const char *topic = "/unsubscribed/client/topic/name";
        const char *data = "some qos 0 data";

        std::string data_string(data);
        std::vector<uint8_t> data_vector;
        std::copy(data_string.begin(), data_string.end(), std::back_inserter(data_vector));
        data_vector.push_back('\0');

        EXPECT_CALL(mqtt_receiver, receive(AllOf(Field(&MqttPublish::data, data_vector),
                                                 Field(&MqttPublish::topic, topic))));

        // when -  send publish with qos 0
        mqtt_sn_sender.send_publish(false, (int8_t) 1, false, false, topic_id, msg_id, (const uint8_t *) data,
                                    (uint8_t) (strlen(data) + 1));

        // wait until all message are exchanged
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << std::flush;
    }

    void SearchGW_maxRadius_returnsGWInfo() {
#if (FakeClientTransmissionProtocol == FakeClientTransmissionProtocol_UDP)
        uint8_t expected_gw_addr[6] = {0, 0, 0, 0, 0, 0};
#else
        uint8_t expected_gw_addr[6] = {127, 0, 0, 1, 0, 0};
#endif

        uint8_t expected_gw_addr_len = 6;

        uint8_t expected_gw_info = 1;
        test_gwinfo expected_gwinfo(expected_gw_info, expected_gw_addr, expected_gw_addr_len);

        EXPECT_CALL(mqtt_sn_receiver, receive_gwinfo(_)).WillRepeatedly(check_gwinfo(expected_gwinfo));

        uint8_t radius = 255;
        mqtt_sn_sender.send_searchgw(radius);

        // wait until all message are exchanged
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void Receive_Advertise() {

        mqtt_sn_sender.getFakeSocket()->connect(&gw_address);

        uint8_t expected_gw_id = 1;
        uint8_t expected_advertise_duration = 120;
        test_advertise expected_advertise(expected_gw_id, expected_advertise_duration);

        EXPECT_CALL(mqtt_sn_receiver, receive_advertise(_)).Times(AtLeast(1)).WillRepeatedly(
                check_advertise(expected_advertise));

        // wait until all message are exchanged
        std::this_thread::sleep_for(std::chrono::milliseconds(30 * 1000));

        std::cout << std::flush;
    }

};



/**
 * High Level Description:
 * Search Gateway
 * Connect with Will
 * Publish Predefined
 * Register
 * Publish Registered
 * Timeout
 *
 * Gateway Advertisment
 * Publish without Connection
 * Connect without Will
 * Subscribe and receive Will
 *
 * Sleep
 * Publish
 * Register
 * Wake Up and Receive Publishes
 * Disconnect
 *
 * Connect from Sleep
 * Register
 * Publish
 * Disconnect
 */



/**
* Search Gateway
* Connect with Will
* Publish Predefined
* Register
* Publish Registered
* Timeout
*/
TEST_F(LinuxUdpGateway_Publish_Check, SearchGW_ConnectWithWill_Timeout) {
    search_gateway();
    connect_with_will();
    publish_predefined();
    //TODO
    //register_topic();
    //publish_to_registered_topic();
    timoeut_client();
    await_gateway_adverstisment();

    std::cout << std::endl;
}


TEST_F(LinuxUdpGateway_Publish_Check, Advertisment_ConnectWithoutWill_Sleep_Disconnect) {
    /**
    * Gateway Advertisment
    * Publish without Connection
    * Connect without Will
    * Subscribe and receive Will
    */
    publish_without_connection();
    connect_without_will();

    /**
    * Sleep
    * Publish
    * Register
    * Wake Up and Receive Publishes
    * Disconnect
    */
    //sleep_client();
    //wake_up_client_and_receive_publishes();

    /**
     * Connect from Sleep
     * Register
     * Publish
     * Disconnect
     */
    //connect_from_sleep();
    //publish_predefined();
    //publish_to_registered_topic();
    disconnect();
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Publish_Check, ConnectWithoutWill_Register_ReceivePublish) {

}
