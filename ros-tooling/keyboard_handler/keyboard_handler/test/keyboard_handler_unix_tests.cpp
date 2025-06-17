// Copyright 2021 Apex.AI, Inc. or its affiliates. All Rights Reserved.
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

#ifndef _WIN32
#include <stdlib.h>
#include <algorithm>
#include <condition_variable>
#include <csignal>
#include <memory>
#include <string>
#include <utility>
#include <tuple>
#include "gmock/gmock.h"
#include "fake_recorder.hpp"
#include "fake_player.hpp"
#include "keyboard_handler/keyboard_handler_unix_impl.hpp"

using ::testing::Return;
using ::testing::Eq;
using ::testing::AtLeast;
using ::testing::_;
using ::testing::NiceMock;

namespace
{
int isatty_mock(int fd) {return 1;}

int tcgetattr_mock(int fd, struct termios * termios_p) {return 0;}

int tcsetattr_mock(int fd, int optional_actions, const struct termios * termios_p) {return 0;}
}  // namespace

// Mock the public system calls APIs. read() function become the stub function.
class MockSystemCalls
{
public:
  ssize_t read(int fd, void * buff_ptr, size_t n_bytes)
  {
    std::unique_lock<std::mutex> lk(read_fn_mutex_);
    if (wait_on_read_) {
      cv_read_.wait(lk, [this]() {return unblock_read_;});
      unblock_read_ = false;
    }
    strncpy(static_cast<char *>(buff_ptr), read_returning_str_value_.c_str(), n_bytes);
    return read_returning_str_value_.length();
  }

  void read_will_return_once(const std::string & str)
  {
    {
      std::lock_guard<std::mutex> lk(read_fn_mutex_);
      read_returning_str_value_ = str;
      wait_on_read_ = true;
      unblock_read_ = true;
    }
    cv_read_.notify_all();
  }

  void unblock_read()
  {
    {
      std::lock_guard<std::mutex> lk(read_fn_mutex_);
      unblock_read_ = true;
      wait_on_read_ = false;
    }
    cv_read_.notify_all();
  }

  void block_read()
  {
    {
      std::lock_guard<std::mutex> lk(read_fn_mutex_);
      unblock_read_ = false;
      wait_on_read_ = true;
    }
    cv_read_.notify_all();
  }


  void read_will_repeatedly_return(const std::string & str)
  {
    {
      std::lock_guard<std::mutex> lk(read_fn_mutex_);
      read_returning_str_value_ = str;
      unblock_read_ = true;
      wait_on_read_ = false;
    }
    cv_read_.notify_all();
  }

private:
  std::mutex read_fn_mutex_;
  std::condition_variable cv_read_;
  // By default read will block and wait
  bool wait_on_read_{true};
  bool unblock_read_{false};
  std::string read_returning_str_value_{};
};

std::shared_ptr<MockSystemCalls> g_system_calls_stub;

class MockKeyboardHandler : public KeyboardHandlerUnixImpl
{
public:
  explicit MockKeyboardHandler(
    const readFunction & read_fn,
    const isattyFunction & isatty_fn = isatty_mock,
    std::weak_ptr<MockSystemCalls> system_calls_stub = g_system_calls_stub,
    bool install_signal_handler = false)
  : KeyboardHandlerUnixImpl(read_fn, isatty_fn, tcgetattr_mock, tcsetattr_mock,
      install_signal_handler),
    system_calls_stub_(std::move(system_calls_stub)) {}

  ~MockKeyboardHandler() override
  {
    auto sys_calls_stub = system_calls_stub_.lock();
    if (sys_calls_stub) {
      // unlock read to let inner worker thread to finish
      if (unblock_read_fn_on_destruction_) {
        sys_calls_stub->unblock_read();
      }
    }
  }
  size_t get_number_of_registered_callbacks() const
  {
    return callbacks_.size();
  }

  std::tuple<KeyCode, KeyModifiers> parse_input_mock(const char * buff, ssize_t read_bytes)
  {
    return parse_input(buff, read_bytes - 1);  // -1 to strip out null terminator
  }

