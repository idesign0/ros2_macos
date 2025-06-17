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
#include <unistd.h>
#include <algorithm>
#include <csignal>
#include <exception>
#include <iostream>
#include <string>
#include <tuple>
#include "keyboard_handler/keyboard_handler_unix_impl.hpp"

std::atomic_bool KeyboardHandlerUnixImpl::exit_{false};
struct termios KeyboardHandlerUnixImpl::old_term_settings_ = {};
KeyboardHandlerUnixImpl::tcsetattrFunction KeyboardHandlerUnixImpl::tcsetattr_fn_ = tcsetattr;
KeyboardHandlerUnixImpl::signal_handler_type KeyboardHandlerUnixImpl::old_sigint_handler_ =
  SIG_DFL;

void KeyboardHandlerUnixImpl::on_signal(int signal_number)
{
  auto old_sigint_handler = KeyboardHandlerUnixImpl::get_old_sigint_handler();
  // Restore buffer mode for stdin
  if (old_sigint_handler == SIG_DFL) {
    if (KeyboardHandlerUnixImpl::restore_buffer_mode_for_stdin()) {
      _exit(EXIT_SUCCESS);
    } else {
      _exit(EXIT_FAILURE);
    }
  } else {
    exit_ = true;
    KeyboardHandlerUnixImpl::restore_buffer_mode_for_stdin();
  }

  if ((old_sigint_handler != SIG_ERR) &&
    (old_sigint_handler != SIG_IGN) &&
    (old_sigint_handler != SIG_DFL))
  {
    old_sigint_handler(signal_number);
  }
}

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerUnixImpl::KeyboardHandlerUnixImpl()
: KeyboardHandlerUnixImpl(read, isatty, tcgetattr, tcsetattr) {}

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerUnixImpl::KeyboardHandlerUnixImpl(bool install_signal_handler)
: KeyboardHandlerUnixImpl(read, isatty, tcgetattr, tcsetattr, install_signal_handler) {}

std::tuple<KeyboardHandlerBase::KeyCode, KeyboardHandlerBase::KeyModifiers>
KeyboardHandlerUnixImpl::parse_input(const char * buff, ssize_t read_bytes)
{
#ifdef PRINT_DEBUG_INFO
  std::cout << "Read " << read_bytes << " bytes: ";
  if (read_bytes > 1) {
    std::cout << "[] = {";
    for (ssize_t i = 0; i < read_bytes; ++i) {
      std::cout << static_cast<int>(buff[i]) << ", ";
    }
    std::cout << "'\\0'};";
  } else {
    std::cout << " : " << static_cast<int>(buff[0]) << " : '" << buff[0] << "'";
  }
  std::cout << std::endl;
#endif
  KeyCode pressed_key_code = KeyCode::UNKNOWN;
  KeyModifiers key_modifiers = KeyModifiers::NONE;

  std::string buff_to_search = buff;
  ssize_t bytes_in_keycode = read_bytes;

  if (read_bytes == 2 && buff[0] == 27) {
    key_modifiers = KeyModifiers::ALT;
    buff_to_search = buff[1];
    bytes_in_keycode = 1;
  }

  if (bytes_in_keycode == 1 && buff_to_search[0] >= 'A' && buff_to_search[0] <= 'Z') {
    char original_key_code = buff_to_search[0];
    original_key_code += 32;
    buff_to_search = original_key_code;
    key_modifiers = key_modifiers | KeyModifiers::SHIFT;
  }

  auto key_map_it = key_codes_map_.find(buff_to_search);
  if (key_map_it != key_codes_map_.end()) {
    pressed_key_code = key_map_it->second;
  }

  // first search in key_codes_map_
  if (pressed_key_code == KeyCode::UNKNOWN && bytes_in_keycode == 1 &&
    static_cast<signed char>(buff_to_search[0]) >= 0 && buff_to_search[0] <= 26)
  {
    char original_key_code = buff_to_search[0];
    original_key_code += 96;    // small chars
    buff_to_search = original_key_code;
    key_modifiers = key_modifiers | KeyModifiers::CTRL;

    auto key_map_it = key_codes_map_.find(buff_to_search);
    if (key_map_it != key_codes_map_.end()) {
      pressed_key_code = key_map_it->second;
    }
  }
  return std::make_tuple(pressed_key_code, key_modifiers);
}

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerUnixImpl::KeyboardHandlerUnixImpl(
  const readFunction & read_fn,
  const isattyFunction & isatty_fn,
  const tcgetattrFunction & tcgetattr_fn,
  const tcsetattrFunction & tcsetattr_fn,
  bool install_signal_handler)
