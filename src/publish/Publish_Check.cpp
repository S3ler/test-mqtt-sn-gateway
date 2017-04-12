
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-cardinalities.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-more-actions.h>
#include <gmock/gmock-generated-matchers.h>
#include <chrono>
#include <thread>
#include <PahoMqttTestMessageHandler.h>
#include <gtest_main.cc>
#include <libgen.h>
#include <MqttReceiverMock.h>
#include <LinuxUdpClientFake.h>
#include <MockMqttSnReceiver.h>
#include "../implementation/linux-mqtt-sn-gateway/src/Implementation/LinuxGateway.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Args;
using ::testing::SaveArg;
using ::testing::Invoke;
using ::testing::SetArgPointee;
using ::testing::Field;

void stop_broker() {
    std::string command = "docker rm -f test-broker";
    std::system(command.c_str());
}


void start_broker() {
    stop_broker();
    std::string command = "docker run -d --name test-broker -p 1884:1883 jllopis/mosquitto:v1.4.10";
    //std::string command = "docker run -d --name test-broker -p 1884:1883 -p 9883:9883 jllopis/mosquitto:v1.4.10";
    std::system(command.c_str());
}


/*
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
std::string _rootPath;

class LinuxUdpGateway_Publish_Check : public ::testing::Test {

protected:
    virtual void TearDown() {
        stop_broker();
        //std::system(("cd " + _rootPath + " && rm *.PRE && rm *.CON && rm *.REG && rm *.SUB && rm *.WIL && rm *.PUB && rm CLIENTS").c_str());
        std::remove((_rootPath + "/00000000.SUB").c_str());
        std::remove((_rootPath + "/MQTT.SUB").c_str());
        std::remove((_rootPath + "/CLIENTS").c_str());

        std::remove((_rootPath + "/MQTT.CON").c_str());
        std::remove((_rootPath + "/TOPICS.PRE").c_str());
    }

    virtual void SetUp() {
        start_broker();
        _rootPath = std::string(dirname(t_argv[0]));
        create_configuration_files();
    }

    void create_configuration_files() const {

        std::ofstream mqtt_con(_rootPath + "/MQTT.CON");
        mqtt_con << "brokeraddress 127.0.0.1" << std::endl;
        mqtt_con << "brokerport 1884" << std::endl;
        mqtt_con << "clientid mqtt-sn gateway 0x01" << std::endl;
        mqtt_con << "gatewayid 1" << std::endl;
        mqtt_con.close();

        std::ofstream topics_pre(_rootPath + "/TOPICS.PRE");
        topics_pre << "50 /unsubscribed/client/topic/name" << std::endl;
        topics_pre << "20 /another/predefined/topic" << std::endl;
        topics_pre.close();
    }

public:
    LinuxUdpGateway_Publish_Check() : Test() {

    }

    virtual ~LinuxUdpGateway_Publish_Check() {

    }

};


TEST_F(LinuxUdpGateway_Publish_Check, QoS_M1_Publish) {

    std::string topic = "/unsubscribed/client/topic/name";
    std::string data = "some qos m1 data";

    // theoratically this is always the setup:
    // given
    LinuxGateway gateway;
    // std::string _rootPath(dirname(t_argv[0])); give false rootpath
    gateway.setRootPath((char *) _rootPath.c_str());
    ASSERT_TRUE(gateway.begin());
    gateway.start_loop();


    MqttReceiverMock receiver;
    EXPECT_CALL(receiver, receive(AllOf(Field(&MqttPublish::data, data), Field(&MqttPublish::topic, topic))));

    PahoMqttTestMessageHandler reveiving_client;
    reveiving_client.setReceiver(&receiver);
    ASSERT_TRUE(reveiving_client.begin());

    uint8_t ip[] = {127, 0, 0, 1};
    uint16_t port = 1884;
    reveiving_client.setServer((uint8_t *) &ip, port);
    reveiving_client.connect("Test Client");
    reveiving_client.start_loop();
    reveiving_client.subscribe(topic.c_str(), 0);
    // setup done

    // send message with qos -1
    device_address gw_address;
    gw_address.bytes[0] = 127;
    gw_address.bytes[1] = 0;
    gw_address.bytes[2] = 0;
    gw_address.bytes[3] = 1;

    uint16_t ntohs_port = ntohs(8888);
    memcpy(&gw_address.bytes[4], &ntohs_port, sizeof(uint16_t));

    LinuxUdpClientFake clientFake;
    MqttSnReceiverMock receiverMock;
    clientFake.setMqttSnReceiver(&receiverMock);
    clientFake.start_loop();
    clientFake.send_publish(&gw_address, (const uint8_t *) data.c_str(),
                            (uint8_t) data.length(), (uint16_t) 50, (int8_t) -1, false, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // theoratically this is always the teardown:
    clientFake.stop_loop();
    reveiving_client.stop_loop();
    gateway.stop_loop();

    std::cout << std::endl;
    ASSERT_TRUE(true);
}

//TODO for further publish test, we need to check connect functionality