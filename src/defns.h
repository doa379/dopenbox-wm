#pragma once

#include <utility>
#include <string_view>
#include <vector>
#include <palette.h>
#include <xkb.h>

namespace dobwm {
  static constexpr auto M1              { MOD1_KEY };
  static constexpr auto M4              { MOD4_KEY };
  static constexpr auto M14             { MOD1_KEY | MOD4_KEY };
  static constexpr auto SM4             { SHIFT_KEY | MOD4_KEY };
  static constexpr auto CM4             { CTRL_KEY | MOD4_KEY };
  static constexpr auto SCM4            { SHIFT_KEY | CTRL_KEY | MOD4_KEY };
  using Kb = std::pair<int, Key>;
  using Btn = std::pair<int, Button>;
  using Cmd = std::tuple<int, Key, std::string_view>;
  using Cmds = std::vector<Cmd>;
}
