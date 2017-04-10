
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-cardinalities.h>
#include <gmock/gmock-actions.h>
#include <gmock/gmock-more-actions.h>
#include <gmock/gmock-generated-matchers.h>

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
        stop_broker();
    }

    virtual void SetUp() {
        start_broker();
    }

public:
    LinuxUdpGateway_Publish_Check() : Test() {

    }

    virtual ~LinuxUdpGateway_Publish_Check() {

    }

};



TEST_F(LinuxUdpGateway_Publish_Check, Two_CLients_Connect_Register_and_Publish_Disconnect_ComplexScenario) {

ASSERT_TRUE(false);

}