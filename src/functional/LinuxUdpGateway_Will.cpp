//
// Created by bele on 24.04.17.
//

#include <gtest/gtest.h>

class LinuxUdpGateway_Will : public ::testing::Test {
public:
    virtual void TearDown() {

    }

    virtual void SetUp() {

    }

};

TEST_F(LinuxUdpGateway_Will,ConnectWithWillDuration60_timeout_WillAfter90received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,DISABLED_ConnectWithoutWillDuration60_timeout_noWillAfter120Received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,DISABLED_ConnectWithtWillDisconnectConnectCleanSessionWithoutWill_timeout_noWillAfter120Received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,DISABLED_ConnectWithWillDuration0_noTimeout_noWillAfter120Received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,ConnectWithWillDuration1_timeout_WillAfter2Received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,ConnectWithWillDuration10_timeout_WillAfter15Received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,ConnectWithWillDuration30_timeout_WillAfter45Received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}


TEST_F(LinuxUdpGateway_Will,ConnectWithWillDuration59_timeout_WillAfter89Received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,ConnectWithWillDuration61_timeout_WillAfter67received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

TEST_F(LinuxUdpGateway_Will,ConnectWithWillDuration90_timeout_WillAfter120received){
    // TODO implement me
    ASSERT_TRUE(false) << "TODO implement me";
    std::cout << std::endl;
}

// TODO implement will update functional tests
// TODO implement topic update => new topic received (not old)
// TODO implement message update => new message received (not old)
// TODO implement message update & topic update => new message & topic received (not old)

