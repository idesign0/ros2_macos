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

#ifndef KEYBOARD_HANDLER__KEYBOARD_HANDLER_BASE_HPP_
#define KEYBOARD_HANDLER__KEYBOARD_HANDLER_BASE_HPP_

#include <functional>
#include <unordered_map>
#include <mutex>
#include <string>
#include "keyboard_handler/visibility_control.hpp"

// #define PRINT_DEBUG_INFO

class KeyboardHandlerBase
{
public:
  /// \brief Enum for possible keys press combinations which keyboard handler capable to handle.
  enum class KeyCode : uint32_t;

  /// \brief Enum for key modifiers such as CTRL, ALT and SHIFT pressed along side with base key.
  /// \details Enum represented as a bitmask and could contain multiple values. Multiple values
  /// can be settled up with overloaded `|` logical OR operator and extracted with `&&` operator.
  enum class KeyModifiers : uint32_t
  {
    NONE  = 0,
    SHIFT = 1,
    ALT   = 1 << 1,
    CTRL  = 1 << 2
  };

  /// \brief Type for callback functions
  using callback_t = std::function<void (KeyCode, KeyModifiers)>;
  using callback_handle_t = uint64_t;

  /// \brief Callback handle returning from add_key_press_callback and using as an argument for
  /// the delete_key_press_callback
  KEYBOARD_HANDLER_PUBLIC
  static constexpr callback_handle_t invalid_handle = 0;

  /// \brief Adding callable object as a handler for specified key press combination.
  /// \param callback Callable which will be called when key_code will be recognized.
  /// \param key_code Value from enum which corresponds to some predefined key press combination.
  /// \param key_modifiers Value from enum which corresponds to the key modifiers pressed along
  /// side with key.
  /// \return Return Newly created callback handle if callback was successfully added to the
  /// keyboard handler, returns invalid_handle if callback is nullptr or keyboard handler wasn't
  /// successfully initialized.
  KEYBOARD_HANDLER_PUBLIC
  callback_handle_t add_key_press_callback(
    const callback_t & callback,
    KeyboardHandlerBase::KeyCode key_code,
    KeyboardHandlerBase::KeyModifiers key_modifiers = KeyboardHandlerBase::KeyModifiers::NONE);

  /// \brief Delete callback from keyboard handler callback's list
  /// \param handle Callback's handle returned from #add_key_press_callback
  KEYBOARD_HANDLER_PUBLIC
  void delete_key_press_callback(const callback_handle_t & handle) noexcept;

protected:
  struct callback_data
  {
    callback_handle_t handle;
    callback_t callback;
  };

  struct KeyAndModifiers
  {
    KeyCode key_code;
    KeyModifiers key_modifiers;

    bool operator==(const KeyAndModifiers & rhs) const
    {
      return this->key_code == rhs.key_code && this->key_modifiers == rhs.key_modifiers;
    }

    bool operator!=(const KeyAndModifiers & rhs) const
    {
      return !operator==(rhs);
    }
  };

  /// \brief Specialized hash function for `unordered_map` with KeyAndModifiers
  struct key_and_modifiers_hash_fn
  {
    std::size_t operator()(const KeyAndModifiers & key_and_mod) const
    {
      using key_undertype = std::underlying_type_t<KeyCode>;
      using mods_undertype = std::underlying_type_t<KeyModifiers>;
      return std::hash<mods_undertype>()(static_cast<mods_undertype>(key_and_mod.key_modifiers)) ^
             (std::hash<key_undertype>()(static_cast<key_undertype>(key_and_mod.key_code)) << 3);
    }
  };

  bool is_init_succeed_ = false;
  std::mutex callbacks_mutex_;
  std::unordered_multimap<KeyAndModifiers, callback_data, key_and_modifiers_hash_fn> callbacks_;

private:
  static callback_handle_t get_new_handle();
};

