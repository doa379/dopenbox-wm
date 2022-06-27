#pragma once

namespace dobwm {
  // Number of Monitors/Displays
  const auto Nm { 2 };
  // Number of Tags per Monitor
  const auto Nt { 4 };
  const auto BAR_HEIGHT { 8 };
  const auto BORDER_WIDTH { 4 };
  const auto BORDER_COLOR0 { 0xD0021B };
  const auto BORDER_COLOR1 { 0xD0021B };
  const auto BORDER_COLOR2 { 0xD0021B };
  const auto CLIENT_HGAP { 0 };
  const auto CLIENT_VGAP { 0 };
  // Key Bindings
  const auto MOD1_KEY { Mod1Mask };
  const auto MOD4_KEY { Mod4Mask };
  const auto CTRL_KEY { ControlMask };
  const auto SHIFT_KEY { ShiftMask };
  const auto RESTART_KEY { MOD1_KEY & MOD4_KEY & 'a' };
  const auto QUIT_KEY { MOD1_KEY & MOD4_KEY & 's' };
  // Mouse Bindings
}
