#pragma once

#include <defns.h>

namespace dobwm {
  // Number of Tags per Monitor
  static constexpr auto Nt                { 4 };
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
  static constexpr Cmds CMDS                  {
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
  static constexpr Cmds CMDS_ASYNC            {
      Cmd { { MODKEY, Key::Return }, "xterm" }
  };
  // Mouse Bindings
  static constexpr Btn SELECT       { { }, Button::Left };
  static constexpr Btn RESIZE       { MODKEY, Button::Right };
}
