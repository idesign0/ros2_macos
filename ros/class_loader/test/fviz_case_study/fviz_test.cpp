// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#include <gtest/gtest.h>

#include <class_loader/class_loader.hpp>

#include "./fviz.hpp"
#include "./fviz_plugin_base.hpp"

const std::string name = class_loader::systemLibraryFormat("class_loader_Test_FvizDefaultPlugin"); // NOLINT

TEST(FvizTest, basic_test) {
  try {
    class_loader::ClassLoader loader(name);
    loader.createInstance<FvizPluginBase>("Bar")->speak();
    loader.createInstance<FvizPluginBase>("Baz")->speak();
  } catch (const class_loader::ClassLoaderException & e) {
    FAIL() << "ClassLoaderException: " << e.what() << "\n";
  }

  SUCCEED();
}
