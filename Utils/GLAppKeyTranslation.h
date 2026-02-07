#pragma once

#ifdef __EMSCRIPTEN__

#define GLENV_RELEASE                EMSCRIPTEN_EVENT_KEYUP
#define GLENV_PRESS                  EMSCRIPTEN_EVENT_KEYDOWN

#define GLENV_MOUSE_RELEASE          0
#define GLENV_MOUSE_PRESS            1

#define GLENV_KEY_SPACE              32  // Space
#define GLENV_KEY_APOSTROPHE         39  // Apostrophe
#define GLENV_KEY_COMMA              44  // Comma
#define GLENV_KEY_MINUS              45  // Minus
#define GLENV_KEY_PERIOD             46  // Period
#define GLENV_KEY_SLASH              47  // Slash
#define GLENV_KEY_0                  48  // Digit 0
#define GLENV_KEY_1                  49  // Digit 1
#define GLENV_KEY_2                  50  // Digit 2
#define GLENV_KEY_3                  51  // Digit 3
#define GLENV_KEY_4                  52  // Digit 4
#define GLENV_KEY_5                  53  // Digit 5
#define GLENV_KEY_6                  54  // Digit 6
#define GLENV_KEY_7                  55  // Digit 7
#define GLENV_KEY_8                  56  // Digit 8
#define GLENV_KEY_9                  57  // Digit 9
#define GLENV_KEY_SEMICOLON          59  // Semicolon
#define GLENV_KEY_EQUAL              61  // Equal
#define GLENV_KEY_A                  97  // a
#define GLENV_KEY_B                  98  // b
#define GLENV_KEY_C                  99  // c
#define GLENV_KEY_D                  100 // d
#define GLENV_KEY_E                  101 // e
#define GLENV_KEY_F                  102 // f
#define GLENV_KEY_G                  103 // g
#define GLENV_KEY_H                  104 // h
#define GLENV_KEY_I                  105 // i
#define GLENV_KEY_J                  106 // j
#define GLENV_KEY_K                  107 // k
#define GLENV_KEY_L                  108 // l
#define GLENV_KEY_M                  109 // m
#define GLENV_KEY_N                  110 // n
#define GLENV_KEY_O                  111 // o
#define GLENV_KEY_P                  112 // p
#define GLENV_KEY_Q                  113 // q
#define GLENV_KEY_R                  114 // r
#define GLENV_KEY_S                  115 // s
#define GLENV_KEY_T                  116 // t
#define GLENV_KEY_U                  117 // u
#define GLENV_KEY_V                  118 // v
#define GLENV_KEY_W                  119 // w
#define GLENV_KEY_X                  120 // x
#define GLENV_KEY_Y                  121 // y
#define GLENV_KEY_Z                  122 // z
#define GLENV_KEY_LEFT_BRACKET       91  // Left Bracket
#define GLENV_KEY_BACKSLASH          92  // Backslash
#define GLENV_KEY_RIGHT_BRACKET      93  // Right Bracket
#define GLENV_KEY_GRAVE_ACCENT       96  // Grave Accent
#define GLENV_KEY_ESCAPE             27  // Escape
#define GLENV_KEY_ENTER              13  // Enter
#define GLENV_KEY_TAB                9   // Tab
#define GLENV_KEY_BACKSPACE          8   // Backspace
#define GLENV_KEY_INSERT             45  // Insert
#define GLENV_KEY_DELETE             46  // Delete
#define GLENV_KEY_RIGHT              39  // Arrow Right
#define GLENV_KEY_LEFT               37  // Arrow Left
#define GLENV_KEY_DOWN               40  // Arrow Down
#define GLENV_KEY_UP                 38  // Arrow Up
#define GLENV_KEY_PAGE_UP            33  // Page Up
#define GLENV_KEY_PAGE_DOWN          34  // Page Down
#define GLENV_KEY_HOME               36  // Home
#define GLENV_KEY_END                35  // End
#define GLENV_KEY_CAPS_LOCK          20  // Caps Lock
#define GLENV_KEY_SCROLL_LOCK        145 // Scroll Lock
#define GLENV_KEY_NUM_LOCK           144 // Num Lock
#define GLENV_KEY_PRINT_SCREEN       44  // Print Screen
#define GLENV_KEY_PAUSE              19  // Pause
#define GLENV_KEY_F1                 112 // F1
#define GLENV_KEY_F2                 113 // F2
#define GLENV_KEY_F3                 114 // F3
#define GLENV_KEY_F4                 115 // F4
#define GLENV_KEY_F5                 116 // F5
#define GLENV_KEY_F6                 117 // F6
#define GLENV_KEY_F7                 118 // F7
#define GLENV_KEY_F8                 119 // F8
#define GLENV_KEY_F9                 120 // F9
#define GLENV_KEY_F10                121 // F10
#define GLENV_KEY_F11                122 // F11
#define GLENV_KEY_F12                123 // F12
#define GLENV_KEY_KP_0               96  // Keypad 0
#define GLENV_KEY_KP_1               97  // Keypad 1
#define GLENV_KEY_KP_2               98  // Keypad 2
#define GLENV_KEY_KP_3               99  // Keypad 3
#define GLENV_KEY_KP_4               100 // Keypad 4
#define GLENV_KEY_KP_5               101 // Keypad 5
#define GLENV_KEY_KP_6               102 // Keypad 6
#define GLENV_KEY_KP_7               103 // Keypad 7
#define GLENV_KEY_KP_8               104 // Keypad 8
#define GLENV_KEY_KP_9               105 // Keypad 9
#define GLENV_KEY_KP_DECIMAL         110 // Keypad Decimal
#define GLENV_KEY_KP_DIVIDE          111 // Keypad Divide
#define GLENV_KEY_KP_MULTIPLY        106 // Keypad Multiply
#define GLENV_KEY_KP_SUBTRACT        109 // Keypad Subtract
#define GLENV_KEY_KP_ADD             107 // Keypad Add
#define GLENV_KEY_KP_ENTER           13  // Keypad Enter
#define GLENV_KEY_LEFT_SHIFT         16  // Left Shift
#define GLENV_KEY_LEFT_CONTROL       17  // Left Control
#define GLENV_KEY_LEFT_ALT           18  // Left Alt
#define GLENV_KEY_RIGHT_SHIFT        16  // Right Shift
#define GLENV_KEY_RIGHT_CONTROL      17  // Right Control
#define GLENV_KEY_RIGHT_ALT          18  // Right Alt
#define GLENV_KEY_MENU               93  // Menu

