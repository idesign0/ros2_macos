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
#include "keyboard_handler/keyboard_handler_unix_impl.hpp"

/// Note that key code sequences translated by the terminal could be differ for different terminal
/// emulators. Please refer to the
/// https://ftp.metu.edu.tr/pub/mirrors/ftp.x.org/pub/X11R6.8.0/PDF/ctlseqs.pdf. Also according
/// to the https://man7.org/linux/man-pages/man5/terminfo.5.html application could reassign some
/// some key code sequences.

//* *INDENT-OFF* */
namespace xterm_seq {
static constexpr char CURSOR_UP[]              = {27, 91, 65, '\0'};
static constexpr char CURSOR_DOWN[]            = {27, 91, 66, '\0'};
static constexpr char CURSOR_ONE_STEP_RIGHT[]  = {27, 91, 67, '\0'};
static constexpr char CURSOR_ONE_STEP_LEFT[]   = {27, 91, 68, '\0'};
static constexpr char ENTER[]                  = {10, '\0'};
static constexpr char ESC[]                    = {27, '\0'};
static constexpr char SPACE[]                  = {32, '\0'};
static constexpr char QUOTATION_MARK[]         = {34, '\0'};
static constexpr char BACK_SLASH[]             = {92, '\0'};
static constexpr char BACK_SPACE[]             = {127, '\0'};

static constexpr char DELETE[]                 = {27, 91, 51, 126, '\0'};
static constexpr char END[]                    = {27, 91, 70, '\0'};
static constexpr char PG_DOWN[]                = {27, 91, 54, 126, '\0'};
static constexpr char PG_UP[]                  = {27, 91, 53, 126, '\0'};
static constexpr char HOME[]                   = {27, 91, 72, '\0'};
static constexpr char INSERT[]                 = {27, 91, 50, 126, '\0'};

static constexpr char F1[]  = {27, 79, 80, '\0'};  // == {27, 79, 'P', '\0'};
static constexpr char F2[]  = {27, 79, 81, '\0'};
static constexpr char F3[]  = {27, 79, 82, '\0'};
static constexpr char F4[]  = {27, 79, 83, '\0'};
static constexpr char F5[]  = {27, 91, 49, 53, 126, '\0'};
static constexpr char F6[]  = {27, 91, 49, 55, 126, '\0'};
static constexpr char F7[]  = {27, 91, 49, 56, 126, '\0'};
static constexpr char F8[]  = {27, 91, 49, 57, 126, '\0'};
static constexpr char F9[]  = {27, 91, 50, 48, 126, '\0'};
static constexpr char F10[] = {27, 91, 50, 49, 126, '\0'};
static constexpr char F11[] = {27, 91, 50, 51, 126, '\0'};
static constexpr char F12[] = {27, 91, 50, 52, 126, '\0'};

// static constexpr char SHIFT_F1[]  = {27, 91, 49, 59, 50, 80, '\0'};
// static constexpr char SHIFT_F2[]  = {27, 91, 49, 59, 50, 81, '\0'};
// static constexpr char SHIFT_F3[]  = {27, 91, 49, 59, 50, 82, '\0'};
// static constexpr char SHIFT_F4[]  = {27, 91, 49, 59, 50, 83, '\0'};
// static constexpr char SHIFT_F5[]  = {27, 91, 49, 53, 59, 50, 126, '\0'};
// Example: The same value for SHIFT_F5[] when numbers replaced by chars
// static constexpr char SHIFT_F5[] = {27, 91, '1', '5', ';', '2', '~', '\0'};
// static constexpr char SHIFT_F6[]  = {27, 91, 49, 55, 59, 50, 126, '\0'};
// static constexpr char SHIFT_F7[]  = {27, 91, 49, 56, 59, 50, 126, '\0'};
// static constexpr char SHIFT_F8[]  = {27, 91, 49, 57, 59, 50, 126, '\0'};
// static constexpr char SHIFT_F9[]  = {27, 91, 50, 48, 59, 50, 126, '\0'};
// static constexpr char SHIFT_F10[] = {27, 91, 50, 49, 59, 50, 126, '\0'};
// static constexpr char SHIFT_F11[] = {27, 91, 50, 51, 59, 50, 126, '\0'};
// static constexpr char SHIFT_F12[] = {27, 91, 50, 52, 59, 50, 126, '\0'};
}  // namespace xterm_seq

