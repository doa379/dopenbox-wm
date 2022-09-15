#pragma once

#include <utility>
#include <string_view>
#include <vector>
#include <palette.h>
#include <xkb.h>

namespace dobwm {
  static constexpr auto MOD01             { MOD1_KEY };
  static constexpr auto MOD04             { MOD4_KEY };
  static constexpr auto MOD14             { MOD1_KEY | MOD4_KEY };
  static constexpr auto MODS4             { SHIFT_KEY | MOD4_KEY };
  static constexpr auto MODC4             { CTRL_KEY | MOD4_KEY };
  static constexpr auto MODSC4            { SHIFT_KEY | CTRL_KEY | MOD4_KEY };
  using Kb = std::pair<int, Key>;
  using Cmd = std::tuple<int, Key, std::string_view>;
  using Cmds = std::vector<Cmd>;
}