enum class KeyboardHandlerBase::KeyCode: uint32_t
{
  UNKNOWN = 0,
  EXCLAMATION_MARK,
  QUOTATION_MARK,
  HASHTAG_SIGN,
  DOLLAR_SIGN,
  PERCENT_SIGN,
  AMPERSAND,
  APOSTROPHE,
  OPENING_PARENTHESIS,
  CLOSING_PARENTHESIS,
  STAR,
  PLUS,
  COMMA,
  MINUS,
  DOT,
  RIGHT_SLASH,
  NUMBER_0,
  NUMBER_1,
  NUMBER_2,
  NUMBER_3,
  NUMBER_4,
  NUMBER_5,
  NUMBER_6,
  NUMBER_7,
  NUMBER_8,
  NUMBER_9,
  COLON,
  SEMICOLON,
  LEFT_ANGLE_BRACKET,
  EQUAL_SIGN,
  RIGHT_ANGLE_BRACKET,
  QUESTION_MARK,
  AT,
  LEFT_SQUARE_BRACKET,
  BACK_SLASH,
  RIGHT_SQUARE_BRACKET,
  CARET,
  UNDERSCORE_SIGN,
  GRAVE_ACCENT_SIGN,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  LEFT_CURLY_BRACKET,
  VERTICAL_BAR,
  RIGHT_CURLY_BRACKET,
  TILDA,
  CURSOR_UP,
  CURSOR_DOWN,
  CURSOR_LEFT,
  CURSOR_RIGHT,
  ESCAPE,
  SPACE,
  ENTER,
  BACK_SPACE,
  DELETE_KEY,
  END,
  PG_DOWN,
  PG_UP,
  HOME,
  INSERT,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  END_OF_KEY_CODE_ENUM
};

/// \brief  Logical AND operator for KeyModifiers enum represented as a bitmask.
/// \return true if testing value in one of the operands present in a bitmask given in another
/// operand, otherwise false.
KEYBOARD_HANDLER_PUBLIC
bool operator&&(
  const KeyboardHandlerBase::KeyModifiers & left,
  const KeyboardHandlerBase::KeyModifiers & right);

/// \brief  Logical OR operator for KeyModifiers enum represented as a bitmask.
/// \param left KeyModifiers enum bitmask
/// \param right Modifier value to set in bitmask
/// \return new KeyModifiers bitmask value with settled bit from right side parameter
KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerBase::KeyModifiers operator|(
  KeyboardHandlerBase::KeyModifiers left,
  const KeyboardHandlerBase::KeyModifiers & right);

/// \brief Prefix increment operator for KeyCode enum values
KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerBase::KeyCode & operator++(KeyboardHandlerBase::KeyCode & key_code);

/// \brief Data type for mapping KeyCode enum value to it's string representation.
struct KeyCodeToStrMap
{
  KeyboardHandlerBase::KeyCode inner_code;
  const char * str;
};

