#pragma once

namespace dobwm {
  // Number of Monitors/Displays
  const auto Nm { 2 };
  // Number of Tags per Monitor
  const auto Nt { 4 };
  const auto BAR_HEIGHT { 8 };
  const auto BORDER_WIDTH { 2 };
  const auto BORDER_COLOR { 0xD0021B };
  const auto BG_COLOR { 0x4A90E2 };
  const auto CLIENT_HGAP { 0 };
  const auto CLIENT_VGAP { 0 };
  const auto MOD0_KEY { Mod4Mask };
  const auto MOD1_KEY { Mod4Mask };
  const auto MOD2_KEY { Mod4Mask };
  const auto RESTART_KEY { MOD0_KEY & 'r' };
  const auto QUIT_KEY { MOD0_KEY & 'q' };
}
