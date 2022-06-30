#pragma once

constexpr auto MOD1_KEY { Mod1Mask };
constexpr auto MOD4_KEY { Mod4Mask };
constexpr auto CTRL_KEY { ControlMask };
constexpr auto SHIFT_KEY { ShiftMask };

enum class Xkb {
  a           = XK_a,
  s           = XK_s,
  Backspace   = XK_BackSpace,
  Tab         = XK_Tab,
  Return      = XK_Return,
  F1          = XK_F1,
  F2          = XK_F2,
};

