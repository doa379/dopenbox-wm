#pragma once

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
*/

static constexpr char WMNAME[] { "dopenboxwm" };
static constexpr char WMVER[] { "-0.0" };
static constexpr auto NWKS { 8 };
static constexpr auto SLOPPY_FOCUS { true };

enum calls {
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
  MOVEUP,
  MOVEDOWN,
  MOVELEFT,
  MOVERIGHT,
  RESIZEVINC,
  RESIZEVDEC,
  RESIZEHDEC,
  RESIZEHINC,
  QUIT,
  SELECT,
  RESIZE,
  STATE,
};

typedef struct {
  int mod, key;
  union {
    enum calls call;
    const char* cmd;
  };
} input_t;

static const input_t KBD[] {
  { Mod4Mask, XK_0, { WK0 } },
  { Mod4Mask, XK_1, { WK1 } },
  { Mod4Mask, XK_2, { WK2 } },
  { Mod4Mask, XK_3, { WK3 } },
  { Mod4Mask, XK_4, { WK4 } },
  { Mod4Mask, XK_5, { WK5 } },
  { Mod4Mask, XK_6, { WK6 } },
  { Mod4Mask, XK_7, { WK7 } },
  { Mod4Mask, XK_8, { WK8 } },
  { Mod4Mask, XK_9, { WK9 } },
  { Mod4Mask, XK_u, { UNMAPALL } },
  { Mod4Mask, XK_v, { REMAPALL } },
  { Mod4Mask, XK_Tab, { SWFOCUS } },
  { Mod4Mask, XK_m, { TOGGLEMODE } },
  { Mod4Mask, XK_o, { PREVCLI } },
  { Mod4Mask, XK_p, { NEXTCLI } },
  { Mod4Mask, XK_space, { SELTOGGLE } },
  { Mod4Mask, XK_c, { SELCLEAR } },
  { Mod4Mask | ShiftMask, XK_1, { MON1 } },
  { Mod4Mask | ShiftMask, XK_2, { MON2 } },
  { Mod4Mask | ShiftMask, XK_3, { MON3 } },
  { Mod4Mask | ShiftMask, XK_4, { MON4 } },
  { Mod4Mask | ShiftMask, XK_5, { MON5 } },
  { Mod4Mask | ShiftMask, XK_6, { MON6 } },
  { Mod4Mask | ShiftMask, XK_7, { MON7 } },
  { Mod4Mask | ShiftMask, XK_8, { MON8 } },
  { Mod4Mask | ShiftMask, XK_9, { MON9 } },
  { Mod4Mask | ShiftMask, XK_0, { MON0 } },
  { Mod4Mask | ShiftMask, XK_k, { KILL } },
  { Mod4Mask | ShiftMask, XK_Up, { MOVEUP } },
  { Mod4Mask | ShiftMask, XK_Down, { MOVEDOWN } },
  { Mod4Mask | ShiftMask, XK_Left, { MOVELEFT } },
  { Mod4Mask | ShiftMask, XK_Right, { MOVERIGHT } },
  { Mod4Mask | ControlMask, XK_Up, { RESIZEVINC } },
  { Mod4Mask | ControlMask, XK_Down, { RESIZEVDEC } },
  { Mod4Mask | ControlMask, XK_Left, { RESIZEHDEC } },
  { Mod4Mask | ControlMask, XK_Right, { RESIZEHINC } },
  { Mod4Mask | ShiftMask | ControlMask, XK_q, { QUIT } },
  // Shell Bindings
  { Mod4Mask, XK_n, { .cmd = "notify-send \"Test Key\"" } },
  { Mod4Mask, XK_Escape, { .cmd = "dmenu_run" } },
  { Mod4Mask, XK_l, { .cmd = "slock" } },
  { Mod4Mask, XF86XK_Sleep, { .cmd = "slock & yyy M" } },
  { Mod4Mask, XK_c, { .cmd = "xconsole" } },
  { Mod4Mask, XK_d, { .cmd = "xclock" } },
  { Mod4Mask, XK_Return, { .cmd = "xterm" } },
};

static const input_t BTN[] {
  // Mouse Bindings
  { 0, Button1, { SELECT } },
  { Mod4Mask, Button3, { RESIZE } },
};
