// Copyright 2021 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <array>
#include <fstream>
#include <string>
#include <unordered_map>

#include "gtest/gtest.h"

#include "rcpputils/filesystem_helper.hpp"
#include "rmw_dds_common/security.hpp"

TEST(test_security, files_exist_no_prefix)
{
  rcpputils::fs::path dir = rcpputils::fs::path("./test_folder");
  rcpputils::fs::remove_all(dir);
  EXPECT_TRUE(rcpputils::fs::create_directories(dir));
  EXPECT_TRUE(rcpputils::fs::exists(dir));
  EXPECT_TRUE(rcpputils::fs::is_directory(dir));

  std::array<std::string, 6> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s"
  };
  for (const std::string & filename : required_files) {
    rcpputils::fs::path full_path = dir / filename;
    std::ofstream output_buffer{full_path.string()};
    output_buffer << "test";
    ASSERT_TRUE(rcpputils::fs::exists(full_path));
  }

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(rmw_dds_common::get_security_files("", dir.string(), security_files));

  EXPECT_EQ(
    security_files["IDENTITY_CA"],
    rcpputils::fs::path("./test_folder/identity_ca.cert.pem").string());
  EXPECT_EQ(
    security_files["CERTIFICATE"],
    rcpputils::fs::path("./test_folder/cert.pem").string());
  EXPECT_EQ(
    security_files["PRIVATE_KEY"],
    rcpputils::fs::path("./test_folder/key.pem").string());
  EXPECT_EQ(
    security_files["PERMISSIONS_CA"],
    rcpputils::fs::path("./test_folder/permissions_ca.cert.pem").string());
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    rcpputils::fs::path("./test_folder/governance.p7s").string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    rcpputils::fs::path("./test_folder/permissions.p7s").string());
}

TEST(test_security, files_exist_with_prefix)
{
  rcpputils::fs::path dir = rcpputils::fs::path("./test_folder");
  rcpputils::fs::remove_all(dir);
  EXPECT_TRUE(rcpputils::fs::create_directories(dir));
  EXPECT_TRUE(rcpputils::fs::exists(dir));
  EXPECT_TRUE(rcpputils::fs::is_directory(dir));

  std::array<std::string, 6> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s"
  };
  for (const std::string & filename : required_files) {
    rcpputils::fs::path full_path = dir / filename;
    std::ofstream output_buffer{full_path.string()};
    output_buffer << "test";
    ASSERT_TRUE(rcpputils::fs::exists(full_path));
  }

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(rmw_dds_common::get_security_files("file://", dir.string(), security_files));

  EXPECT_EQ(
    security_files["IDENTITY_CA"],
    "file://" + rcpputils::fs::path("./test_folder/identity_ca.cert.pem").string());
  EXPECT_EQ(
    security_files["CERTIFICATE"],
    "file://" + rcpputils::fs::path("./test_folder/cert.pem").string());
  EXPECT_EQ(
    security_files["PRIVATE_KEY"],
    "file://" + rcpputils::fs::path("./test_folder/key.pem").string());
  EXPECT_EQ(
    security_files["PERMISSIONS_CA"],
    "file://" + rcpputils::fs::path("./test_folder/permissions_ca.cert.pem").string());
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    "file://" + rcpputils::fs::path("./test_folder/governance.p7s").string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    "file://" + rcpputils::fs::path("./test_folder/permissions.p7s").string());
}

TEST(test_security, file_missing)
{
  rcpputils::fs::path dir = rcpputils::fs::path("./test_folder");
  rcpputils::fs::remove_all(dir);
  EXPECT_TRUE(rcpputils::fs::create_directories(dir));
  EXPECT_TRUE(rcpputils::fs::exists(dir));
  EXPECT_TRUE(rcpputils::fs::is_directory(dir));

  std::array<std::string, 5> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s"
  };
  for (const std::string & filename : required_files) {
    rcpputils::fs::path full_path = dir / filename;
    std::ofstream output_buffer{full_path.string()};
    output_buffer << "test";
    ASSERT_TRUE(rcpputils::fs::exists(full_path));
  }

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_FALSE(rmw_dds_common::get_security_files("", dir.string(), security_files));
  ASSERT_EQ(security_files.size(), 0UL);
}

TEST(test_security, optional_file_exist)
{
  rcpputils::fs::path dir = rcpputils::fs::path("./test_folder");
  rcpputils::fs::remove_all(dir);
  EXPECT_TRUE(rcpputils::fs::create_directories(dir));
  EXPECT_TRUE(rcpputils::fs::exists(dir));
  EXPECT_TRUE(rcpputils::fs::is_directory(dir));

  std::array<std::string, 7> required_files = {
    "identity_ca.cert.pem", "cert.pem", "key.pem",
    "permissions_ca.cert.pem", "governance.p7s", "permissions.p7s", "crl.pem",
  };
  for (const std::string & filename : required_files) {
    rcpputils::fs::path full_path = dir / filename;
    std::ofstream output_buffer{full_path.string()};
    output_buffer << "test";
    ASSERT_TRUE(rcpputils::fs::exists(full_path));
  }

  std::unordered_map<std::string, std::string> security_files;
  ASSERT_TRUE(rmw_dds_common::get_security_files("", dir.string(), security_files));

  EXPECT_EQ(
    security_files["IDENTITY_CA"],
    rcpputils::fs::path("./test_folder/identity_ca.cert.pem").string());
  EXPECT_EQ(
    security_files["CERTIFICATE"],
    rcpputils::fs::path("./test_folder/cert.pem").string());
  EXPECT_EQ(
    security_files["PRIVATE_KEY"],
    rcpputils::fs::path("./test_folder/key.pem").string());
  EXPECT_EQ(
    security_files["PERMISSIONS_CA"],
    rcpputils::fs::path("./test_folder/permissions_ca.cert.pem").string());
  EXPECT_EQ(
    security_files["GOVERNANCE"],
    rcpputils::fs::path("./test_folder/governance.p7s").string());
  EXPECT_EQ(
    security_files["PERMISSIONS"],
    rcpputils::fs::path("./test_folder/permissions.p7s").string());

  EXPECT_EQ(
    security_files["CRL"],
    rcpputils::fs::path("./test_folder/crl.pem").string());
}
