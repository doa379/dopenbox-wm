#pragma once

#include <utility>
#include <string_view>
#include <array>
#include <palette.h>
#include <xkb.h>

namespace dobwm {
  static constexpr auto M1              { MOD1_KEY };
  static constexpr auto M4              { MOD4_KEY };
  static constexpr auto M14             { MOD1_KEY | MOD4_KEY };
  static constexpr auto SM4             { SHIFT_KEY | MOD4_KEY };
  static constexpr auto CM4             { CTRL_KEY | MOD4_KEY };
  static constexpr auto SCM4            { SHIFT_KEY | CTRL_KEY | MOD4_KEY };
  // Number of Commands
  //static constexpr auto MAX_NBTN        { 8 };
  static constexpr auto MAX_NCMD        { 64 };
  using Kb = std::pair<unsigned, Key>;
  using Btn = std::pair<unsigned, Button>;
  //using Btns = std::array<Btn, MAX_NBTN>;
  using Cmd = std::pair<Kb, std::string_view>;
  using Cmds = std::array<Cmd, MAX_NCMD>;
}