  bool unblock_read_fn_on_destruction_{true};

private:
  std::weak_ptr<MockSystemCalls> system_calls_stub_;
};

class MockPlayer : public FakePlayer
{
public:
  MOCK_METHOD(
    void, callback_func,
    (KeyboardHandler::KeyCode key_code, KeyboardHandler::KeyModifiers key_modifiers));
};

class KeyboardHandlerUnixTest : public ::testing::Test
{
public:
  KeyboardHandlerUnixTest()
  {
    g_system_calls_stub = std::make_shared<MockSystemCalls>();
    read_fn_ = [](int fd, void * buf_ptr, size_t n_bytes) -> ssize_t {
        if (g_system_calls_stub) {
          return g_system_calls_stub->read(fd, buf_ptr, n_bytes);
        } else {
          std::cerr << "Call for non existing unique_ptr" << std::endl;
          return 0;
        }
      };
  }

  ~KeyboardHandlerUnixTest() override
  {
    g_system_calls_stub.reset();
  }

  static void on_signal(int signal_number)
  {
    running_ = false;
  }

protected:
  KeyboardHandlerUnixImpl::readFunction read_fn_ = nullptr;
  static std::atomic_bool running_;
};

std::atomic_bool KeyboardHandlerUnixTest::running_{true};

TEST_F(KeyboardHandlerUnixTest, nullptr_as_callback) {
  MockKeyboardHandler keyboard_handler(read_fn_);
  ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);
  EXPECT_EQ(
    KeyboardHandler::invalid_handle,
    keyboard_handler.add_key_press_callback(nullptr, KeyboardHandler::KeyCode::A));
  EXPECT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);
}

TEST_F(KeyboardHandlerUnixTest, enum_key_code_to_str) {
  using KeyCode = KeyboardHandler::KeyCode;
  for (auto key_code = KeyCode::UNKNOWN; key_code != KeyCode::END_OF_KEY_CODE_ENUM; ++key_code) {
    /* *INDENT-OFF* */
    EXPECT_FALSE(enum_key_code_to_str(key_code).empty()) <<
      "No string representation for KeyCode enum value = " <<
       static_cast<std::underlying_type_t<KeyCode>>(key_code);
    /* *INDENT-ON* */
  }
}

TEST_F(KeyboardHandlerUnixTest, unregister_callback) {
  MockKeyboardHandler keyboard_handler(read_fn_);
  const std::string terminal_seq =
    keyboard_handler.get_terminal_sequence(KeyboardHandler::KeyCode::E);

  auto lambda_as_callback = [](KeyboardHandler::KeyCode key_code,
      KeyboardHandler::KeyModifiers key_modifiers) {
      ASSERT_FALSE(true) << "This code should not be called \n";
    };
  auto callback_handle = keyboard_handler.add_key_press_callback(
    lambda_as_callback, KeyboardHandler::KeyCode::E, KeyboardHandler::KeyModifiers::NONE);

  EXPECT_NE(callback_handle, KeyboardHandler::invalid_handle);
  EXPECT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 1U);

  keyboard_handler.delete_key_press_callback(callback_handle);
  EXPECT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);

  // Try to delete callback one more time to make sure that it will be handled correctly
  keyboard_handler.delete_key_press_callback(callback_handle);
  EXPECT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);
  g_system_calls_stub->read_will_repeatedly_return(terminal_seq);
}

TEST_F(KeyboardHandlerUnixTest, stdin_is_not_a_terminal_device) {
  auto isatty_fail = [](int fd) -> int {return 0;};
  MockKeyboardHandler keyboard_handler(read_fn_, isatty_fail);
  ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);

  auto callback = [](KeyboardHandler::KeyCode key_code,
      KeyboardHandler::KeyModifiers key_modifiers) {
      ASSERT_FALSE(true) << "This code should not be called \n";
    };
  auto callback_handle = keyboard_handler.add_key_press_callback(
    callback, KeyboardHandler::KeyCode::E, KeyboardHandler::KeyModifiers::NONE);
  EXPECT_EQ(callback_handle, KeyboardHandler::invalid_handle);
}

