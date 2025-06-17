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

#include <atomic>
#include <string>
#include <sstream>
#include "keyboard_handler/keyboard_handler_base.hpp"

KEYBOARD_HANDLER_PUBLIC
constexpr KeyboardHandlerBase::callback_handle_t KeyboardHandlerBase::invalid_handle;

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerBase::callback_handle_t KeyboardHandlerBase::add_key_press_callback(
  const callback_t & callback, KeyboardHandlerBase::KeyCode key_code,
  KeyboardHandlerBase::KeyModifiers key_modifiers)
{
  if (callback == nullptr || !is_init_succeed_) {
    return invalid_handle;
  }
  std::lock_guard<std::mutex> lk(callbacks_mutex_);
  callback_handle_t new_handle = get_new_handle();
  callbacks_.emplace(
    KeyAndModifiers{key_code, key_modifiers},
    callback_data{new_handle, callback});
  return new_handle;
}

KEYBOARD_HANDLER_PUBLIC
bool operator&&(
  const KeyboardHandlerBase::KeyModifiers & left,
  const KeyboardHandlerBase::KeyModifiers & right)
{
  using KeyModifiers = KeyboardHandlerBase::KeyModifiers;
  /* *INDENT-OFF* */
  return static_cast<std::underlying_type_t<KeyModifiers>>(left) &
         static_cast<std::underlying_type_t<KeyModifiers>>(right);
  /* *INDENT-ON* */
}

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerBase::KeyModifiers operator|(
  KeyboardHandlerBase::KeyModifiers left,
  const KeyboardHandlerBase::KeyModifiers & right)
{
  using KeyModifiers = KeyboardHandlerBase::KeyModifiers;
  /* *INDENT-OFF* */
  left = static_cast<KeyModifiers>(static_cast<std::underlying_type_t<KeyModifiers>>(left) |
                                   static_cast<std::underlying_type_t<KeyModifiers>>(right));
  /* *INDENT-ON* */
  return left;
}

KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerBase::KeyCode & operator++(KeyboardHandlerBase::KeyCode & key_code)
{
  using KeyCode = KeyboardHandlerBase::KeyCode;
/* *INDENT-OFF* */
  key_code = static_cast<KeyCode>(static_cast<std::underlying_type_t<KeyCode>>(key_code) + 1);
/* *INDENT-ON* */
  return key_code;
}

KEYBOARD_HANDLER_PUBLIC
std::string enum_key_code_to_str(KeyboardHandlerBase::KeyCode key_code)
{
  for (auto & it : ENUM_KEY_TO_STR_MAP) {
    if (it.inner_code == key_code) {
      return it.str;
    }
  }
  return std::string();
}

/// \brief Translate str value to it's keycode representation.
/// \param String key_code_str
/// \return KeyboardHandlerBase::Keycode
KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerBase::KeyCode enum_str_to_key_code(const std::string & key_code_str)
{
  for (auto & it : ENUM_KEY_TO_STR_MAP) {
    if (it.str == key_code_str) {
      return it.inner_code;
    }
  }
  return KeyboardHandlerBase::KeyCode::UNKNOWN;
}

KEYBOARD_HANDLER_PUBLIC
std::string enum_key_modifiers_to_str(KeyboardHandlerBase::KeyModifiers key_modifiers)
{
  using KeyModifiers = KeyboardHandlerBase::KeyModifiers;
  std::stringstream ss;
  if (key_modifiers && KeyModifiers::SHIFT) {
    ss << "SHIFT";
  }
  if (key_modifiers && KeyModifiers::CTRL) {
    ss.str().empty() ? ss << "CTRL" : ss << " CTRL";
  }
  if (key_modifiers && KeyModifiers::ALT) {
    ss.str().empty() ? ss << "ALT" : ss << " ALT";
  }
  return ss.str();
}

KEYBOARD_HANDLER_PUBLIC
void KeyboardHandlerBase::delete_key_press_callback(const callback_handle_t & handle) noexcept
{
  std::lock_guard<std::mutex> lk(callbacks_mutex_);
  for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it) {
    if (it->second.handle == handle) {
      callbacks_.erase(it);
      return;
    }
  }
}

KeyboardHandlerBase::callback_handle_t KeyboardHandlerBase::get_new_handle()
{
  static std::atomic<callback_handle_t> handle_count{0};
  return handle_count.fetch_add(1, std::memory_order_relaxed) + 1;
}
