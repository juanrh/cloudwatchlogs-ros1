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
#include <cloudwatch_logger/log_node_param_helper.h>
#include <aws_common/sdk_utils/parameter_reader.h>
#include <aws_common/sdk_utils/aws_error.h>

using namespace Aws::Client;
using namespace Aws::CloudWatchLogs::Utils;
using ::testing::_;
using ::testing::StrEq;
using ::testing::InSequence;
using ::testing::SetArgReferee;
using ::testing::DoAll;
using ::testing::Return;
using Aws::AwsError;

class ParameterReaderMock : public ParameterReaderInterface 
{
public:
  MOCK_CONST_METHOD2(ReadList, Aws::AwsError(const char *, std::vector<std::string> &));
  MOCK_CONST_METHOD2(ReadDouble, Aws::AwsError(const char *, double &));
  MOCK_CONST_METHOD2(ReadInt, Aws::AwsError(const char *, int &));
  MOCK_CONST_METHOD2(ReadBool, Aws::AwsError(const char *, bool &));
  MOCK_CONST_METHOD2(ReadString, Aws::AwsError(const char *, Aws::String &));
  MOCK_CONST_METHOD2(ReadStdString, Aws::AwsError(const char *, std::string &));
  MOCK_CONST_METHOD2(ReadMap, Aws::AwsError(const char *, std::map<std::string, std::string> &));

};

class LogNodeParamHelperFixture : public ::testing::Test
{
protected:
  std::shared_ptr<ParameterReaderMock> param_reader_ = std::make_shared<ParameterReaderMock>();
};

TEST_F(LogNodeParamHelperFixture, TestReadPublishFrequency)
{
    double expected_param_value = 42.0;

    {
      InSequence read_param_seq;

      EXPECT_CALL(*param_reader_, ReadDouble(StrEq(kNodeParamPublishFrequencyKey), _))
        .WillOnce(Return(AwsError::AWS_ERR_FAILURE)); 

      EXPECT_CALL(*param_reader_, ReadDouble(StrEq(kNodeParamPublishFrequencyKey), _))
        .WillOnce(Return(AwsError::AWS_ERR_NOT_FOUND)); 

      EXPECT_CALL(*param_reader_, ReadDouble(StrEq(kNodeParamPublishFrequencyKey), _))
        .WillOnce(
          DoAll(SetArgReferee<1>(expected_param_value), Return(AwsError::AWS_ERR_OK))
        );
    }

    double param = -1; 
    EXPECT_EQ(AwsError::AWS_ERR_FAILURE, ReadPublishFrequency(param_reader_, param));
    EXPECT_EQ(kNodePublishFrequencyDefaultValue, param);

    param = -1; 
    EXPECT_EQ(AwsError::AWS_ERR_NOT_FOUND, ReadPublishFrequency(param_reader_, param));
    EXPECT_EQ(kNodePublishFrequencyDefaultValue, param);
    
    param = -1; 
    EXPECT_EQ(AwsError::AWS_ERR_OK, ReadPublishFrequency(param_reader_, param));
    EXPECT_EQ(expected_param_value, param);
}

TEST_F(LogNodeParamHelperFixture, TestReadLogGroup)
{
    std::string expected_param_value = "MTc4MGNjOTc3ZTA1OTY";

    {
      InSequence read_param_seq;

      EXPECT_CALL(*param_reader_, ReadStdString(StrEq(kNodeParamLogGroupNameKey), _))
        .WillOnce(Return(AwsError::AWS_ERR_FAILURE)); 

      EXPECT_CALL(*param_reader_, ReadStdString(StrEq(kNodeParamLogGroupNameKey), _))
        .WillOnce(Return(AwsError::AWS_ERR_NOT_FOUND)); 

      EXPECT_CALL(*param_reader_, ReadStdString(StrEq(kNodeParamLogGroupNameKey), _))
        .WillOnce(
          DoAll(SetArgReferee<1>(expected_param_value), Return(AwsError::AWS_ERR_OK))
        );
    }

    std::string param = ""; 
    EXPECT_EQ(AwsError::AWS_ERR_FAILURE, ReadLogGroup(param_reader_, param));
    EXPECT_STREQ(kNodeLogGroupNameDefaultValue, param.c_str());

    param = ""; 
    EXPECT_EQ(AwsError::AWS_ERR_NOT_FOUND, ReadLogGroup(param_reader_, param));
    EXPECT_STREQ(kNodeLogGroupNameDefaultValue, param.c_str());
    
    param = ""; 
    EXPECT_EQ(AwsError::AWS_ERR_OK, ReadLogGroup(param_reader_, param));
    EXPECT_STREQ(expected_param_value.c_str(), param.c_str());
}

TEST_F(LogNodeParamHelperFixture, TestReadLogStream)
{
    std::string expected_param_value = "MGQyNTU3N2I3NzU1ZGIyNWQzMTZhYmVh";

    {
      InSequence read_param_seq;

      EXPECT_CALL(*param_reader_, ReadStdString(StrEq(kNodeParamLogStreamNameKey), _))
        .WillOnce(Return(AwsError::AWS_ERR_FAILURE)); 

      EXPECT_CALL(*param_reader_, ReadStdString(StrEq(kNodeParamLogStreamNameKey), _))
        .WillOnce(Return(AwsError::AWS_ERR_NOT_FOUND)); 

      EXPECT_CALL(*param_reader_, ReadStdString(StrEq(kNodeParamLogStreamNameKey), _))
        .WillOnce(
          DoAll(SetArgReferee<1>(expected_param_value), Return(AwsError::AWS_ERR_OK))
        );
    }

    std::string param = ""; 
    EXPECT_EQ(AwsError::AWS_ERR_FAILURE, ReadLogStream(param_reader_, param));
    EXPECT_STREQ(kNodeLogStreamNameDefaultValue, param.c_str());

    param = ""; 
    EXPECT_EQ(AwsError::AWS_ERR_NOT_FOUND, ReadLogStream(param_reader_, param));
    EXPECT_STREQ(kNodeLogStreamNameDefaultValue, param.c_str());
    
    param = ""; 
    EXPECT_EQ(AwsError::AWS_ERR_OK, ReadLogStream(param_reader_, param));
    EXPECT_STREQ(expected_param_value.c_str(), param.c_str());
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}