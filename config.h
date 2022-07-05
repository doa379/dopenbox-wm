#pragma once

#include <palette.h>
#include <xkb.h>

namespace dobwm {
  // Number of Monitors/Displays
  static const auto Nm { 2 };
  // Number of Tags per Monitor
  static const auto Nt { 4 };
  static const auto BAR_HEIGHT { 8 };
  static const auto BORDER_WIDTH { 4 };
  static const auto BORDER_COLOR0 { Palette::Red };
  static const auto BORDER_COLOR1 { Palette::Red };
  static const auto BORDER_COLOR2 { Palette::Red };
  static const auto HGAPS { 0 };
  static const auto VGAPS { 0 };
  // Key Bindings
  static const auto MOD04 { MOD4_KEY };
  static const auto MOD14 { MOD1_KEY & MOD4_KEY };
  static const auto MODS4 { SHIFT_KEY & MOD4_KEY };
  static const auto MODC4 { CTRL_KEY & MOD4_KEY };
  static const auto MODSC4 { SHIFT_KEY & CTRL_KEY & MOD4_KEY };
  static const auto RESTART_KEY { std::pair(MOD14, _a_) };
  static const auto QUIT_KEY { std::pair(MOD14, _s_) };
  static const auto SOME_KEY { std::pair(MOD04, _a_ & _s_) };
  // Mouse Bindings
}
