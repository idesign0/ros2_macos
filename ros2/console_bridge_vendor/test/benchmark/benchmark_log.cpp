// Copyright 2020 Open Source Robotics Foundation, Inc.
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

#include <fstream>
#include <sstream>
#include <string>

#include "console_bridge/console.h"

#include "performance_test_fixture/performance_test_fixture.hpp"

using performance_test_fixture::PerformanceTest;

class OutputHandlerString : public console_bridge::OutputHandler
{
public:
  OutputHandlerString()
  {
  }

  ~OutputHandlerString() override
  {
  }

  void log(
    const std::string & text, console_bridge::LogLevel level, const char * filename,
    int line) override
  {
    (void)line;
    (void)filename;
    text_ = text;
    log_level_ = level;
  }

  std::string text_;
  console_bridge::LogLevel log_level_{console_bridge::LogLevel::CONSOLE_BRIDGE_LOG_NONE};
};

class PerformanceFileHandlerTest : public PerformanceTest
{
public:
  PerformanceFileHandlerTest()
  : log_filename_("/tmp/tmp.txt") {}

  virtual void SetUp(benchmark::State & st)
  {
    // Needs to be reset to avoid side effects from other tests
    console_bridge::setLogLevel(console_bridge::CONSOLE_BRIDGE_LOG_WARN);
    PerformanceTest::SetUp(st);
  }

  virtual void TearDown(benchmark::State & st)
  {
    PerformanceTest::TearDown(st);
    remove(log_filename());
  }

  std::string getTextFromLogFile()
  {
    std::ifstream f(log_filename_);
    std::stringstream result;
    result << f.rdbuf();
    return result.str();
  }

  const char * log_filename()
  {
    return log_filename_;
  }

private:
  const char * log_filename_;
};

BENCHMARK_F(PerformanceTest, log_to_console)(benchmark::State & st)
{
  const char * text = "Some logging text";
  console_bridge::setLogLevel(console_bridge::CONSOLE_BRIDGE_LOG_INFO);
  OutputHandlerString string_oh;
  console_bridge::useOutputHandler(&string_oh);
  for (auto _ : st) {
    CONSOLE_BRIDGE_logInform(text);
  }
}

BENCHMARK_F(PerformanceTest, log_to_console_to_low)(benchmark::State & st)
{
  const char * text = "Some logging text";
  console_bridge::setLogLevel(console_bridge::CONSOLE_BRIDGE_LOG_INFO);
  OutputHandlerString string_oh;
  console_bridge::useOutputHandler(&string_oh);
  for (auto _ : st) {
    CONSOLE_BRIDGE_logDebug(text);
  }
}

BENCHMARK_F(PerformanceFileHandlerTest, log_to_file)(benchmark::State & st)
{
  const char * text = "Some logging text";
  console_bridge::setLogLevel(console_bridge::CONSOLE_BRIDGE_LOG_INFO);
  console_bridge::OutputHandlerFile handler(log_filename());
  console_bridge::useOutputHandler(&handler);
  reset_heap_counters();
  for (auto _ : st) {
    CONSOLE_BRIDGE_logInform(text);
  }
}

BENCHMARK_F(PerformanceFileHandlerTest, log_to_file_to_low)(benchmark::State & st)
{
  const char * text = "Some logging text";
  console_bridge::setLogLevel(console_bridge::CONSOLE_BRIDGE_LOG_INFO);
  console_bridge::OutputHandlerFile handler(log_filename());
  console_bridge::useOutputHandler(&handler);
  reset_heap_counters();
  for (auto _ : st) {
    CONSOLE_BRIDGE_logDebug(text);
  }
}
