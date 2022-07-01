#pragma once

constexpr auto MOD1_KEY { Mod1Mask };
constexpr auto MOD4_KEY { Mod4Mask };
constexpr auto CTRL_KEY { ControlMask };
constexpr auto SHIFT_KEY { ShiftMask };

enum Xkb {
  _a_         = XK_a,
  _s_         = XK_s,
  _Backspace_ = XK_BackSpace,
  _Tab_       = XK_Tab,
  _Return_    = XK_Return,
  _F1_        = XK_F1,
  _F2_        = XK_F2,
};

