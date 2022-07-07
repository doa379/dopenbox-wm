#pragma once

//static constexpr auto NUMLOCK_KEY { };
static constexpr auto LOCK_KEY { ~LockMask };
//static constexpr auto LOCK_KEY { ~(numlockmask | LockMask) };
static constexpr auto MOD1_KEY { Mod1Mask & LOCK_KEY };
static constexpr auto MOD4_KEY { Mod4Mask & LOCK_KEY };
static constexpr auto CTRL_KEY { ControlMask & LOCK_KEY };
static constexpr auto SHIFT_KEY { ShiftMask & LOCK_KEY };

enum Xkb {
  _F1_        = XK_F1,
  _F2_        = XK_F2,
  _F3_        = XK_F3,
  _F4_        = XK_F4,
  _F5_        = XK_F5,
  _F6_        = XK_F6,
  _F7_        = XK_F7,
  _F8_        = XK_F8,
  _F9_        = XK_F9,
  _F10_       = XK_F10,
  _F11_       = XK_F11,
  _F12_       = XK_F12,
  _Numlock_   = XK_Num_Lock, 
	_Gr_        = XK_grave,
  _1_         = XK_1,
  _2_         = XK_2,
  _3_         = XK_3,
  _4_         = XK_4,
  _5_         = XK_5,
  _6_         = XK_6,
  _7_         = XK_7,
  _8_         = XK_8,
  _9_         = XK_9,
  _0_         = XK_0,
	_Minus_     = XK_minus,
	_Equal_     = XK_equal,
  _Backspace_ = XK_BackSpace,
  _Tab_       = XK_Tab,
  _Bleft_     = XK_bracketleft,
	_Bright_    = XK_bracketright,
  _Q_         = XK_q,
  _R_         = XK_r,
  _S_         = XK_s,
	_Comma_     = XK_comma,
	_Period_    = XK_period,
  _Return_    = XK_Return,
};
