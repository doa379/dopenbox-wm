#pragma once

#include <X11/XF86keysym.h>
#include <palette.h>
/*
enum class Key : ::KeySym {
  Esc       = XK_Escape,
  F1        = XK_F1,
  F2        = XK_F2,
  F3        = XK_F3,
  F4        = XK_F4,
  F5        = XK_F5,
  F6        = XK_F6,
  F7        = XK_F7,
  F8        = XK_F8,
  F9        = XK_F9,
  F10       = XK_F10,
  F11       = XK_F11,
  F12       = XK_F12,
  Gr        = XK_grave,
  N1        = XK_1,
  N2        = XK_2,
  N3        = XK_3,
  N4        = XK_4,
  N5        = XK_5,
  N6        = XK_6,
  N7        = XK_7,
  N8        = XK_8,
  N9        = XK_9,
  N0        = XK_0,
  Minus     = XK_minus,
  Equal     = XK_equal,
  Backspace = XK_BackSpace,
  Tab       = XK_Tab,
  Q         = XK_q,
  W         = XK_w,
  E         = XK_e,
  R         = XK_r,
  T         = XK_t,
  Y         = XK_y,
  U         = XK_u,
  I         = XK_t,
  O         = XK_o,
  P         = XK_p,
  Bleft     = XK_bracketleft,
  Bright    = XK_bracketright,
  A         = XK_a,
  S         = XK_s,
  D         = XK_d,
  F         = XK_f,
  G         = XK_g,
  H         = XK_h,
  J         = XK_j,
  K         = XK_k,
  L         = XK_l,
  Z         = XK_z,
  X         = XK_x,
  C         = XK_c,
  V         = XK_v,
  B         = XK_b,
  N         = XK_n,
  M         = XK_m,
  Comma     = XK_comma,
  Period    = XK_period,
  Space     = XK_space,
  Left      = XK_Left,
  Up        = XK_Up,
  Down      = XK_Down,
  Right     = XK_Right,
  Return    = XK_Return,
  Sleep     = XF86XK_Sleep,
};

enum class Button : unsigned {
  Left      = Button1,
  Middle    = Button2,
  Right     = Button3,
  Up        = Button4,
  Down      = Button5,
};
*/

namespace dobwm {
  static constexpr auto MOD1_KEY { Mod1Mask };
  static constexpr auto MOD4_KEY { Mod4Mask };
  static constexpr auto CTRL_KEY { ControlMask };
  static constexpr auto SHIFT_KEY { ShiftMask };

  static constexpr auto M1              { MOD1_KEY };
  static constexpr auto M4              { MOD4_KEY };
  static constexpr auto M14             { MOD1_KEY | MOD4_KEY };
  static constexpr auto SM4             { SHIFT_KEY | MOD4_KEY };
  static constexpr auto CM4             { CTRL_KEY | MOD4_KEY };
  static constexpr auto SCM4            { SHIFT_KEY | CTRL_KEY | MOD4_KEY };
  // Number of Commands
  //static constexpr auto MAX_NBTN        { 8 };
  static constexpr auto MAX_NCMD        { 64 };
  using Kb = std::pair<unsigned, Key>;
  using Btn = std::pair<unsigned, Button>;
  //using Btns = std::array<Btn, MAX_NBTN>;
  using Cmd = std::pair<Kb, std::string_view>;
  using Cmds = std::array<Cmd, MAX_NCMD>;


  // Number of Tags per Monitor
  static constexpr auto NT                { 4 };
  static constexpr auto BAR_HEIGHT        { 8 };
  static constexpr auto BDR_WIDTH         { 4 };
  static constexpr auto ACTBDR_COLOR      { Palette::Red };
  static constexpr auto INACTBDR_COLOR    { Palette::Cyan };
  static constexpr auto SELBDR_COLOR      { Palette::Yellow };
  static constexpr auto WINDOW_GAP        { 0 };
  static constexpr auto SLOPPY_FOCUS      { false };
  static constexpr auto MOVESTEP_PX       { 5 };
  static constexpr auto MODKEY            { M14 };
  // Cmd Bindings
  static constexpr Cmds CMDS {
      // Int Commands
      Cmd { { MODKEY, Key::Q },     "QUIT" },
      Cmd { { MODKEY, Key::U },     "UNMAPALL" },
      Cmd { { MODKEY, Key::V },     "REMAPALL" },
      Cmd { { MODKEY, Key::K },     "KILLCLI" },
      Cmd { { MODKEY, Key::Tab },   "SWFOCUS" },
      Cmd { { MODKEY, Key::Space }, "SELTOGGLE" },
      Cmd { { MODKEY, Key::C },     "SELCLEAR" },
      Cmd { { SM4, Key::Up },       "MOVEUP" },
      Cmd { { SM4, Key::Down },     "MOVEDOWN" },
      Cmd { { SM4, Key::Left },     "MOVELEFT" },
      Cmd { { SM4, Key::Right },    "MOVERIGHT" },
      Cmd { { CM4, Key::Up },       "RESIZEUP" },
      Cmd { { CM4, Key::Down },     "RESIZEDOWN" },
      Cmd { { CM4, Key::Left },     "RESIZELEFT" },
      Cmd { { CM4, Key::Right },    "RESIZERIGHT" },
      // Sys Commands
      Cmd { { MODKEY, Key::Esc },   "dmenu_run" },
      Cmd { { MODKEY, Key::L },     "slock" },
      Cmd { { MODKEY, Key::Sleep }, "slock & yyy M" },
      Cmd { { { }, Key::N },        "notify-send \"No Modkey\"" }
  };
  static constexpr Cmds CMDS_ASYNC {
      Cmd { { MODKEY, Key::D }, "xclock" },
      Cmd { { MODKEY, Key::Return }, "xterm" }
  };
  // Mouse Bindings
  /*
  static constexpr Btn SELECT       { { }, Button::Left };
  static constexpr Btn RESIZE       { MODKEY, Button::Right };
  */
  /*
  static constexpr Btns BTNS {
      Btn { { { }, Button::Left },      "SELECT" },
      Btn { { MODKEY, Button::Right },  "RESIZE" }
  };
  */
}