TEST_F(KeyboardHandlerUnixTest, check_input_parser) {
  MockKeyboardHandler keyboard_handler(read_fn_);
  // Test for CTRL + ALT key modifiers
  const char CTRL_ALT_K[] = {27, 11, '\0'};
  auto key_code_and_modifiers = keyboard_handler.parse_input_mock(CTRL_ALT_K, sizeof(CTRL_ALT_K));
  KeyboardHandler::KeyCode pressed_key_code = std::get<0>(key_code_and_modifiers);
  KeyboardHandler::KeyModifiers pressed_key_modifiers = std::get<1>(key_code_and_modifiers);
  EXPECT_EQ(pressed_key_code, KeyboardHandler::KeyCode::K);
  EXPECT_TRUE(pressed_key_modifiers && KeyboardHandler::KeyModifiers::CTRL);
  EXPECT_TRUE(pressed_key_modifiers && KeyboardHandler::KeyModifiers::ALT);

  // Test for SHIFT + ALT key modifiers
  const char SHIFT_ALT_K[] = {27, 75, '\0'};
  key_code_and_modifiers = keyboard_handler.parse_input_mock(SHIFT_ALT_K, sizeof(SHIFT_ALT_K));
  pressed_key_code = std::get<0>(key_code_and_modifiers);
  pressed_key_modifiers = std::get<1>(key_code_and_modifiers);
  EXPECT_EQ(pressed_key_code, KeyboardHandler::KeyCode::K);
  KeyboardHandler::KeyModifiers expected_key_modifiers =
    KeyboardHandler::KeyModifiers::SHIFT | KeyboardHandler::KeyModifiers::ALT;
  EXPECT_EQ(pressed_key_modifiers, expected_key_modifiers);
}

TEST_F(KeyboardHandlerUnixTest, weak_ptr_in_callbacks) {
  auto recorder = FakeRecorder::create();
  std::shared_ptr<FakePlayer> player_shared_ptr(new FakePlayer());
  {
    MockKeyboardHandler keyboard_handler(read_fn_);
    const std::string terminal_seq =
      keyboard_handler.get_terminal_sequence(KeyboardHandler::KeyCode::CURSOR_UP);
    ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);

    // Capture std::cout to verify at the end of the test that callbacks was correctly processed
    testing::internal::CaptureStdout();

    recorder->register_callbacks(keyboard_handler);
    ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 1U);

    player_shared_ptr->register_callbacks(keyboard_handler);
    ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 2U);
    g_system_calls_stub->read_will_return_once(terminal_seq);
  }
  // Check that callbacks was called with proper key code.
  std::string test_output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(
    test_output.find("FakePlayer callback with key code = CURSOR_UP") != std::string::npos);
  EXPECT_TRUE(
    test_output.find("FakeRecorder callback with key code = CURSOR_UP") != std::string::npos);
}

TEST_F(KeyboardHandlerUnixTest, weak_ptr_in_callbacks_and_deleted_objects) {
  {
    MockKeyboardHandler keyboard_handler(read_fn_);
    const std::string terminal_seq =
      keyboard_handler.get_terminal_sequence(KeyboardHandler::KeyCode::CURSOR_UP);
    ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);
    {
      auto recorder = FakeRecorder::create();
      std::shared_ptr<FakePlayer> player_shared_ptr(new FakePlayer());

      // Capture std::cout to verify at the end of the test that callbacks for deleted objects
      // was correctly processed
      testing::internal::CaptureStdout();
      recorder->register_callbacks(keyboard_handler);
      ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 1U);

      player_shared_ptr->register_callbacks(keyboard_handler);
      ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 2U);
    }
    g_system_calls_stub->read_will_return_once(terminal_seq);
  }
  // Check that callbacks was called for deleted objects and processed properly
  std::string test_output = testing::internal::GetCapturedStdout();
  EXPECT_TRUE(
    test_output.find("Object for assigned callback FakePlayer() was deleted") != std::string::npos);
  EXPECT_TRUE(
    test_output.find("Object for assigned callback FakeRecorder() was deleted") !=
    std::string::npos);
}

