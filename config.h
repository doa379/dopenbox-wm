#pragma once

#include <X11/XF86keysym.h>
#include <palette.h>
#include <variant>
#include <map>

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
/*
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
*/
  // Number of Commands
  //static constexpr auto MAX_NBTN        { 8 };
  //using Kb = std::pair<unsigned, ::KeySym>;
  //using Btn = std::pair<unsigned, Button>;
  //using Btns = std::array<Btn, MAX_NBTN>;
  //using Cmd = std::pair<Key, std::string_view>;
  //using Cmd = std::tuple<int, ::KeySym, std::string_view>;
  //using MCmd = std::tuple<int, int, std::string_view>;


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
  static constexpr auto MODKEY            { Mod4Mask };

  enum class Calls : std::size_t {
    QUIT,
    UNMAPALL,
    REMAPALL,
    KILL,
    SWFOCUS,
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
    /*
    QUIT = MODKEY | XK_q,
    UNMAPALL = MODKEY | XK_u,
    REMAPALL = MODKEY | XK_v,
    KILL = MODKEY | XK_k,
    SWFOCUS = MODKEY | XK_Tab,
    SELTOGGLE = MODKEY | XK_space,
    SELCLEAR = MODKEY | XK_c,
    MOVEUP = MODKEY | ShiftMask | XK_Up,
    MOVEDOWN = MODKEY | ShiftMask | XK_Down,
    MOVELEFT = MODKEY | ShiftMask | XK_Left,
    MOVERIGHT = MODKEY | ShiftMask | XK_Right,
    RESIZEVINC = MODKEY | ControlMask | XK_Up,
    RESIZEVDEC = MODKEY | ControlMask | XK_Down,
    RESIZEHDEC = MODKEY | ControlMask | XK_Left,
    RESIZEHINC = MODKEY | ControlMask | XK_Right,
    SELECT = Button1,
    RESIZE = MODKEY | Button3,
    Z = XK_VoidSymbol // Terminator
    */
  };
  /*
  // Internal Cmds
  using Cmd = std::tuple<unsigned, unsigned, std::variant<Calls, std::string_view>>;
  static constexpr auto NC { 24 };
  using Cmds = std::array<Cmd, NC>;
  // Cmd Bindings
  static constexpr Cmds CMDS {
    // KB Bindings
    Cmd { Mod4Mask, XK_q, Calls::QUIT },
    Cmd { Mod4Mask, XK_u, Calls::UNMAPALL },
    Cmd { Mod4Mask, XK_v, Calls::REMAPALL },
    Cmd { Mod4Mask, XK_k, Calls::KILL },
    Cmd { Mod4Mask, XK_Tab, Calls::SWFOCUS },
    Cmd { Mod4Mask, XK_space, Calls::SELTOGGLE },
    Cmd { Mod4Mask, XK_c, Calls::SELCLEAR },
    Cmd { Mod4Mask | ShiftMask, XK_Up, Calls::MOVEUP },
    Cmd { Mod4Mask | ShiftMask, XK_Down, Calls::MOVEDOWN },
    Cmd { Mod4Mask | ShiftMask, XK_Left, Calls::MOVELEFT },
    Cmd { Mod4Mask | ShiftMask, XK_Right, Calls::MOVERIGHT },
    Cmd { Mod4Mask | ControlMask, XK_Up, Calls::RESIZEVINC },
    Cmd { Mod4Mask | ControlMask, XK_Down, Calls::RESIZEVDEC },
    Cmd { Mod4Mask | ControlMask, XK_Left, Calls::RESIZEHDEC },
    Cmd { Mod4Mask | ControlMask, XK_Right, Calls::RESIZEHINC },
    // Mouse Bindings
    Cmd { { }, Button1, Calls::SELECT },
    Cmd { Mod4Mask, Button3, Calls::RESIZE },
  // User definable Cmds
  };

  static constexpr Cmds USERCMDS {
    Cmd { { }, XK_n, "notify-send \"No Modkey\"" },
    Cmd { Mod4Mask, XK_Escape, "dmenu_run" },
    Cmd { Mod4Mask, XK_l, "slock" },
    Cmd { Mod4Mask, XF86XK_Sleep, "slock & yyy M" },
    Cmd { Mod4Mask, XK_d, "xclock" },
    Cmd { Mod4Mask, XK_Return, "xterm" }
  };
  */
  static const std::map<std::size_t, std::variant<Calls, std::string_view>> CMDS {
    { MODKEY | XK_q, Calls::QUIT },
    { MODKEY | XK_u, Calls::UNMAPALL },
    { MODKEY | XK_v, Calls::REMAPALL },
    { MODKEY | XK_k, Calls::KILL },
    { MODKEY | XK_Tab, Calls::SWFOCUS },
    { MODKEY | XK_space, Calls::SELTOGGLE },
    { MODKEY | XK_c, Calls::SELCLEAR },
    { MODKEY | ShiftMask | XK_Up, Calls::MOVEUP },
    { MODKEY | ShiftMask | XK_Down, Calls::MOVEDOWN },
    { MODKEY | ShiftMask | XK_Left, Calls::MOVELEFT },
    { MODKEY | ShiftMask | XK_Right, Calls::MOVERIGHT },
    { MODKEY | ControlMask | XK_Up, Calls::RESIZEVINC },
    { MODKEY | ControlMask | XK_Down, Calls::RESIZEVDEC },
    { MODKEY | ControlMask | XK_Left, Calls::RESIZEHDEC },
    { MODKEY | ControlMask | XK_Right, Calls::RESIZEHINC },
    // Mouse Bindings
    { Button1, Calls::SELECT },
    { MODKEY | Button3, Calls::RESIZE },
    // Shell Bindings
    { MODKEY | XK_n, "notify-send \"Test Key\"" },
    { MODKEY | XK_Escape, "dmenu_run" },
    { MODKEY | XK_l, "slock" },
    { MODKEY | XF86XK_Sleep, "slock & yyy M" },
    { MODKEY | XK_d, "xclock" },
    { MODKEY | XK_Return, "xterm" }
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
