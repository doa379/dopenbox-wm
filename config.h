#pragma once

#include <X11/XF86keysym.h>
#include <palette.h>
#include <variant>
#include <array>
#include <tuple>

/*
Some ::KeySym {
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

Some Buttons {
  Left      = Button1,
  Middle    = Button2,
  Right     = Button3,
  Up        = Button4,
  Down      = Button5,
}
*/

namespace dobwm {
  // Number of Tags per Monitor
  static constexpr auto NT                { 4 };
  static constexpr auto BAR_HEIGHT        { 8 };
  static constexpr auto BDR_WIDTH         { 4 };
  static constexpr auto ACTBDR_COLOR      { Palette::Red };
  static constexpr auto INACTBDR_COLOR    { Palette::Cyan };
  static constexpr auto SELBDR_COLOR      { Palette::Yellow };
  static constexpr auto WINDOW_GAP        { 0 };
  static constexpr auto SLOPPY_FOCUS      { true };
  static constexpr auto MOVESTEP_PX       { 5 };

  enum class Calls : std::size_t {
    // Declare pool of calls
    QUIT,
    UNMAPALL,
    REMAPALL,
    KILL,
    SWFOCUS,
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
    SELECT,
    RESIZE,
    Z // Terminator
  };
  
  using Input = std::tuple<std::size_t, std::size_t, std::variant<Calls, std::string_view>>;
  
  static constexpr std::array<Input, 32> KEYS {
    Input { Mod4Mask, XK_u, Calls::UNMAPALL },
      { Mod4Mask, XK_v, Calls::REMAPALL },
      { Mod4Mask, XK_Tab, Calls::SWFOCUS },
      { Mod4Mask, XK_o, Calls::PREVCLI },
      { Mod4Mask, XK_p, Calls::NEXTCLI },
      { Mod4Mask, XK_space, Calls::SELTOGGLE },
      { Mod4Mask, XK_c, Calls::SELCLEAR },
      { Mod4Mask | ShiftMask, XK_k, Calls::KILL },
      { Mod4Mask | ShiftMask, XK_Up, Calls::MOVEUP },
      { Mod4Mask | ShiftMask, XK_Down, Calls::MOVEDOWN },
      { Mod4Mask | ShiftMask, XK_Left, Calls::MOVELEFT },
      { Mod4Mask | ShiftMask, XK_Right, Calls::MOVERIGHT },
      { Mod4Mask | ControlMask, XK_Up, Calls::RESIZEVINC },
      { Mod4Mask | ControlMask, XK_Down, Calls::RESIZEVDEC },
      { Mod4Mask | ControlMask, XK_Left, Calls::RESIZEHDEC },
      { Mod4Mask | ControlMask, XK_Right, Calls::RESIZEHINC },
      { Mod4Mask | ShiftMask | ControlMask, XK_q, Calls::QUIT },
      // Shell Bindings
      { Mod4Mask, XK_n, "notify-send \"Test Key\"" },
      { Mod4Mask, XK_Escape, "dmenu_run" },
      { Mod4Mask, XK_l, "slock" },
      { Mod4Mask, XF86XK_Sleep, "slock & yyy M" },
      { Mod4Mask, XK_c, "xconsole" },
      { Mod4Mask, XK_d, "xclock" },
      { Mod4Mask, XK_Return, "xterm" }
  };
  
  static constexpr std::array<Input, 8> BTNS {
    // Mouse Bindings
    Input { 0, Button1, Calls::SELECT },
      { Mod4Mask, Button3, Calls::RESIZE },
  };
}
