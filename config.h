#pragma once

#include <palette.h>
#include <xkb.h>

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
  static constexpr auto MOD04             { MOD4_KEY };
  static constexpr auto MOD14             { MOD1_KEY | MOD4_KEY };
  static constexpr auto MODS4             { SHIFT_KEY | MOD4_KEY };
  static constexpr auto MODC4             { CTRL_KEY | MOD4_KEY };
  static constexpr auto MODSC4            { SHIFT_KEY | CTRL_KEY | MOD4_KEY };
  static constexpr auto MOD               { MOD14 };
  static constexpr int QUIT_KEY[]         { MOD, _Q_ };
  static constexpr int RESTART_KEY[]      { MOD, _R_ };
  static constexpr int UNMAPALL_KEY[]     { MOD, _U_ };
  static constexpr int REMAPALL_KEY[]     { MOD, _V_ };
  static constexpr int KILLCLI_KEY[]      { MOD, _K_ };
  static constexpr int SWCLIFOCUS_KEY[]   { MOD, _Tab_ };
  static constexpr int SELTOGGLE_KEY[]    { MOD, _S_ }; // Grpset select
  static constexpr int SELCLEAR_KEY[]     { MOD, _C_ }; // Grpset clear
  // Mouse Bindings
  // Cmd Bindings
  /*
  static const char *CMD0[]               { "/usr/local/bin/cmd~0.bin" };
  static const char *CMD1[]               { "/usr/local/bin/cmd~1.bin" };
  static const char *CMD2[]               { "/usr/local/bin/cmd~2.bin" };
  */
}
