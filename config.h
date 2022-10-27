#pragma once

#include <defns.h>

namespace dobwm {
  // Number of Monitors/Displays
  static constexpr auto Nm                { 2 };
  // Number of Tags per Monitor
  static constexpr auto Nt                { 4 };
  static constexpr auto BAR_HEIGHT        { 8 };
  static constexpr auto BDR_WIDTH         { 4 };
  static constexpr auto ACTBDR_COLOR      { Palette::Red };
  static constexpr auto INACTBDR_COLOR    { Palette::Cyan };
  static constexpr auto SELBDR_COLOR      { Palette::Yellow };
  static constexpr auto WINDOW_GAP        { 0 };
  static constexpr auto SLOPPY_FOCUS      { true };
  // Key Bindings
  static constexpr Kb QUIT                { MOD14, Key::Q };
  static constexpr Kb RESTART             { MOD14, Key::R };
  static constexpr Kb UNMAPALL            { MOD14, Key::U };
  static constexpr Kb REMAPALL            { MOD14, Key::V };
  static constexpr Kb KILLCLI             { MOD14, Key::K };
  static constexpr Kb SWCLIFOCUS          { MOD14, Key::Tab };
  static constexpr Kb SELTOGGLE           { MOD14, Key::Space };
  static constexpr Kb SELCLEAR            { MOD14, Key::C };
  static constexpr Kb MOVEUP              { MODS4, Key::Up };
  static constexpr Kb MOVEDOWN            { MODS4, Key::Down };
  static constexpr Kb MOVELEFT            { MODS4, Key::Left };
  static constexpr Kb MOVERIGHT           { MODS4, Key::Right };
  static constexpr Kb RESIZEUP            { MODC4, Key::Up };
  static constexpr Kb RESIZEDOWN          { MODC4, Key::Down };
  static constexpr Kb RESIZELEFT          { MODC4, Key::Left };
  static constexpr Kb RESIZERIGHT         { MODC4, Key::Right };
  // Mouse Bindings
  // Cmd Bindings
  static const Cmds CMDS                  {
      Cmd { MOD14, Key::Esc,   "dmenu_run" },
      Cmd { MOD14, Key::L,     "slock" },
      Cmd { MOD14, Key::Sleep, "slock & yyy M" },
      Cmd { 0, Key::N, "notify-send \"No Modkey\"" }
  };
}
