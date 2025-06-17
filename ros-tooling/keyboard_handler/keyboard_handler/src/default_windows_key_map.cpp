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

#include "keyboard_handler/keyboard_handler_windows_impl.hpp"

// ch == 0 for F1 - F10 keys, ch == 0xE0 == 224 for all other control keys.
//* *INDENT-OFF* */

const KeyboardHandlerWindowsImpl::KeyMap KeyboardHandlerWindowsImpl::DEFAULT_STATIC_KEY_MAP[]  = {
  {KeyCode::HOME,                 {0xE0, 71}},
  {KeyCode::CURSOR_UP,            {0xE0, 72}},
  {KeyCode::PG_UP,                {0xE0, 73}},
  {KeyCode::CURSOR_LEFT,          {0xE0, 75}},
  {KeyCode::CURSOR_RIGHT,         {0xE0, 77}},
  {KeyCode::END,                  {0xE0, 79}},
  {KeyCode::CURSOR_DOWN,          {0xE0, 80}},
  {KeyCode::PG_DOWN,              {0xE0, 81}},
  {KeyCode::INSERT,               {0xE0, 82}},
  {KeyCode::DELETE_KEY,           {0xE0, 83}},

  {KeyCode::F1,                   {0,    59}},
  {KeyCode::F2,                   {0,    60}},
  {KeyCode::F3,                   {0,    61}},
  {KeyCode::F4,                   {0,    62}},
  {KeyCode::F5,                   {0,    63}},
  {KeyCode::F6,                   {0,    64}},
  {KeyCode::F7,                   {0,    65}},
  {KeyCode::F8,                   {0,    66}},
  {KeyCode::F9,                   {0,    67}},
  {KeyCode::F10,                  {0,    68}},
  {KeyCode::F11,                  {0xE0, 133}},
  {KeyCode::F12,                  {0xE0, 134}},

  {KeyCode::BACK_SPACE,           {8,   WinKeyCode::NOT_A_KEY}},  // 8
  {KeyCode::ENTER,                {13,  WinKeyCode::NOT_A_KEY}},  // 13
  {KeyCode::ESCAPE,               {27,  WinKeyCode::NOT_A_KEY}},  // 27
  {KeyCode::SPACE,                {32,  WinKeyCode::NOT_A_KEY}},  // 32
  {KeyCode::EXCLAMATION_MARK,     {'!', WinKeyCode::NOT_A_KEY}},  // 33
  {KeyCode::QUOTATION_MARK,       {34,  WinKeyCode::NOT_A_KEY}},  // 34
  {KeyCode::HASHTAG_SIGN,         {'#', WinKeyCode::NOT_A_KEY}},  // 35
  {KeyCode::DOLLAR_SIGN,          {'$', WinKeyCode::NOT_A_KEY}},  // 36
  {KeyCode::PERCENT_SIGN,         {'%', WinKeyCode::NOT_A_KEY}},  // 37
  {KeyCode::AMPERSAND,            {'&', WinKeyCode::NOT_A_KEY}},  // 38
  {KeyCode::APOSTROPHE,           {39,  WinKeyCode::NOT_A_KEY}},  // 39
  {KeyCode::OPENING_PARENTHESIS,  {'(', WinKeyCode::NOT_A_KEY}},  // 40
  {KeyCode::CLOSING_PARENTHESIS,  {')', WinKeyCode::NOT_A_KEY}},  // 41
  {KeyCode::STAR,                 {'*', WinKeyCode::NOT_A_KEY}},  // 42
  {KeyCode::PLUS,                 {'+', WinKeyCode::NOT_A_KEY}},  // 43
  {KeyCode::COMMA,                {',', WinKeyCode::NOT_A_KEY}},  // 44
  {KeyCode::MINUS,                {'-', WinKeyCode::NOT_A_KEY}},  // 45
  {KeyCode::DOT,                  {'.', WinKeyCode::NOT_A_KEY}},  // 46
  {KeyCode::RIGHT_SLASH,          {'/', WinKeyCode::NOT_A_KEY}},  // 47
  {KeyCode::NUMBER_0,             {'0', WinKeyCode::NOT_A_KEY}},  // 48
  {KeyCode::NUMBER_1,             {'1', WinKeyCode::NOT_A_KEY}},  // 49
  {KeyCode::NUMBER_2,             {'2', WinKeyCode::NOT_A_KEY}},  // 50
  {KeyCode::NUMBER_3,             {'3', WinKeyCode::NOT_A_KEY}},  // 51
  {KeyCode::NUMBER_4,             {'4', WinKeyCode::NOT_A_KEY}},  // 52
  {KeyCode::NUMBER_5,             {'5', WinKeyCode::NOT_A_KEY}},  // 53
  {KeyCode::NUMBER_6,             {'6', WinKeyCode::NOT_A_KEY}},  // 54
  {KeyCode::NUMBER_7,             {'7', WinKeyCode::NOT_A_KEY}},  // 55
  {KeyCode::NUMBER_8,             {'8', WinKeyCode::NOT_A_KEY}},  // 56
  {KeyCode::NUMBER_9,             {'9', WinKeyCode::NOT_A_KEY}},  // 57
  {KeyCode::COLON,                {':', WinKeyCode::NOT_A_KEY}},  // 58
  {KeyCode::SEMICOLON,            {';', WinKeyCode::NOT_A_KEY}},  // 59
  {KeyCode::LEFT_ANGLE_BRACKET,   {'<', WinKeyCode::NOT_A_KEY}},  // 60
  {KeyCode::EQUAL_SIGN,           {'=', WinKeyCode::NOT_A_KEY}},  // 61
  {KeyCode::RIGHT_ANGLE_BRACKET,  {'>', WinKeyCode::NOT_A_KEY}},  // 62
  {KeyCode::QUESTION_MARK,        {'?', WinKeyCode::NOT_A_KEY}},  // 63
  {KeyCode::AT,                   {'@', WinKeyCode::NOT_A_KEY}},  // 64
//  {KeyCode::CAPITAL_A,          {'A', WinKeyCode::NOT_A_KEY}},  // 65  'a' = 97 - 32 = 65
//  ...
//  {KeyCode::CAPITAL_Z,          {'Z', WinKeyCode::NOT_A_KEY}},  // 90  'z' = 122 - 32 = 90
  {KeyCode::LEFT_SQUARE_BRACKET,  {'[', WinKeyCode::NOT_A_KEY}},  // 91
  {KeyCode::BACK_SLASH,           {92,  WinKeyCode::NOT_A_KEY}},  // 92
  {KeyCode::RIGHT_SQUARE_BRACKET, {']', WinKeyCode::NOT_A_KEY}},  // 93
  {KeyCode::CARET,                {'^', WinKeyCode::NOT_A_KEY}},  // 94
  {KeyCode::UNDERSCORE_SIGN,      {'_', WinKeyCode::NOT_A_KEY}},  // 95
  {KeyCode::GRAVE_ACCENT_SIGN,    {'`', WinKeyCode::NOT_A_KEY}},  // 96
  {KeyCode::A,                    {'a', WinKeyCode::NOT_A_KEY}},  // 97
  {KeyCode::B,                    {'b', WinKeyCode::NOT_A_KEY}},  // 98
  {KeyCode::C,                    {'c', WinKeyCode::NOT_A_KEY}},  // 99
  {KeyCode::D,                    {'d', WinKeyCode::NOT_A_KEY}},  // 100
  {KeyCode::E,                    {'e', WinKeyCode::NOT_A_KEY}},  // 101
  {KeyCode::F,                    {'f', WinKeyCode::NOT_A_KEY}},  // 102
  {KeyCode::G,                    {'g', WinKeyCode::NOT_A_KEY}},  // 103
  {KeyCode::H,                    {'h', WinKeyCode::NOT_A_KEY}},  // 104
  {KeyCode::I,                    {'i', WinKeyCode::NOT_A_KEY}},  // 105
  {KeyCode::J,                    {'j', WinKeyCode::NOT_A_KEY}},  // 106
  {KeyCode::K,                    {'k', WinKeyCode::NOT_A_KEY}},  // 107
  {KeyCode::L,                    {'l', WinKeyCode::NOT_A_KEY}},  // 108
  {KeyCode::M,                    {'m', WinKeyCode::NOT_A_KEY}},  // 109
  {KeyCode::N,                    {'n', WinKeyCode::NOT_A_KEY}},  // 110
  {KeyCode::O,                    {'o', WinKeyCode::NOT_A_KEY}},  // 111
  {KeyCode::P,                    {'p', WinKeyCode::NOT_A_KEY}},  // 112
  {KeyCode::Q,                    {'q', WinKeyCode::NOT_A_KEY}},  // 113
  {KeyCode::R,                    {'r', WinKeyCode::NOT_A_KEY}},  // 114
  {KeyCode::S,                    {'s', WinKeyCode::NOT_A_KEY}},  // 115
  {KeyCode::T,                    {'t', WinKeyCode::NOT_A_KEY}},  // 116
  {KeyCode::U,                    {'u', WinKeyCode::NOT_A_KEY}},  // 117
  {KeyCode::V,                    {'v', WinKeyCode::NOT_A_KEY}},  // 118
  {KeyCode::W,                    {'w', WinKeyCode::NOT_A_KEY}},  // 119
  {KeyCode::X,                    {'x', WinKeyCode::NOT_A_KEY}},  // 120
  {KeyCode::Y,                    {'y', WinKeyCode::NOT_A_KEY}},  // 121
  {KeyCode::Z,                    {'z', WinKeyCode::NOT_A_KEY}},  // 122
  {KeyCode::LEFT_CURLY_BRACKET,   {'{', WinKeyCode::NOT_A_KEY}},  // 123
  {KeyCode::VERTICAL_BAR,         {'|', WinKeyCode::NOT_A_KEY}},  // 124
  {KeyCode::RIGHT_CURLY_BRACKET,  {'}', WinKeyCode::NOT_A_KEY}},  // 125
  {KeyCode::TILDA,                {'~', WinKeyCode::NOT_A_KEY}},  // 126
};
/* *INDENT-ON* */

const size_t KeyboardHandlerWindowsImpl::STATIC_KEY_MAP_LENGTH =
  sizeof(KeyboardHandlerWindowsImpl::DEFAULT_STATIC_KEY_MAP) /
  sizeof(KeyboardHandlerWindowsImpl::KeyMap);
