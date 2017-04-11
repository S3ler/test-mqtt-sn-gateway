
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-cardinalities.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-more-actions.h>
#include <gmock/gmock-generated-matchers.h>
#include <PahoMqttTestMessageHandler.h>
#include "../implementation/linux-mqtt-sn-gateway/src/Implementation/LinuxGateway.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Args;
using ::testing::SaveArg;
using ::testing::Invoke;
using ::testing::SetArgPointee;

void start_broker(){
    std::string command = "docker run -d --name test-broker -p 1884:1883 jllopis/mosquitto:v1.4.10";
    //std::string command = "docker run -d --name test-broker -p 1884:1883 -p 9883:9883 jllopis/mosquitto:v1.4.10";
    std::system(command.c_str());
}

void stop_broker(){
    std::string command = "docker rm -f test-broker";
    std::system(command.c_str());
}


class LinuxUdpGateway_Publish_Check : public ::testing::Test {

protected:
    virtual void TearDown() {
        //stop_broker();
    }

    virtual void SetUp() {
        //start_broker();
    }

public:
    LinuxUdpGateway_Publish_Check() : Test() {

    }

    virtual ~LinuxUdpGateway_Publish_Check() {

    }

};



TEST_F(LinuxUdpGateway_Publish_Check, QoS_M1_Publish) {
    LinuxGateway gateway;

    PahoMqttTestMessageHandler reveiving_client;
    reveiving_client.begin();


    uint8_t ip[] = {127, 0, 0, 1};
    uint16_t port = 1884;
    reveiving_client.setServer((uint8_t *) &ip, port);
    reveiving_client.connect("Test Client");

    reveiving_client.start_loop();

    reveiving_client.stop_loop();
    // TODO how let google test run something within a thread (gateway) and let is stop later, save it in member variable

    // connect gw
    // connect test client
    // set test client expected (maybe over a mock object?) then we can use the full power of google test
    // create a to send message

    // set mock object to expect the topic , qos and message

    // send message with qos -1

    // evaluate result

ASSERT_TRUE(false);

}