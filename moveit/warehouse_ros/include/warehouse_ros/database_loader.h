// Copyright 2015 Fetch Robotics
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the Fetch Robotics nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/* Author: Connor Brew */

#ifndef WAREHOUSE_ROS_DATABASE_LOADER_
#define WAREHOUSE_ROS_DATABASE_LOADER_

#include <warehouse_ros/database_connection.h>
#include <pluginlib/class_loader.hpp>

namespace warehouse_ros
{
class DBConnectionStub : public DatabaseConnection
{
public:
  bool setParams(const std::string& /*host*/, unsigned /*port*/, float /*timeout*/) override
  {
    return false;
  }
  bool setTimeout(float /*timeout*/) override
  {
    return false;
  }
  bool connect() override
  {
    return false;
  }
  bool isConnected() override
  {
    return false;
  }
  void dropDatabase(const std::string& /*db_name*/) override
  {
    throw warehouse_ros::DbConnectException("Database is stub");
  }
  std::string messageType(const std::string& /*db_name*/, const std::string& /*collection_name*/) override
  {
    throw warehouse_ros::DbConnectException("Database is stub");
  }

protected:
  typename MessageCollectionHelper::Ptr openCollectionHelper(const std::string& db_name,
                                                             const std::string& collection_name) override;
};

/// \brief This class provides the mechanism to connect to a database and reads needed ROS parameters when appropriate.
class DatabaseLoader
{
public:
  /// \brief Takes a warehouse_ros DatabaseConnection.
  /// The DatabaseConnection is expected to have already been initialized.
  DatabaseLoader(const rclcpp::node_interfaces::NodeParametersInterface::SharedPtr& node_parameters);
  DatabaseLoader(const rclcpp::Node::SharedPtr& node);
  ~DatabaseLoader(){};

  /** \brief Load a database connection using pluginlib
   Looks for ROS params specifying which plugin/host/port to use. NodeHandle::searchParam()
   is used starting from ~ to look for warehouse_plugin, warehouse_host and warehouse_port. */
  typename DatabaseConnection::Ptr loadDatabase();

private:
  rclcpp::node_interfaces::NodeParametersInterface::SharedPtr node_parameters_;
  std::unique_ptr<pluginlib::ClassLoader<warehouse_ros::DatabaseConnection> > db_plugin_loader_;
};
}  // namespace warehouse_ros

#endif
