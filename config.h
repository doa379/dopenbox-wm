#pragma once

#include <variant>
#include <X11/Xutil.h>
#include <X11/XF86keysym.h>
#include <palette.h>

/*
KeySym {
  XK_Escape,
  XK_F1,
  XK_F2,
  XK_F3,
  XK_F4,
  XK_F5,
  XK_F6,
  XK_F7,
  XK_F8,
  XK_F9,
  XK_F10,
  XK_F11,
  XK_F12,
  XK_grave,
  XK_1,
  XK_2,
  XK_3,
  XK_4,
  XK_5,
  XK_6,
  XK_7,
  XK_8,
  XK_9,
  XK_0,
  XK_minus,
  XK_equal,
  XK_BackSpace,
  XK_Tab,
  XK_q,
  XK_w,
  XK_e,
  XK_r,
  XK_t,
  XK_y,
  XK_u,
  XK_t,
  XK_o,
  XK_p,
  XK_bracketleft,
  XK_bracketright,
  XK_a,
  XK_s,
  XK_d,
  XK_f,
  XK_g,
  XK_h,
  XK_j,
  XK_k,
  XK_l,
  XK_z,
  XK_x,
  XK_c,
  XK_v,
  XK_b,
  XK_n,
  XK_m,
  XK_comma,
  XK_period,
  XK_space,
  XK_Left,
  XK_Up,
  XK_Down,
  XK_Right,
  XK_Return,
  XF86XK_Sleep,
}

Buttons {
  Left      = Button1,
  Middle    = Button2,
  Right     = Button3,
  Up        = Button4,
  Down      = Button5,
}

Modifiers {
  Mod1Mask,
  Mod2Mask,
  Mod3Mask,
  Mod4Mask,
  Mod5Mask,
  ShiftMask,
  ControlMask,
  LockMask
}
*/

namespace wmconf {
  static char constexpr WMNAME[] { "dopenboxwm" };
  static auto constexpr NWKS { 8 };
  static auto constexpr SLOPPY_FOCUS { true };
  static auto constexpr BDRPX { 2 };
  // Color Scheme: COLORS[] { BG, Selected BG, FG }
  static std::size_t constexpr COLORS[] { Cyan10, Cyan70, Gray10 };

  enum class Calls : std::size_t {
    // Declare pool of calls
    WK0,
    WK1,
    WK2,
    WK3,
    WK4,
    WK5,
    WK6,
    WK7,
    WK8,
    WK9,
    MON0,
    MON1,
    MON2,
    MON3,
    MON4,
    MON5,
    MON6,
    MON7,
    MON8,
    MON9,
    UNMAPALL,
    REMAPALL,
    KILL,
    SWFOCUS,
    TOGGLEMODE,
    PREVCLI,
    NEXTCLI,
    SELTOGGLE,
    SELCLEAR,
    QUIT,
    SELECT,
    RESIZE,
    STATE,
  };

  struct Input {
    int mod;
    int sym;
    std::variant<Calls, char const*> var;
  };

  static Input const KBD[] {
    { Mod4Mask, XK_0, Calls::WK0 },
    { Mod4Mask, XK_1, Calls::WK1 },
    { Mod4Mask, XK_2, Calls::WK2 },
    { Mod4Mask, XK_3, Calls::WK3 },
    { Mod4Mask, XK_4, Calls::WK4 },
    { Mod4Mask, XK_5, Calls::WK5 },
    { Mod4Mask, XK_6, Calls::WK6 },
    { Mod4Mask, XK_7, Calls::WK7 },
    { Mod4Mask, XK_8, Calls::WK8 },
    { Mod4Mask, XK_9, Calls::WK9 },
    { Mod4Mask, XK_u, Calls::UNMAPALL },
    { Mod4Mask, XK_v, Calls::REMAPALL },
    { Mod4Mask, XK_Tab, Calls::SWFOCUS },
    { Mod4Mask, XK_m, Calls::TOGGLEMODE },
    { Mod4Mask, XK_o, Calls::PREVCLI },
    { Mod4Mask, XK_p, Calls::NEXTCLI },
    { Mod4Mask, XK_space, Calls::SELTOGGLE },
    { Mod4Mask, XK_c, Calls::SELCLEAR },
    { Mod4Mask | ShiftMask, XK_1, Calls::MON1 },
    { Mod4Mask | ShiftMask, XK_2, Calls::MON2 },
    { Mod4Mask | ShiftMask, XK_3, Calls::MON3 },
    { Mod4Mask | ShiftMask, XK_4, Calls::MON4 },
    { Mod4Mask | ShiftMask, XK_5, Calls::MON5 },
    { Mod4Mask | ShiftMask, XK_6, Calls::MON6 },
    { Mod4Mask | ShiftMask, XK_7, Calls::MON7 },
    { Mod4Mask | ShiftMask, XK_8, Calls::MON8 },
    { Mod4Mask | ShiftMask, XK_9, Calls::MON9 },
    { Mod4Mask | ShiftMask, XK_0, Calls::MON0 },
    { Mod4Mask | ShiftMask, XK_k, Calls::KILL },
    { Mod4Mask | ShiftMask | ControlMask, XK_q, Calls::QUIT },
    // Shell Bindings
    { Mod4Mask, XK_n, "notify-send \"Test Key\"" },
    { Mod4Mask, XK_Escape, "dmenu_run" },
    { Mod4Mask, XK_l, "slock" },
    { Mod4Mask, XF86XK_Sleep, "slock & yyy M" },
    { Mod4Mask, XK_c, "xconsole" },
    { Mod4Mask, XK_d, "xclock" },
    { Mod4Mask, XK_Return, "xterm" },
  };

  static Input const BTN[] {
    // Mouse Bindings
    { 0, Button1, Calls::SELECT },
    { Mod4Mask, Button3, Calls::RESIZE },
  };
}
