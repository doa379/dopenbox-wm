#pragma once

#include <palette.h>
#include <xkb.h>

namespace dobwm {
  // Number of Monitors/Displays
  const auto Nm { 2 };
  // Number of Tags per Monitor
  const auto Nt { 4 };
  const auto BAR_HEIGHT { 8 };
  const auto BORDER_WIDTH { 4 };
  const auto BORDER_COLOR0 { Palette::Red };
  const auto BORDER_COLOR1 { Palette::Red };
  const auto BORDER_COLOR2 { Palette::Red };
  const auto CLIENT_HGAP { 0 };
  const auto CLIENT_VGAP { 0 };
  // Key Bindings
  const auto RESTART_KEY { MOD1_KEY & MOD4_KEY & 'a' };
  const auto QUIT_KEY { MOD1_KEY & MOD4_KEY & 's' };
  // Mouse Bindings
}