TEST_F(KeyboardHandlerUnixTest, global_function_as_callback) {
  using KeyCode = KeyboardHandler::KeyCode;
  using KeyModifiers = KeyboardHandler::KeyModifiers;
  testing::MockFunction<void(KeyCode key_code, KeyModifiers key_modifiers)> mock_global_callback;

  EXPECT_CALL(
    mock_global_callback, Call(Eq(KeyCode::E), Eq(KeyModifiers::SHIFT))).Times(AtLeast(1));

  MockKeyboardHandler keyboard_handler(read_fn_);
  const std::string terminal_seq = "E";

  ASSERT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 0U);
  EXPECT_NE(
    KeyboardHandler::invalid_handle,
    keyboard_handler.add_key_press_callback(
      mock_global_callback.AsStdFunction(),
      KeyCode::E, KeyModifiers::SHIFT));

  EXPECT_EQ(keyboard_handler.get_number_of_registered_callbacks(), 1U);
  g_system_calls_stub->read_will_return_once(terminal_seq);
}

TEST_F(KeyboardHandlerUnixTest, class_member_as_callback) {
  MockKeyboardHandler keyboard_handler(read_fn_);
  const std::string terminal_seq =
    keyboard_handler.get_terminal_sequence(KeyboardHandler::KeyCode::CURSOR_DOWN);

  std::shared_ptr<MockPlayer> mock_player_shared_ptr(new MockPlayer());
  EXPECT_CALL(
    *mock_player_shared_ptr,
    callback_func(
      Eq(KeyboardHandler::KeyCode::CURSOR_DOWN),
      Eq(KeyboardHandler::KeyModifiers::NONE))).Times(AtLeast(1));
  EXPECT_CALL(
    *mock_player_shared_ptr,
    callback_func(Eq(KeyboardHandler::KeyCode::CURSOR_UP), testing::_)).Times(0);

  std::shared_ptr<FakePlayer> fake_player_shared_ptr(new FakePlayer());

  auto callback = std::bind(
    &MockPlayer::callback_func, mock_player_shared_ptr,
    std::placeholders::_1, std::placeholders::_2);

  EXPECT_NE(
    KeyboardHandler::invalid_handle,
    keyboard_handler.add_key_press_callback(callback, KeyboardHandler::KeyCode::CURSOR_UP));
  EXPECT_NE(
    KeyboardHandler::invalid_handle,
    keyboard_handler.add_key_press_callback(callback, KeyboardHandler::KeyCode::CURSOR_DOWN));

  g_system_calls_stub->read_will_return_once(terminal_seq);
}

TEST_F(KeyboardHandlerUnixTest, no_signal_handler) {
  auto process_id = fork();
  if (process_id == 0) {  // In child process
    MockKeyboardHandler keyboard_handler(read_fn_);
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    kill(process_id, SIGINT);
    int child_return_code = EXIT_FAILURE;
    EXPECT_NE(waitpid(process_id, &child_return_code, 0), -1);
    EXPECT_NE(WIFSIGNALED(child_return_code), 0);
    EXPECT_EQ(WTERMSIG(child_return_code), SIGINT);
    EXPECT_THAT(WEXITSTATUS(child_return_code), EXIT_SUCCESS);
  }
}

TEST_F(KeyboardHandlerUnixTest, install_signal_handler_with_exit) {
  auto process_id = fork();
  if (process_id == 0) {  // In child process
    MockKeyboardHandler keyboard_handler(read_fn_, isatty_mock, g_system_calls_stub, true);
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    kill(process_id, SIGINT);
    int status = EXIT_FAILURE;
    auto wait_ret_code = waitpid(process_id, &status, 0);
    EXPECT_NE(wait_ret_code, -1);
    EXPECT_EQ(wait_ret_code, process_id);
    EXPECT_EQ(WIFSIGNALED(status), 0) << "Process terminated by signal: " << WTERMSIG(status);
    EXPECT_THAT(WEXITSTATUS(status), EXIT_SUCCESS);
  }
}

