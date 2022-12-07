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
  // Key Bindings
  static constexpr Kb QUIT                { MODKEY, Key::Q };
  static constexpr Kb UNMAPALL            { MODKEY, Key::U };
  static constexpr Kb REMAPALL            { MODKEY, Key::V };
  static constexpr Kb KILLCLI             { MODKEY, Key::K };
  static constexpr Kb SWCLIFOCUS          { MODKEY, Key::Tab };
  static constexpr Kb SELTOGGLE           { MODKEY, Key::Space };
  static constexpr Kb SELCLEAR            { MODKEY, Key::C };
  static constexpr Kb MOVEUP              { SM4, Key::Up };
  static constexpr Kb MOVEDOWN            { SM4, Key::Down };
  static constexpr Kb MOVELEFT            { SM4, Key::Left };
  static constexpr Kb MOVERIGHT           { SM4, Key::Right };
  static constexpr Kb RESIZEUP            { CM4, Key::Up };
  static constexpr Kb RESIZEDOWN          { CM4, Key::Down };
  static constexpr Kb RESIZELEFT          { CM4, Key::Left };
  static constexpr Kb RESIZERIGHT         { CM4, Key::Right };
  // Cmd Bindings
  static const Cmds CMDS                  {
      Cmd { { MODKEY, Key::Esc },   "dmenu_run" },
      Cmd { { MODKEY, Key::L },     "slock" },
      Cmd { { MODKEY, Key::Sleep }, "slock & yyy M" },
      Cmd { { { }, Key::N }, "notify-send \"No Modkey\"" }
  };
  // Mouse Bindings
  static constexpr Btn SELECT             { { }, Button::Left };
  static constexpr Btn RESIZE             { MODKEY, Button::Right };
}
