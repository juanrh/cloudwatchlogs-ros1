/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cloudwatch_logger/log_node.h>
#include <cloudwatch_logs_common/log_manager.h>
#include <cloudwatch_logs_common/log_manager_factory.h>
#include <rosgraph_msgs/Log.h>

using namespace Aws::CloudWatchLogs;
using namespace Aws::CloudWatchLogs::Utils;
using ::testing::_;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::Eq;

class LogManagerFactoryMock : public LogManagerFactory
{
  public:
    MOCK_METHOD4(CreateLogManager, std::shared_ptr<LogManager>(
    const std::string & log_group, const std::string & log_stream,
    const Aws::Client::ClientConfiguration & client_config, const Aws::SDKOptions & sdk_options));
};

// FIXME if needed, mock log_publisher and make LogManagerMock create a mock log publisher 
// available in a public member ... or not, just in the fixture 

class LogManagerMock : public LogManager
{
public: 
  LogManagerMock(): LogManager(nullptr) {}

  MOCK_CONST_METHOD1(RecordLog, ROSCloudWatchLogsErrors(const std::string & log_msg_formatted));
  MOCK_METHOD0(Service, ROSCloudWatchLogsErrors());
  // MOCK_METHOD1(LogManager, LogManager(const std::shared_ptr<LogPublisher> log_publisher));
};

class LogNodeFixture : public ::testing::Test
{
protected:
  std::shared_ptr<LogManagerMock> log_manager_ = std::make_shared<LogManagerMock>();
  std::shared_ptr<LogManagerFactoryMock> log_manager_factory_ = std::make_shared<LogManagerFactoryMock>();

  rosgraph_msgs::Log log_message_;
  
  void SetUp() override 
  {
    log_message_.name = "NjhkYjRkZjQ3N2Qw";
    log_message_.msg = "ZjQxOGE2MWM5MTFkMWNjMDVkMGY2OTZm";
    log_message_.level = rosgraph_msgs::Log::WARN;

  //   EXPECT_CALL(*log_manager_factory_, CreateLogManager(::testing::_, ::testing::_, ::testing::_, ::testing::_ ))
  //     .WillRepeatedly(Return(log_manager_));
  }

  std::shared_ptr<LogNode> build_test_subject(int8_t severity_level = rosgraph_msgs::Log::DEBUG) {
    return std::make_shared<LogNode>(severity_level);
  }

  rosgraph_msgs::Log::ConstPtr message_to_constptr(rosgraph_msgs::Log log_message)
  {
    return boost::make_shared<rosgraph_msgs::Log const>(log_message);
  }

  void initialize_log_node(std::shared_ptr<LogNode> & log_node) {
    std::string log_group = "YjFjMTM5YTEyNzliMjdlNjBlZGY1ZjQy";
    std::string log_stream = "hZDExYmNmMGJkMjMw";
    Aws::Client::ClientConfiguration config; 
    Aws::SDKOptions sdk_options;

    EXPECT_CALL(*log_manager_factory_, 
        CreateLogManager(StrEq(log_group), StrEq(log_stream), Eq(config), _))
      .WillOnce(Return(log_manager_));

    log_node->Initialize(log_group, log_stream, config, sdk_options, log_manager_factory_);  
  }


// add if required, with suitable mock expectations 
  // std::shared_ptr<LogNode> build_initialized_test_subject(int8_t severity_level, ) {
  //   return std::make_shared<LogNode>(severity_level);  
// }

};

TEST_F(LogNodeFixture, TestInitialize)
{
  std::shared_ptr<LogNode> test_subject = build_test_subject();
  initialize_log_node(test_subject); 
}

TEST_F(LogNodeFixture, TestRecordLogsUninitialized)
{
  std::shared_ptr<LogNode> test_subject = build_test_subject();
  
  test_subject->RecordLogs(message_to_constptr(log_message_));
}


TEST_F(LogNodeFixture, TestRecordLogInitialized)
{
  auto min_log_severity_ = rosgraph_msgs::Log::WARN;
  std::shared_ptr<LogNode> test_subject = build_test_subject(); // (min_log_severity_);

  initialize_log_node(test_subject); 

  // RecordLog(::testing::_))
  std::ostringstream log_name_reference_stream; 
  log_name_reference_stream << "[node name: " << log_message_.name << "]";

  // TODO test no topics and some 
  std::ostringstream log_topics_reference_stream; 
  log_topics_reference_stream << "[topics: ]";


  EXPECT_CALL(*log_manager_, 
      RecordLog(AllOf(
        HasSubstr("WARN"), HasSubstr(log_message_.msg), 
        HasSubstr(log_name_reference_stream.str()), HasSubstr(log_topics_reference_stream.str())
        )))
    .WillRepeatedly(Return(Aws::CloudWatchLogs::ROSCloudWatchLogsErrors::CW_LOGS_SUCCEEDED)); // FIXME match

// "0.000000000 WARN [node name: NjhkYjRkZjQ3N2Qw] [topics: ] ZjQxOGE2MWM5MTFkMWNjMDVkMGY2OTZm\n"

  // TODO test diffeent log levels: above and below, all of them 
  // TODO test assertions on message name: do they make sense?, should they take constants from somewhere else?
  log_message_.level = min_log_severity_;
  test_subject->RecordLogs(message_to_constptr(log_message_));
}

TEST_F(LogNodeFixture, TestTriggerLogPublisher)
{
  std::shared_ptr<LogNode> test_subject = build_test_subject();
  initialize_log_node(test_subject); 

  EXPECT_CALL(*log_manager_, Service())
    .WillOnce(Return(Aws::CloudWatchLogs::ROSCloudWatchLogsErrors::CW_LOGS_SUCCEEDED))
    .WillOnce(Return(Aws::CloudWatchLogs::ROSCloudWatchLogsErrors::CW_LOGS_FAILED))
    .WillOnce(Return(Aws::CloudWatchLogs::ROSCloudWatchLogsErrors::CW_LOGS_SUCCEEDED));
  
  ros::TimerEvent event1, event2, event3;
  test_subject->TriggerLogPublisher(event1);
  test_subject->TriggerLogPublisher(event2);
  test_subject->TriggerLogPublisher(event3);
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}