#define GLENV_MOUSE_BUTTON_LEFT      0
#define GLENV_MOUSE_BUTTON_RIGHT     2
#define GLENV_MOUSE_BUTTON_MIDDLE    1

#define GLENV_MOD_SHIFT              1
#define GLENV_MOD_CONTROL            2
#define GLENV_MOD_ALT                4

#else

#define GLENV_RELEASE                GLFW_RELEASE
#define GLENV_PRESS                  GLFW_PRESS

#define GLENV_MOUSE_RELEASE          GLFW_RELEASE
#define GLENV_MOUSE_PRESS            GLFW_PRESS

#define GLENV_KEY_SPACE              GLFW_KEY_SPACE
#define GLENV_KEY_APOSTROPHE         GLFW_KEY_APOSTROPHE
#define GLENV_KEY_COMMA              GLFW_KEY_COMMA
#define GLENV_KEY_MINUS              GLFW_KEY_MINUS
#define GLENV_KEY_PERIOD             GLFW_KEY_PERIOD
#define GLENV_KEY_SLASH              GLFW_KEY_SLASH
#define GLENV_KEY_0                  GLFW_KEY_0
#define GLENV_KEY_1                  GLFW_KEY_1
#define GLENV_KEY_2                  GLFW_KEY_2
#define GLENV_KEY_3                  GLFW_KEY_3
#define GLENV_KEY_4                  GLFW_KEY_4
#define GLENV_KEY_5                  GLFW_KEY_5
#define GLENV_KEY_6                  GLFW_KEY_6
#define GLENV_KEY_7                  GLFW_KEY_7
#define GLENV_KEY_8                  GLFW_KEY_8
#define GLENV_KEY_9                  GLFW_KEY_9
#define GLENV_KEY_SEMICOLON          GLFW_KEY_SEMICOLON
#define GLENV_KEY_EQUAL              GLFW_KEY_EQUAL
#define GLENV_KEY_A                  GLFW_KEY_A
#define GLENV_KEY_B                  GLFW_KEY_B
#define GLENV_KEY_C                  GLFW_KEY_C
#define GLENV_KEY_D                  GLFW_KEY_D
#define GLENV_KEY_E                  GLFW_KEY_E
#define GLENV_KEY_F                  GLFW_KEY_F
#define GLENV_KEY_G                  GLFW_KEY_G
#define GLENV_KEY_H                  GLFW_KEY_H
#define GLENV_KEY_I                  GLFW_KEY_I
#define GLENV_KEY_J                  GLFW_KEY_J
#define GLENV_KEY_K                  GLFW_KEY_K
#define GLENV_KEY_L                  GLFW_KEY_L
#define GLENV_KEY_M                  GLFW_KEY_M
#define GLENV_KEY_N                  GLFW_KEY_N
#define GLENV_KEY_O                  GLFW_KEY_O
#define GLENV_KEY_P                  GLFW_KEY_P
#define GLENV_KEY_Q                  GLFW_KEY_Q
#define GLENV_KEY_R                  GLFW_KEY_R
#define GLENV_KEY_S                  GLFW_KEY_S
#define GLENV_KEY_T                  GLFW_KEY_T
#define GLENV_KEY_U                  GLFW_KEY_U
#define GLENV_KEY_V                  GLFW_KEY_V
#define GLENV_KEY_W                  GLFW_KEY_W
#define GLENV_KEY_X                  GLFW_KEY_X
#define GLENV_KEY_Y                  GLFW_KEY_Y
#define GLENV_KEY_Z                  GLFW_KEY_Z
#define GLENV_KEY_LEFT_BRACKET       GLFW_KEY_LEFT_BRACKET
#define GLENV_KEY_BACKSLASH          GLFW_KEY_BACKSLASH
#define GLENV_KEY_RIGHT_BRACKET      GLFW_KEY_RIGHT_BRACKET
#define GLENV_KEY_GRAVE_ACCENT       GLFW_KEY_GRAVE_ACCENT
#define GLENV_KEY_WORLD_1            GLFW_KEY_WORLD_1
#define GLENV_KEY_WORLD_2            GLFW_KEY_WORLD_2
#define GLENV_KEY_ESCAPE             GLFW_KEY_ESCAPE
#define GLENV_KEY_ENTER              GLFW_KEY_ENTER
#define GLENV_KEY_TAB                GLFW_KEY_TAB
#define GLENV_KEY_BACKSPACE          GLFW_KEY_BACKSPACE
#define GLENV_KEY_INSERT             GLFW_KEY_INSERT
#define GLENV_KEY_DELETE             GLFW_KEY_DELETE
#define GLENV_KEY_RIGHT              GLFW_KEY_RIGHT
#define GLENV_KEY_LEFT               GLFW_KEY_LEFT
#define GLENV_KEY_DOWN               GLFW_KEY_DOWN
#define GLENV_KEY_UP                 GLFW_KEY_UP
#define GLENV_KEY_PAGE_UP            GLFW_KEY_PAGE_UP
#define GLENV_KEY_PAGE_DOWN          GLFW_KEY_PAGE_DOWN
#define GLENV_KEY_HOME               GLFW_KEY_HOME
#define GLENV_KEY_END                GLFW_KEY_END
#define GLENV_KEY_CAPS_LOCK          GLFW_KEY_CAPS_LOCK
#define GLENV_KEY_SCROLL_LOCK        GLFW_KEY_SCROLL_LOCK
#define GLENV_KEY_NUM_LOCK           GLFW_KEY_NUM_LOCK
#define GLENV_KEY_PRINT_SCREEN       GLFW_KEY_PRINT_SCREEN
#define GLENV_KEY_PAUSE              GLFW_KEY_PAUSE
#define GLENV_KEY_F1                 GLFW_KEY_F1
#define GLENV_KEY_F2                 GLFW_KEY_F2
#define GLENV_KEY_F3                 GLFW_KEY_F3
#define GLENV_KEY_F4                 GLFW_KEY_F4
#define GLENV_KEY_F5                 GLFW_KEY_F5
#define GLENV_KEY_F6                 GLFW_KEY_F6
#define GLENV_KEY_F7                 GLFW_KEY_F7
#define GLENV_KEY_F8                 GLFW_KEY_F8
#define GLENV_KEY_F9                 GLFW_KEY_F9
#define GLENV_KEY_F10                GLFW_KEY_F10
#define GLENV_KEY_F11                GLFW_KEY_F11
#define GLENV_KEY_F12                GLFW_KEY_F12
#define GLENV_KEY_F13                GLFW_KEY_F13
#define GLENV_KEY_F14                GLFW_KEY_F14
#define GLENV_KEY_F15                GLFW_KEY_F15
#define GLENV_KEY_F16                GLFW_KEY_F16
#define GLENV_KEY_F17                GLFW_KEY_F17
#define GLENV_KEY_F18                GLFW_KEY_F18
#define GLENV_KEY_F19                GLFW_KEY_F19
#define GLENV_KEY_F20                GLFW_KEY_F20
#define GLENV_KEY_F21                GLFW_KEY_F21
#define GLENV_KEY_F22                GLFW_KEY_F22
#define GLENV_KEY_F23                GLFW_KEY_F23
#define GLENV_KEY_F24                GLFW_KEY_F24
#define GLENV_KEY_F25                GLFW_KEY_F25
#define GLENV_KEY_KP_0               GLFW_KEY_KP_0
#define GLENV_KEY_KP_1               GLFW_KEY_KP_1
#define GLENV_KEY_KP_2               GLFW_KEY_KP_2
#define GLENV_KEY_KP_3               GLFW_KEY_KP_3
#define GLENV_KEY_KP_4               GLFW_KEY_KP_4
#define GLENV_KEY_KP_5               GLFW_KEY_KP_5
#define GLENV_KEY_KP_6               GLFW_KEY_KP_6
#define GLENV_KEY_KP_7               GLFW_KEY_KP_7
#define GLENV_KEY_KP_8               GLFW_KEY_KP_8
#define GLENV_KEY_KP_9               GLFW_KEY_KP_9
#define GLENV_KEY_KP_DECIMAL         GLFW_KEY_KP_DECIMAL
#define GLENV_KEY_KP_DIVIDE          GLFW_KEY_KP_DIVIDE
#define GLENV_KEY_KP_MULTIPLY        GLFW_KEY_KP_MULTIPLY
#define GLENV_KEY_KP_SUBTRACT        GLFW_KEY_KP_SUBTRACT
#define GLENV_KEY_KP_ADD             GLFW_KEY_KP_ADD
#define GLENV_KEY_KP_ENTER           GLFW_KEY_KP_ENTER
#define GLENV_KEY_KP_EQUAL           GLFW_KEY_KP_EQUAL
#define GLENV_KEY_LEFT_SHIFT         GLFW_KEY_LEFT_SHIFT
#define GLENV_KEY_LEFT_CONTROL       GLFW_KEY_LEFT_CONTROL
#define GLENV_KEY_LEFT_ALT           GLFW_KEY_LEFT_ALT
#define GLENV_KEY_LEFT_SUPER         GLFW_KEY_LEFT_SUPER
#define GLENV_KEY_RIGHT_SHIFT        GLFW_KEY_RIGHT_SHIFT
#define GLENV_KEY_RIGHT_CONTROL      GLFW_KEY_RIGHT_CONTROL
#define GLENV_KEY_RIGHT_ALT          GLFW_KEY_RIGHT_ALT
#define GLENV_KEY_RIGHT_SUPER        GLFW_KEY_RIGHT_SUPER
#define GLENV_KEY_MENU               GLFW_KEY_MENU

#define GLENV_MOUSE_BUTTON_LEFT      GLFW_MOUSE_BUTTON_LEFT
#define GLENV_MOUSE_BUTTON_RIGHT     GLFW_MOUSE_BUTTON_RIGHT
#define GLENV_MOUSE_BUTTON_MIDDLE    GLFW_MOUSE_BUTTON_MIDDLE

#define GLENV_MOD_SHIFT              GLFW_MOD_SHIFT
#define GLENV_MOD_CONTROL            GLFW_MOD_CONTROL
#define GLENV_MOD_ALT                GLFW_MOD_ALT

#endif

/*
 Copyright (c) 2026 Computer Graphics and Visualization Group, University of
 Duisburg-Essen

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 to permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
