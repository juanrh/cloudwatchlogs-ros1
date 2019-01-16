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

#pragma once

#include <aws_common/sdk_utils/aws_error.h>
#include <aws_common/sdk_utils/parameter_reader.h>
#include <ros/ros.h>
#include <rosgraph_msgs/Log.h>

namespace Aws {
namespace CloudWatchLogs {
namespace Utils {

Aws::AwsError ReadPublishFrequency(
  std::shared_ptr<Aws::Client::ParameterReaderInterface> parameter_reader,
  double & publish_frequency);

Aws::AwsError ReadLogGroup(std::shared_ptr<Aws::Client::ParameterReaderInterface> parameter_reader,
                           std::string & log_group);


Aws::AwsError ReadLogStream(std::shared_ptr<Aws::Client::ParameterReaderInterface> parameter_reader,
                            std::string & log_stream);

Aws::AwsError ReadSubscribeToRosout(
  std::shared_ptr<Aws::Client::ParameterReaderInterface> parameter_reader,
  bool & subscribe_to_rosout);

Aws::AwsError ReadMinLogVerbosity(
  std::shared_ptr<Aws::Client::ParameterReaderInterface> parameter_reader,
  int8_t & min_log_verbosity);

Aws::AwsError ReadSubscriberList(
  std::shared_ptr<Aws::Client::ParameterReaderInterface> parameter_reader,
  std::vector<ros::Subscriber> & subscriptions, ros::NodeHandle & nh, bool & subscribe_to_rosout,
  boost::function<void(const rosgraph_msgs::Log::ConstPtr &)> callback);

}  // namespace Utils
}  // namespace CloudWatchLogs
}  // namespace Aws