: stdin_fd_(fileno(stdin))
{
  if (read_fn == nullptr) {
    throw std::invalid_argument("KeyboardHandlerUnixImpl read_fn must be non-empty.");
  }
  if (isatty_fn == nullptr) {
    throw std::invalid_argument("KeyboardHandlerUnixImpl isatty_fn must be non-empty.");
  }
  if (tcgetattr_fn == nullptr) {
    throw std::invalid_argument("KeyboardHandlerUnixImpl tcgetattr_fn must be non-empty.");
  }
  if (tcsetattr_fn == nullptr) {
    throw std::invalid_argument("KeyboardHandlerUnixImpl tcsetattr_fn must be non-empty.");
  }
  tcsetattr_fn_ = tcsetattr_fn;

  for (size_t i = 0; i < STATIC_KEY_MAP_LENGTH; i++) {
    key_codes_map_.emplace(
      DEFAULT_STATIC_KEY_MAP[i].terminal_sequence,
      DEFAULT_STATIC_KEY_MAP[i].inner_code);
  }

  // Check if we can handle key press from std input
  if (!isatty_fn(stdin_fd_)) {
    // If stdin is not a real terminal (redirected to text file or pipe ) can't do much here
    // with keyboard handling.
    std::cerr << "stdin is not a terminal device. Keyboard handling disabled.";
    return;
  }

  struct termios new_term_settings;
  if (tcgetattr_fn(stdin_fd_, &old_term_settings_) == -1) {
    throw std::runtime_error("Error in tcgetattr(). errno = " + std::to_string(errno));
  }

  if (install_signal_handler) {
    // Setup signal handler to return
    old_sigint_handler_ = std::signal(SIGINT, KeyboardHandlerUnixImpl::on_signal);
    // terminal in original (buffered) mode in case of abnormal program termination.
    if (old_sigint_handler_ == SIG_ERR) {
      throw std::runtime_error("Error. Can't install SIGINT handler");
    }
  }
  install_signal_handler_ = install_signal_handler;

  new_term_settings = old_term_settings_;
  // Set stdin to unbuffered mode for reading directly from the stdin.
  // Disable canonical input and disable echo.
  new_term_settings.c_lflag &= ~(ICANON | ECHO);
  new_term_settings.c_cc[VMIN] = 0;   // 0 means purely timeout driven readout
  new_term_settings.c_cc[VTIME] = 1;  // Wait maximum for 0.1 sec since start of the read() call.

  if (tcsetattr_fn_(stdin_fd_, TCSANOW, &new_term_settings) == -1) {
    throw std::runtime_error("Error in tcsetattr(). errno = " + std::to_string(errno));
  }
  is_init_succeed_ = true;

  key_handler_thread_ = std::thread(
    [ = ]() {
      try {
        static constexpr size_t BUFF_LEN = 10;
        char buff[BUFF_LEN] = {0};
        do {
          ssize_t read_bytes = read_fn(stdin_fd_, buff, BUFF_LEN);
          if (read_bytes < 0 && errno != EAGAIN) {
            throw std::runtime_error("Error in read(). errno = " + std::to_string(errno));
          }

          if (read_bytes == 0) {
            // Do nothing. 0 means read() returned by timeout.
          } else {  // read_bytes > 0
            buff[std::min(BUFF_LEN - 1, static_cast<size_t>(read_bytes))] = '\0';

            auto key_code_and_modifiers = parse_input(buff, read_bytes);

            KeyCode pressed_key_code = std::get<0>(key_code_and_modifiers);
            KeyModifiers key_modifiers = std::get<1>(key_code_and_modifiers);

#ifdef PRINT_DEBUG_INFO
            auto modifiers_str = enum_key_modifiers_to_str(key_modifiers);
            std::cout << "pressed key: " << modifiers_str;
            if (!modifiers_str.empty()) {
              std::cout << " + ";
            }
            std::cout << "'" << enum_key_code_to_str(pressed_key_code) << "'" << std::endl;
#endif
            std::lock_guard<std::mutex> lk(callbacks_mutex_);
            auto range = callbacks_.equal_range(KeyAndModifiers{pressed_key_code, key_modifiers});
            for (auto it = range.first; it != range.second; ++it) {
              it->second.callback(pressed_key_code, key_modifiers);
            }
          }
        } while (!exit_.load());
      } catch (...) {
        thread_exception_ptr = std::current_exception();
      }

      // Restore buffer mode for stdin
      if (!restore_buffer_mode_for_stdin()) {
        if (thread_exception_ptr == nullptr) {
          try {
            throw std::runtime_error(
              "Error in tcsetattr old_term_settings. errno = " + std::to_string(errno));
          } catch (...) {
            thread_exception_ptr = std::current_exception();
          }
        } else {
          std::cerr <<
            "Error in tcsetattr old_term_settings. errno = " + std::to_string(errno) << std::endl;
        }
      }
    });
}

KeyboardHandlerUnixImpl::~KeyboardHandlerUnixImpl()
{
  if (install_signal_handler_) {
    signal_handler_type old_sigint_handler = std::signal(SIGINT, old_sigint_handler_);
    if (old_sigint_handler == SIG_ERR) {
      std::cerr << "Error. Can't install old SIGINT handler" << std::endl;
    }
    if (old_sigint_handler != KeyboardHandlerUnixImpl::on_signal) {
      std::cerr << "Error. Can't return old SIGINT handler, someone override our signal handler" <<
        std::endl;
      std::signal(SIGINT, old_sigint_handler);  // return overridden signal handler
    }
  }
  exit_ = true;
  if (key_handler_thread_.joinable()) {
    key_handler_thread_.join();
  }

  try {
    if (thread_exception_ptr != nullptr) {
      std::rethrow_exception(thread_exception_ptr);
    }
  } catch (const std::exception & e) {
    std::cerr << "Caught exception: \"" << e.what() << "\"\n";
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;
  }
}

KEYBOARD_HANDLER_PUBLIC
std::string
KeyboardHandlerUnixImpl::get_terminal_sequence(KeyboardHandlerUnixImpl::KeyCode key_code)
{
  std::string ret_str{};
  for (const auto & it : key_codes_map_) {
    if (it.second == key_code) {
      return it.first;
    }
  }
  return ret_str;
}

bool KeyboardHandlerUnixImpl::restore_buffer_mode_for_stdin()
{
  if (tcsetattr_fn_(fileno(stdin), TCSANOW, &old_term_settings_) == -1) {
    return false;
  }
  return true;
}

KeyboardHandlerUnixImpl::signal_handler_type KeyboardHandlerUnixImpl::get_old_sigint_handler()
{
  return old_sigint_handler_;
}

#endif  // #ifndef _WIN32
