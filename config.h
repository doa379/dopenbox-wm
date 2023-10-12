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
  static constexpr auto MODKEY            { Mod4Mask };

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
  
  static const std::map<std::size_t, std::variant<Calls, std::string_view>> CMDS {
    { MODKEY | XK_u, Calls::UNMAPALL },
    { MODKEY | XK_v, Calls::REMAPALL },
    { MODKEY | XK_Tab, Calls::SWFOCUS },
    { MODKEY | XK_o, Calls::PREVCLI },
    { MODKEY | XK_p, Calls::NEXTCLI },
    { MODKEY | XK_space, Calls::SELTOGGLE },
    { MODKEY | XK_c, Calls::SELCLEAR },
    // Fix this ShiftMask
    { MODKEY | ShiftMask | XK_w, Calls::QUIT },
    { MODKEY | ShiftMask | XK_k, Calls::KILL },
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
    { MODKEY | XK_c, "xconsole" },
    { MODKEY | XK_d, "xclock" },
    { MODKEY | XK_Return, "xterm" }
  };
}
