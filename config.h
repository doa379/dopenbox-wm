#pragma once

#include <defns.h>

namespace dobwm {
  // Number of Monitors/Displays
  static constexpr auto Nm                { 2 };
  // Number of Tags per Monitor
  static constexpr auto Nt                { 4 };
  static constexpr auto BAR_HEIGHT        { 8 };
  static constexpr auto BRDR_WIDTH        { 4 };
  static constexpr auto ACTBRDR_COLOR     { Palette::Red };
  static constexpr auto INACTBRDR_COLOR   { Palette::Cyan };
  static constexpr auto SELBRDR_COLOR     { Palette::Yellow };
  static constexpr auto GAP               { 0 };
  // Key Bindings
  static constexpr Kb QUIT                { MOD14, Key::Q };
  static constexpr Kb RESTART             { MOD14, Key::R };
  static constexpr Kb UNMAPALL            { MOD14, Key::U };
  static constexpr Kb REMAPALL            { MOD14, Key::V };
  static constexpr Kb KILLCLI             { MOD14, Key::K };
  static constexpr Kb SWCLIFOCUS          { MOD14, Key::Tab };
  static constexpr Kb SELTOGGLE           { MOD14, Key::Space };
  static constexpr Kb SELCLEAR            { MOD14, Key::C };
  // Mouse Bindings
  // Cmd Bindings
  static const Cmds CMDS                  {
      Cmd { MOD14, Key::Esc,   "dmenu_run" },
      Cmd { MOD14, Key::L,     "slock" },
      Cmd { MOD14, Key::Sleep, "slock & yyy M" }
  };
}