TEST_F(KeyboardHandlerUnixTest, install_signal_handler_with_old_handler) {
  constexpr int expected_ret_code = 101;
  auto process_id = fork();
  if (process_id == 0) {  // In child process
    auto on_signal = [](int /* signal */) {
        _exit(expected_ret_code);
      };
    auto old_sigint_handler = std::signal(SIGINT, on_signal);
    EXPECT_NE(old_sigint_handler, SIG_ERR) << "Can't install SIGINT handler in test";
    MockKeyboardHandler keyboard_handler(read_fn_, isatty_mock, g_system_calls_stub, true);
    while (true) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    kill(process_id, SIGINT);
    int status = EXIT_FAILURE;
    auto wait_ret_code = waitpid(process_id, &status, 0);
    EXPECT_NE(wait_ret_code, -1);
    EXPECT_EQ(wait_ret_code, process_id);
    EXPECT_EQ(WIFSIGNALED(status), 0) << "Process terminated by signal: " << WTERMSIG(status);
    EXPECT_THAT(WEXITSTATUS(status), expected_ret_code);
  }
}

TEST_F(KeyboardHandlerUnixTest, force_exit_from_main_loop_after_signal_handling) {
  constexpr int expected_ret_code = 101;
  auto process_id = fork();

  if (process_id == 0) {  // In child process
    auto old_sigint_handler = std::signal(SIGINT, KeyboardHandlerUnixTest::on_signal);
    EXPECT_NE(old_sigint_handler, SIG_ERR) << "Can't install SIGINT handler in test";
    using KeyCode = KeyboardHandler::KeyCode;
    using KeyModifiers = KeyboardHandler::KeyModifiers;
    testing::MockFunction<void(KeyCode key_code, KeyModifiers key_modifiers)> mock_global_callback;

    {
      g_system_calls_stub->read_will_repeatedly_return("E");
      MockKeyboardHandler keyboard_handler(read_fn_, isatty_mock, g_system_calls_stub, true);
      keyboard_handler.unblock_read_fn_on_destruction_ = false;

      while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      // Emulate situation when canonical mode for read_fn_ was settled up in signal handler
      g_system_calls_stub->block_read();
    }
    // If test passes we should cleanly exit from KeyboardHandler destructor.
    // If test fails KeyboardHandler destructor will hang out forever and parent process will
    // terminate it by timeout
    _exit(expected_ret_code);
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    kill(process_id, SIGINT);

    int status = EXIT_FAILURE;
    pid_t wait_ret_code = 0;
    const std::chrono::seconds ktimeout = std::chrono::seconds(10);
    std::chrono::steady_clock::time_point const start = std::chrono::steady_clock::now();
    while (wait_ret_code == 0 && std::chrono::steady_clock::now() - start < ktimeout) {
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      // WNOHANG checks child processes without causing the caller to be blocked
      wait_ret_code = waitpid(process_id, &status, WNOHANG);
    }
    if (wait_ret_code == 0) {
      kill(process_id, SIGKILL);
    }
    EXPECT_NE(wait_ret_code, -1);
    EXPECT_EQ(wait_ret_code, process_id);
    EXPECT_EQ(WIFSIGNALED(status), 0) << "Process terminated by signal: " << WTERMSIG(status);
    EXPECT_THAT(WEXITSTATUS(status), expected_ret_code);
  }
}

TEST_F(KeyboardHandlerUnixTest, return_old_signal_handler_after_destruction) {
  auto on_signal = [](int /* signal */) {
      _exit(EXIT_SUCCESS);
    };
  auto old_sigint_handler = std::signal(SIGINT, on_signal);
  EXPECT_NE(old_sigint_handler, SIG_ERR) << "Can't install SIGINT handler in test";
  {
    MockKeyboardHandler keyboard_handler(read_fn_, isatty_mock, g_system_calls_stub, true);
  }
  old_sigint_handler = std::signal(SIGINT, SIG_DFL);
  EXPECT_EQ(old_sigint_handler, on_signal);
}
#endif  // #ifndef _WIN32