const KeyboardHandlerUnixImpl::KeyMap KeyboardHandlerUnixImpl::DEFAULT_STATIC_KEY_MAP[]  = {
  {KeyCode::CURSOR_UP,    xterm_seq::CURSOR_UP},
  {KeyCode::CURSOR_DOWN,  xterm_seq::CURSOR_DOWN},
  {KeyCode::CURSOR_RIGHT, xterm_seq::CURSOR_ONE_STEP_RIGHT},
  {KeyCode::CURSOR_LEFT,  xterm_seq::CURSOR_ONE_STEP_LEFT},
  {KeyCode::DELETE_KEY,   xterm_seq::DELETE},
  {KeyCode::END,          xterm_seq::END},
  {KeyCode::PG_DOWN,      xterm_seq::PG_DOWN},
  {KeyCode::PG_UP,        xterm_seq::PG_UP},
  {KeyCode::HOME,         xterm_seq::HOME},
  {KeyCode::INSERT,       xterm_seq::INSERT},

  {KeyCode::F1,           xterm_seq::F1},
  {KeyCode::F2,           xterm_seq::F2},
  {KeyCode::F3,           xterm_seq::F3},
  {KeyCode::F4,           xterm_seq::F4},
  {KeyCode::F5,           xterm_seq::F5},
  {KeyCode::F6,           xterm_seq::F6},
  {KeyCode::F7,           xterm_seq::F7},
  {KeyCode::F8,           xterm_seq::F8},
  {KeyCode::F9,           xterm_seq::F9},
  {KeyCode::F10,          xterm_seq::F10},
  {KeyCode::F11,          xterm_seq::F11},
  {KeyCode::F12,          xterm_seq::F12},

  {KeyCode::ENTER,        xterm_seq::ENTER},  // 10
  {KeyCode::ESCAPE,       xterm_seq::ESC},    // 27
  {KeyCode::SPACE,        xterm_seq::SPACE},  // 32

  {KeyCode::EXCLAMATION_MARK,     "!"},  // 33
  {KeyCode::QUOTATION_MARK,   xterm_seq::QUOTATION_MARK},  // 34
  {KeyCode::HASHTAG_SIGN,         "#"},  // 35  shift + KeyCode::NUMBER_3 = 51 - 16 = 35
  {KeyCode::DOLLAR_SIGN,          "$"},  // 36  shift + KeyCode::NUMBER_4 = 52 - 16 = 36
  {KeyCode::PERCENT_SIGN,         "%"},  // 37  shift + KeyCode::NUMBER_5 = 53 - 16 = 37
  {KeyCode::AMPERSAND,            "&"},  // 38  shift + KeyCode::NUMBER_7 = 55 - 17 = 38
  {KeyCode::APOSTROPHE,           "'"},  // 39
  {KeyCode::OPENING_PARENTHESIS,  "("},  // 40  shift + KeyCode::NUMBER_9 = 57 - 17 = 40
  {KeyCode::CLOSING_PARENTHESIS,  ")"},  // 41  shift + KeyCode::NUMBER_0 = 48 - 7  = 41
  {KeyCode::STAR,                 "*"},  // 42  shift + KeyCode::NUMBER_8 = 56 - 14 = 42
  {KeyCode::PLUS,                 "+"},  // 43
  {KeyCode::COMMA,                ","},  // 44
  {KeyCode::MINUS,                "-"},  // 45
  {KeyCode::DOT,                  "."},  // 46
  {KeyCode::RIGHT_SLASH,          "/"},  // 47
  {KeyCode::NUMBER_0,             "0"},  // 48
  {KeyCode::NUMBER_1,             "1"},  // 49
  {KeyCode::NUMBER_2,             "2"},  // 50
  {KeyCode::NUMBER_3,             "3"},  // 51
  {KeyCode::NUMBER_4,             "4"},  // 52
  {KeyCode::NUMBER_5,             "5"},  // 53
  {KeyCode::NUMBER_6,             "6"},  // 54
  {KeyCode::NUMBER_7,             "7"},  // 55
  {KeyCode::NUMBER_8,             "8"},  // 56
  {KeyCode::NUMBER_9,             "9"},  // 57
  {KeyCode::COLON,                ":"},  // 58
  {KeyCode::SEMICOLON,            ";"},  // 59
  {KeyCode::LEFT_ANGLE_BRACKET,   "<"},  // 60
  {KeyCode::EQUAL_SIGN,           "="},  // 61
  {KeyCode::RIGHT_ANGLE_BRACKET,  ">"},  // 62
  {KeyCode::QUESTION_MARK,        "?"},  // 63
  {KeyCode::AT,                   "@"},  // 64
//  {KeyCode::CAPITAL_A,            "A"},  // 65  'a' = 97 - 32 = 65
//  {KeyCode::CAPITAL_Z,            "Z"},  // 90  'z' = 122 - 32 = 90
  {KeyCode::LEFT_SQUARE_BRACKET,  "["},  // 91
  {KeyCode::BACK_SLASH,           xterm_seq::BACK_SLASH},  // 92
  {KeyCode::RIGHT_SQUARE_BRACKET, "]"},  // 93
  {KeyCode::CARET,                "^"},  // 94  shift + KeyCode::NUMBER_6 = 54 + 40 = 94
  {KeyCode::UNDERSCORE_SIGN,      "_"},  // 95
  {KeyCode::GRAVE_ACCENT_SIGN,    "`"},  // 96
  {KeyCode::A,                    "a"},  // 97
  {KeyCode::B,                    "b"},  // 98
  {KeyCode::C,                    "c"},  // 99
  {KeyCode::D,                    "d"},  // 100
  {KeyCode::E,                    "e"},  // 101
  {KeyCode::F,                    "f"},  // 102
  {KeyCode::G,                    "g"},  // 103
  {KeyCode::H,                    "h"},  // 104
  {KeyCode::I,                    "i"},  // 105
  {KeyCode::J,                    "j"},  // 106
  {KeyCode::K,                    "k"},  // 107
  {KeyCode::L,                    "l"},  // 108
  {KeyCode::M,                    "m"},  // 109
  {KeyCode::N,                    "n"},  // 110
  {KeyCode::O,                    "o"},  // 111
  {KeyCode::P,                    "p"},  // 112
  {KeyCode::Q,                    "q"},  // 113
  {KeyCode::R,                    "r"},  // 114
  {KeyCode::S,                    "s"},  // 115
  {KeyCode::T,                    "t"},  // 116
  {KeyCode::U,                    "u"},  // 117
  {KeyCode::V,                    "v"},  // 118
  {KeyCode::W,                    "w"},  // 119
  {KeyCode::X,                    "x"},  // 120
  {KeyCode::Y,                    "y"},  // 121
  {KeyCode::Z,                    "z"},  // 122
  {KeyCode::LEFT_CURLY_BRACKET,   "{"},  // 123
  {KeyCode::VERTICAL_BAR,         "|"},  // 124
  {KeyCode::RIGHT_CURLY_BRACKET,  "}"},  // 125
  {KeyCode::TILDA,                "~"},  // 126
  {KeyCode::BACK_SPACE,           xterm_seq::BACK_SPACE},  // 127
};
/* *INDENT-ON* */

const size_t KeyboardHandlerUnixImpl::STATIC_KEY_MAP_LENGTH =
  sizeof(KeyboardHandlerUnixImpl::DEFAULT_STATIC_KEY_MAP) / sizeof(KeyboardHandlerUnixImpl::KeyMap);

#endif  // #ifndef _WIN32
