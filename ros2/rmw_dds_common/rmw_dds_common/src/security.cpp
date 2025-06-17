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

#include <string>
#include <utility>
#include <unordered_map>

#include "rcpputils/filesystem_helper.hpp"
#include "rmw_dds_common/security.hpp"

namespace rmw_dds_common
{

bool get_security_files(
  const std::string & prefix, const std::string & secure_root,
  std::unordered_map<std::string, std::string> & result)
{
  const std::unordered_map<std::string, std::string> required_files{
    {"IDENTITY_CA", "identity_ca.cert.pem"},
    {"CERTIFICATE", "cert.pem"},
    {"PRIVATE_KEY", "key.pem"},
    {"PERMISSIONS_CA", "permissions_ca.cert.pem"},
    {"GOVERNANCE", "governance.p7s"},
    {"PERMISSIONS", "permissions.p7s"},
  };

  const std::unordered_map<std::string, std::string> optional_files{
    {"CRL", "crl.pem"},
  };

  for (const std::pair<const std::string, std::string> & el : required_files) {
    rcpputils::fs::path full_path(secure_root);
    full_path /= el.second;
    if (!full_path.is_regular_file()) {
      result.clear();
      return false;
    }

    result[el.first] = prefix + full_path.string();
  }

  for (const std::pair<const std::string, std::string> & el : optional_files) {
    rcpputils::fs::path full_path(secure_root);
    full_path /= el.second;
    if (full_path.is_regular_file()) {
      result[el.first] = prefix + full_path.string();
    }
  }

  return true;
}

}  // namespace rmw_dds_common