/// \brief Lookup table for mapping KeyCode enum value to it's string representation.
static const KeyCodeToStrMap ENUM_KEY_TO_STR_MAP[] {
  {KeyboardHandlerBase::KeyCode::UNKNOWN, "UNKNOWN"},
  {KeyboardHandlerBase::KeyCode::EXCLAMATION_MARK, "!"},
  {KeyboardHandlerBase::KeyCode::QUOTATION_MARK, "QUOTATION_MARK"},
  {KeyboardHandlerBase::KeyCode::HASHTAG_SIGN, "#"},
  {KeyboardHandlerBase::KeyCode::DOLLAR_SIGN, "&"},
  {KeyboardHandlerBase::KeyCode::PERCENT_SIGN, "%"},
  {KeyboardHandlerBase::KeyCode::AMPERSAND, "&"},
  {KeyboardHandlerBase::KeyCode::APOSTROPHE, "'"},
  {KeyboardHandlerBase::KeyCode::OPENING_PARENTHESIS, "("},
  {KeyboardHandlerBase::KeyCode::CLOSING_PARENTHESIS, ")"},
  {KeyboardHandlerBase::KeyCode::STAR, "*"},
  {KeyboardHandlerBase::KeyCode::PLUS, "+"},
  {KeyboardHandlerBase::KeyCode::COMMA, ","},
  {KeyboardHandlerBase::KeyCode::DOT, "."},
  {KeyboardHandlerBase::KeyCode::RIGHT_SLASH, "/"},
  {KeyboardHandlerBase::KeyCode::NUMBER_1, "NUMBER_1"},
  {KeyboardHandlerBase::KeyCode::NUMBER_2, "NUMBER_2"},
  {KeyboardHandlerBase::KeyCode::NUMBER_3, "NUMBER_3"},
  {KeyboardHandlerBase::KeyCode::NUMBER_4, "NUMBER_4"},
  {KeyboardHandlerBase::KeyCode::NUMBER_5, "NUMBER_5"},
  {KeyboardHandlerBase::KeyCode::NUMBER_6, "NUMBER_6"},
  {KeyboardHandlerBase::KeyCode::NUMBER_7, "NUMBER_7"},
  {KeyboardHandlerBase::KeyCode::NUMBER_8, "NUMBER_8"},
  {KeyboardHandlerBase::KeyCode::NUMBER_9, "NUMBER_9"},
  {KeyboardHandlerBase::KeyCode::NUMBER_0, "NUMBER_0"},
  {KeyboardHandlerBase::KeyCode::MINUS, "MINUS"},
  {KeyboardHandlerBase::KeyCode::COLON, ":"},
  {KeyboardHandlerBase::KeyCode::SEMICOLON, ";"},
  {KeyboardHandlerBase::KeyCode::LEFT_ANGLE_BRACKET, "<"},
  {KeyboardHandlerBase::KeyCode::EQUAL_SIGN, "EQUAL_SIGN"},
  {KeyboardHandlerBase::KeyCode::RIGHT_ANGLE_BRACKET, ">"},
  {KeyboardHandlerBase::KeyCode::QUESTION_MARK, "?"},
  {KeyboardHandlerBase::KeyCode::AT, "@"},
  {KeyboardHandlerBase::KeyCode::A, "a"},
  {KeyboardHandlerBase::KeyCode::B, "b"},
  {KeyboardHandlerBase::KeyCode::C, "c"},
  {KeyboardHandlerBase::KeyCode::D, "d"},
  {KeyboardHandlerBase::KeyCode::E, "e"},
  {KeyboardHandlerBase::KeyCode::F, "f"},
  {KeyboardHandlerBase::KeyCode::G, "g"},
  {KeyboardHandlerBase::KeyCode::H, "h"},
  {KeyboardHandlerBase::KeyCode::I, "i"},
  {KeyboardHandlerBase::KeyCode::J, "j"},
  {KeyboardHandlerBase::KeyCode::K, "k"},
  {KeyboardHandlerBase::KeyCode::L, "l"},
  {KeyboardHandlerBase::KeyCode::M, "m"},
  {KeyboardHandlerBase::KeyCode::N, "n"},
  {KeyboardHandlerBase::KeyCode::O, "o"},
  {KeyboardHandlerBase::KeyCode::P, "p"},
  {KeyboardHandlerBase::KeyCode::Q, "q"},
  {KeyboardHandlerBase::KeyCode::R, "r"},
  {KeyboardHandlerBase::KeyCode::S, "s"},
  {KeyboardHandlerBase::KeyCode::T, "t"},
  {KeyboardHandlerBase::KeyCode::U, "u"},
  {KeyboardHandlerBase::KeyCode::V, "v"},
  {KeyboardHandlerBase::KeyCode::W, "w"},
  {KeyboardHandlerBase::KeyCode::X, "x"},
  {KeyboardHandlerBase::KeyCode::Y, "y"},
  {KeyboardHandlerBase::KeyCode::Z, "z"},
  {KeyboardHandlerBase::KeyCode::LEFT_SQUARE_BRACKET, "["},
  {KeyboardHandlerBase::KeyCode::BACK_SLASH, "BACK_SLASH"},
  {KeyboardHandlerBase::KeyCode::RIGHT_SQUARE_BRACKET, "]"},
  {KeyboardHandlerBase::KeyCode::CARET, "^"},
  {KeyboardHandlerBase::KeyCode::UNDERSCORE_SIGN, "_"},
  {KeyboardHandlerBase::KeyCode::GRAVE_ACCENT_SIGN, "`"},
  {KeyboardHandlerBase::KeyCode::LEFT_CURLY_BRACKET, "{"},
  {KeyboardHandlerBase::KeyCode::VERTICAL_BAR, "|"},
  {KeyboardHandlerBase::KeyCode::RIGHT_CURLY_BRACKET, "}"},
  {KeyboardHandlerBase::KeyCode::TILDA, "~"},
  {KeyboardHandlerBase::KeyCode::CURSOR_UP, "CURSOR_UP"},
  {KeyboardHandlerBase::KeyCode::CURSOR_DOWN, "CURSOR_DOWN"},
  {KeyboardHandlerBase::KeyCode::CURSOR_LEFT, "CURSOR_LEFT"},
  {KeyboardHandlerBase::KeyCode::CURSOR_RIGHT, "CURSOR_RIGHT"},
  {KeyboardHandlerBase::KeyCode::ESCAPE, "ESCAPE"},
  {KeyboardHandlerBase::KeyCode::SPACE, "SPACE"},
  {KeyboardHandlerBase::KeyCode::ENTER, "ENTER"},
  {KeyboardHandlerBase::KeyCode::BACK_SPACE, "BACK_SPACE"},
  {KeyboardHandlerBase::KeyCode::DELETE_KEY, "DELETE_KEY"},
  {KeyboardHandlerBase::KeyCode::END, "END"},
  {KeyboardHandlerBase::KeyCode::PG_DOWN, "PG_DOWN"},
  {KeyboardHandlerBase::KeyCode::PG_UP, "PG_UP"},
  {KeyboardHandlerBase::KeyCode::HOME, "HOME"},
  {KeyboardHandlerBase::KeyCode::INSERT, "INSERT"},
  {KeyboardHandlerBase::KeyCode::F1, "F1"},
  {KeyboardHandlerBase::KeyCode::F2, "F2"},
  {KeyboardHandlerBase::KeyCode::F3, "F3"},
  {KeyboardHandlerBase::KeyCode::F4, "F4"},
  {KeyboardHandlerBase::KeyCode::F5, "F5"},
  {KeyboardHandlerBase::KeyCode::F6, "F6"},
  {KeyboardHandlerBase::KeyCode::F7, "F7"},
  {KeyboardHandlerBase::KeyCode::F8, "F8"},
  {KeyboardHandlerBase::KeyCode::F9, "F9"},
  {KeyboardHandlerBase::KeyCode::F10, "F10"},
  {KeyboardHandlerBase::KeyCode::F11, "F11"},
  {KeyboardHandlerBase::KeyCode::F12, "F12"},
};

/// \brief Translate KeyCode enum value to it's string representation.
/// \param key_code Value from enum which corresponds to some predefined key press combination.
/// \return String corresponding to the specified enum value in ENUM_KEY_TO_STR_MAP lookup table.
KEYBOARD_HANDLER_PUBLIC
std::string enum_key_code_to_str(KeyboardHandlerBase::KeyCode key_code);

/// \brief Translate str value to it's keycode representation.
/// \param String key_code_str
/// \return KeyboardHandlerBase::Keycode
KEYBOARD_HANDLER_PUBLIC
KeyboardHandlerBase::KeyCode enum_str_to_key_code(const std::string & key_code_str);

/// \brief Translate KeyModifiers enum value to it's string representation.
/// \param key_modifiers bitmask with key modifiers
/// \return String corresponding to the specified enum value.
KEYBOARD_HANDLER_PUBLIC
std::string enum_key_modifiers_to_str(KeyboardHandlerBase::KeyModifiers key_modifiers);

#endif  // KEYBOARD_HANDLER__KEYBOARD_HANDLER_BASE_HPP_
