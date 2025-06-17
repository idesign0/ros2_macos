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

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#include <exception>
#include <iostream>
#include <tuple>
#include "keyboard_handler/keyboard_handler_windows_impl.hpp"

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerWindowsImpl::KeyboardHandlerWindowsImpl()
: KeyboardHandlerWindowsImpl(_isatty, _kbhit, _getch)
{
}

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerWindowsImpl::KeyboardHandlerWindowsImpl(
  const isattyFunction & isatty_fn,
  const kbhitFunction & kbhit_fn,
  const getchFunction & getch_fn)
: exit_(false)
{
  if (isatty_fn == nullptr) {
    throw std::invalid_argument("KeyboardHandlerWindowsImpl isatty_fn must be non-empty.");
  }
  if (kbhit_fn == nullptr) {
    throw std::invalid_argument("KeyboardHandlerWindowsImpl kbhit_fn must be non-empty.");
  }
  if (getch_fn == nullptr) {
    throw std::invalid_argument("KeyboardHandlerWindowsImpl getch_fn must be non-empty.");
  }

  for (size_t i = 0; i < STATIC_KEY_MAP_LENGTH; i++) {
    key_codes_map_.emplace(
      DEFAULT_STATIC_KEY_MAP[i].win_key_code,
      DEFAULT_STATIC_KEY_MAP[i].inner_code);
  }

  // Check if we can handle key press from std input
  if (!isatty_fn(_fileno(stdin))) {
    // If stdin is not a real terminal or console (redirected to file or pipe ) can't do much here
    // with keyboard handling.
    std::cerr << "stdin is not a terminal or console device. Keyboard handling disabled.";
    return;
  }

  is_init_succeed_ = true;

  key_handler_thread_ = std::thread(
    [ = ]() {
      try {
        do {
          if (kbhit_fn()) {
            WinKeyCode win_key_code{WinKeyCode::NOT_A_KEY, WinKeyCode::NOT_A_KEY};
            KeyModifiers key_modifiers = KeyModifiers::NONE;
            int ch = getch_fn();
            win_key_code.first = ch;
            if (::GetAsyncKeyState(VK_MENU) & 0x8000) {
              key_modifiers = KeyModifiers::ALT;
            }
            // When reading a function key or an arrow key, getch function must be called twice;
            // the first call returns 0 or 0xE0, and the second call returns the actual key code.
            // https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2012/078sfkak(v=vs.110)
            if (ch == 0 || ch == 0xE0) {  // 0xE0 == 224
              // ch == 0 for F1 - F10 keys, ch == 0xE0 for all other control keys.
              ch = getch_fn();
              win_key_code.second = ch;
            }

            auto key_code_and_modifiers = win_key_code_to_enums(win_key_code);
            KeyCode pressed_key_code = std::get<0>(key_code_and_modifiers);
            key_modifiers = key_modifiers | std::get<1>(key_code_and_modifiers);

#ifdef PRINT_DEBUG_INFO
            std::cout << "Pressed first key code = " << win_key_code.first << ". ";
            std::cout << "Second code = " << win_key_code.second << ".";
            auto modifiers_str = enum_key_modifiers_to_str(key_modifiers);
            std::cout << " Detected as pressed key: " << modifiers_str;
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
            // Wait for 0.1 sec to yield processor resources for another threads
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
        } while (!exit_.load());
      } catch (...) {
        thread_exception_ptr = std::current_exception();
      }
    });
}

KeyboardHandlerWindowsImpl::~KeyboardHandlerWindowsImpl()
{
  exit_ = true;
  if (key_handler_thread_.joinable()) {
    key_handler_thread_.join();
  }

  try {
    if (thread_exception_ptr != nullptr) {
      std::rethrow_exception(thread_exception_ptr);
    }
  } catch (const std::exception & e) {
    std::cerr << "Caught exception \"" << e.what() << "\"\n";
  } catch (...) {
    std::cerr << "Caught unknown exception" << std::endl;
  }
}

KEYBOARD_HANDLER_PUBLIC
std::tuple<KeyboardHandlerBase::KeyCode, KeyboardHandlerBase::KeyModifiers>
KeyboardHandlerWindowsImpl::win_key_code_to_enums(WinKeyCode win_key_code) const
{
  KeyCode pressed_key_code = KeyCode::UNKNOWN;
  KeyModifiers key_modifiers = KeyModifiers::NONE;

  if (win_key_code.first == 0 && win_key_code.second >= 94 && win_key_code.second <= 103) {
    win_key_code.second -= 35;  // F1..F10
    key_modifiers = key_modifiers | KeyModifiers::CTRL;
  }

  if (win_key_code.first == 0xE0 && (win_key_code.second == 138 || win_key_code.second == 137)) {
    win_key_code.second -= 4;  // F11 and F12
    key_modifiers = key_modifiers | KeyModifiers::CTRL;
  }

  if (win_key_code.first == 0 && win_key_code.second >= 84 && win_key_code.second <= 93) {
    win_key_code.second -= 25;  // F1..F10
    key_modifiers = key_modifiers | KeyModifiers::SHIFT;
  }

  if (win_key_code.first == 0xE0 && (win_key_code.second == 135 || win_key_code.second == 136)) {
    win_key_code.second -= 2;  // F11 and F12
    key_modifiers = key_modifiers | KeyModifiers::SHIFT;
  }

  if (win_key_code.first >= 'A' && win_key_code.first <= 'Z') {
    win_key_code.first += 32;
    key_modifiers = key_modifiers | KeyModifiers::SHIFT;
  }

  auto key_map_it = key_codes_map_.find(win_key_code);
  if (key_map_it != key_codes_map_.end()) {
    pressed_key_code = key_map_it->second;
  }

  // first search in key_codes_map_
  if (pressed_key_code == KeyCode::UNKNOWN && win_key_code.second == WinKeyCode::NOT_A_KEY &&
    win_key_code.first >= 0 && win_key_code.first <= 26)
  {
    win_key_code.first += 96;  // small chars
    key_modifiers = key_modifiers | KeyModifiers::CTRL;

    auto key_map_it = key_codes_map_.find(win_key_code);
    if (key_map_it != key_codes_map_.end()) {
      pressed_key_code = key_map_it->second;
    }
  }

  return std::make_tuple(pressed_key_code, key_modifiers);
}

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerWindowsImpl::WinKeyCode
KeyboardHandlerWindowsImpl::enum_key_code_to_win_code(KeyboardHandlerBase::KeyCode key_code) const
{
  for (const auto & it : key_codes_map_) {
    if (it.second == key_code) {
      return it.first;
    }
  }
  return {WinKeyCode::NOT_A_KEY, WinKeyCode::NOT_A_KEY};
}

#endif  // #ifdef _WIN32
