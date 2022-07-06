#pragma once

//static constexpr auto NUMLOCK_KEY { };
static constexpr auto LOCK_KEY { ~LockMask };
//static constexpr auto LOCK_KEY { ~(numlockmask | LockMask) };
static constexpr auto MOD1_KEY { Mod1Mask & LOCK_KEY };
static constexpr auto MOD4_KEY { Mod4Mask & LOCK_KEY };
static constexpr auto CTRL_KEY { ControlMask & LOCK_KEY };
static constexpr auto SHIFT_KEY { ShiftMask & LOCK_KEY };

enum Xkb {
  _NumLock_   = XK_Num_Lock, 
  _q_         = XK_q,
  _r_         = XK_r,
  _s_         = XK_s,
  _Backspace_ = XK_BackSpace,
  _Tab_       = XK_Tab,
  _Return_    = XK_Return,
  _F1_        = XK_F1,
  _F2_        = XK_F2,
};
