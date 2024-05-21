#include <iostream>
#include <csignal>
#include <utility>
#include <algorithm>
#include "wm.h"
#include "../config.h"

some::Draw::Draw() : 
  rootw { xlib.root() }, 
  font { draw.load_font(wmconf::FONT) } {

}

some::Draw::~Draw() {

}

some::Wm::Wm() : rootw { xlib.root() } {
  xlib::DefaultXError error; 
  input.select(rootw, xlib::ROOTMASK);
  if (error.get())
    throw std::runtime_error("Initialization error (another wm running?)");

  input.ungrab_allkey(rootw);
  auto const MASK { input.modmask() };
  for (auto const& K : wmconf::KBD) {
    auto const KCODE { input.keysym_keycode(K.sym) };
    KCODE_KSYM[KCODE] = K.sym;
    input.grab_key(rootw, K.mod & MASK, KCODE);
  }
  
  CALL[std::to_underlying(wmconf::Calls::WK0)] = [&] { sw_wk(0); };
  CALL[std::to_underlying(wmconf::Calls::WK1)] = [&] { sw_wk(1); };
  CALL[std::to_underlying(wmconf::Calls::WK2)] = [&] { sw_wk(2); };
  CALL[std::to_underlying(wmconf::Calls::WK3)] = [&] { sw_wk(3); };
  CALL[std::to_underlying(wmconf::Calls::WK4)] = [&] { sw_wk(4); };
  CALL[std::to_underlying(wmconf::Calls::WK5)] = [&] { sw_wk(5); };
  CALL[std::to_underlying(wmconf::Calls::WK6)] = [&] { sw_wk(6); };
  CALL[std::to_underlying(wmconf::Calls::WK7)] = [&] { sw_wk(7); };
  CALL[std::to_underlying(wmconf::Calls::WK8)] = [&] { sw_wk(8); };
  CALL[std::to_underlying(wmconf::Calls::WK9)] = [&] { sw_wk(9); };
  CALL[std::to_underlying(wmconf::Calls::LOADWK)] = [&] { load_wk(); };
  CALL[std::to_underlying(wmconf::Calls::UNLOADWK)] = [&] { unload_wk(); };
  CALL[std::to_underlying(wmconf::Calls::MON0)] = [&] { sw_mon(0); };
  CALL[std::to_underlying(wmconf::Calls::MON1)] = [&] { sw_mon(1); };
  CALL[std::to_underlying(wmconf::Calls::MON2)] = [&] { sw_mon(2); };
  CALL[std::to_underlying(wmconf::Calls::MON3)] = [&] { sw_mon(3); };
  CALL[std::to_underlying(wmconf::Calls::MON4)] = [&] { sw_mon(4); };
  CALL[std::to_underlying(wmconf::Calls::MON5)] = [&] { sw_mon(5); };
  CALL[std::to_underlying(wmconf::Calls::MON6)] = [&] { sw_mon(6); };
  CALL[std::to_underlying(wmconf::Calls::MON7)] = [&] { sw_mon(7); };
  CALL[std::to_underlying(wmconf::Calls::MON8)] = [&] { sw_mon(8); };
  CALL[std::to_underlying(wmconf::Calls::MON9)] = [&] { sw_mon(9); };
  CALL[std::to_underlying(wmconf::Calls::UNMAPALL)] = [&] { unmap_all(); };
  CALL[std::to_underlying(wmconf::Calls::MAPALL)] = [&] { map_all(); };
  CALL[std::to_underlying(wmconf::Calls::SWFOCUS)] = [] { };
  CALL[std::to_underlying(wmconf::Calls::TOGGLEMODE)] = [] { };
  CALL[std::to_underlying(wmconf::Calls::PREVCLI)] = [&] { prev_client(); };
  CALL[std::to_underlying(wmconf::Calls::NEXTCLI)] = [&] { next_client(); };
  CALL[std::to_underlying(wmconf::Calls::KILL)] = [&] { kill_client(); };
  CALL[std::to_underlying(wmconf::Calls::SELTOGGLE)] = [] { };
  CALL[std::to_underlying(wmconf::Calls::SELCLEAR)] = [] { };
  CALL[std::to_underlying(wmconf::Calls::QUIT)] = []{ std::raise(SIGINT); };
  CALL[std::to_underlying(wmconf::Calls::SELECT)] = [] { };
  CALL[std::to_underlying(wmconf::Calls::RESIZE)] = [] { };
  CALL[std::to_underlying(wmconf::Calls::STATE)] = [] { };
  
  for (auto i { 0 }; i < wmconf::NWKS; i++)
    WK.emplace_back(Wk { });

  some::xlib::QueryTree query { rootw };
  auto const WINS { query.get() };
  for (auto const WIN : WINS)
    ;

  some::xlib::Xinerama xinerama;
  std::cout << wmconf::WMNAME << " initialized, have a nice day!\n";
}

some::Wm::~Wm() {
  for (auto const &WK : this->WK)
    std::ranges::for_each(WK.C, 
      [this](auto const& C) { xlib.map_win(C.parw); });
    
  input.set_focus(rootw);
  std::cout << wmconf::WMNAME << " exit\n";
}

void some::Wm::sw_wk(unsigned const N) noexcept {
  if (N == currwk || N > WK.size() - 1)
    return;

  static auto& wk { this->WK[currwk] };
  for (auto const& C : wk.C)
    xlib.unmap_win(C.parw);

  prevwk = currwk;
  currwk = N;
  wk = this->WK[currwk];
  for (auto const& C : wk.C)
    xlib.map_win(C.parw);
}

void some::Wm::sw_mon(unsigned const N) const noexcept {
  if (N > 0)
    return;

}

void some::Wm::unmap_all() const noexcept {
  static auto const& WK { this->WK[currwk] };
  for (auto const& C : WK.C)
    xlib.unmap_win(C.parw);
}

void some::Wm::map_all() const noexcept {
  static auto const& WK { this->WK[currwk] };
  for (auto const& C : WK.C)
    xlib.map_win(C.parw);
}

void some::Wm::load_wk() noexcept {
  WK.emplace_back(Wk { });
  // propagate prop change
}

void some::Wm::unload_wk() noexcept {
  static auto const& WK { this->WK.cbegin() + currwk };
  static auto next { 
    std::next(this->WK.begin() + currwk) == this->WK.cend() ? 
    this->WK.begin() + currwk - 1 :
    this->WK.begin() + currwk + 1 };
  
  for (auto const& C : WK->C)
    next->C.emplace_back(C);

  this->WK.erase(WK);
  // propagate prop change
}

void some::Wm::unfocus(xlib::Win const WIN) const noexcept {
  auto const MASK { input.modmask() };
  for (auto const& B : wmconf::BTN)
    input.ungrab_btn(WIN, B.mod & MASK, B.sym);

  //delprop_active(wk[1]->client[1]->w);
  xlib.set_winbg(WIN, wmconf::COLORS[BG]);
  xlib.set_bdrcolor(WIN, wmconf::COLORS[BG]);
}

void some::Wm::focus(xlib::Win const WIN) const noexcept {
  input.set_focus(WIN);
  auto const MASK { input.modmask() };
  for (auto const& B : wmconf::BTN)
    input.grab_btn(WIN, B.mod & MASK, B.sym);
  
  xlib.set_winbg(WIN, wmconf::COLORS[SEL]);
  xlib.set_bdrcolor(WIN, wmconf::COLORS[SEL]);
}

void some::Wm::prev_client() noexcept {
  static auto& wk { this->WK[currwk] };
  if (wk.C.size() < 2)
    return;

  unfocus(wk.C[wk.currc].parw);
  wk.prevc = wk.currc;
  if (wk.currc == 0)
    wk.currc = wk.C.size() - 1;
  else
    wk.currc--;
  xlib.map_win(wk.C[wk.currc].parw);
  focus(wk.C[wk.currc].parw);
}

void some::Wm::next_client() noexcept {
  static auto& wk { this->WK[currwk] };
  if (wk.C.size() < 2)
    return;

  unfocus(wk.C[wk.currc].parw);
  wk.prevc = wk.currc;
  wk.currc++;
  if (wk.currc == wk.C.size())
    wk.currc = 0;
  xlib.map_win(wk.C[wk.currc].parw);
  focus(wk.C[wk.currc].parw);
}

void some::Wm::kill_client() {
  //(void) wk[CURR]->client[CURR];

}

void some::Recv::key(data::L const& DATA) {
  std::cout << "EV: Key Press\n";
  auto const MASK { input.modmask() };
  auto const KMOD { static_cast<int>(DATA[0]) & MASK };
  auto const KCODE { static_cast<int>(DATA[1]) };
  auto const KSYM { KCODE_KSYM[KCODE] };
  for (auto const& K : wmconf::KBD)
    if (K.mod == KMOD && K.sym == KSYM) {
      if (std::holds_alternative<char const*>(K.var)) {
        Sys const sys;
        sys.spawn(std::get<char const*>(K.var));
      } else if (std::holds_alternative<wmconf::Calls>(K.var))
        CALL[std::to_underlying(std::get<wmconf::Calls>(K.var))]();
      
      break;
    }
}

void some::Recv::button(data::L const& DATA) const {
  std::cout << "EV: Btn Press\n";
  auto const MASK { input.modmask() };
  auto const WIN { static_cast<xlib::Win>(DATA[0]) };
  auto const KMOD { static_cast<int>(DATA[1]) & MASK };
  auto const KCODE { static_cast<int>(DATA[2]) };
  input.ungrab_pointer();
}

void some::Recv::motion(data::L const& DATA) const {
  auto const WIN { static_cast<xlib::Win>(DATA[0]) };
  if (WIN == rootw) {

  }
}

void some::Recv::crossing(data::L const&) const {
  std::cout << "EV: Enter Notify\n";

}

void some::Recv::expose(data::L const&) const {
  std::cout << "EV: Expose\n";

}

void some::Recv::unmap(data::L const& DATA) {
  std::cout << "EV: Unmapnotify\n";
  auto const WIN { static_cast<xlib::Win>(DATA[1]) };
  //std::cout << "Unmap window " << WIN << "\n";
  for (auto& wk : WK)
    if (auto const C { std::ranges::find_if(wk.C, 
      [WIN](auto const& C) { return C.win == WIN; })}; C < wk.C.cend()) {
      xlib.destroy_win(C->parw);
      if (wk.C.size() == 2)
        wk.currc = wk.prevc;
      else if (wk.C.size() > 2)
        wk.currc = std::distance(wk.C.begin(), C - 1);
      
      xlib.map_win(wk.C[wk.currc].parw);
      focus(wk.C[wk.currc].parw);
      wk.C.erase(C);
      break;
    }
}

void some::Recv::map(data::L const& DATA) const {
  std::cout << "EV: Mapnotify\n";
  auto const WIN { static_cast<xlib::Win>(DATA[1]) };
  (void) WIN;
  // std::cout << "Map window " << WIN << "\n";
}

void some::Recv::maprequest(data::L const& DATA) {
  std::cout << "EV: Map Request\n";
  auto const PARW { static_cast<xlib::Win>(DATA[0]) };
  auto const WIN { static_cast<xlib::Win>(DATA[1]) };
  /*
    Handle override_redirect
  */
  try {
    xlib::WinAttr wa { WIN };
    Dim<int, int> size { wa.size() };
    size.h() += VO;
    xlib::Win const PARW { xlib.create_win(rootw, size.w(), size.h()) };
    input.select(PARW, xlib::PARMASK);
    xlib.repar_win(WIN, PARW, 0, VO);
    xlib.set_bdrwidth(PARW, wmconf::BDRPX);
    xlib.map_win(PARW);
    xlib.set_bdrwidth(WIN, 0);
    xlib.map_win(WIN);
    static auto& WK { this->WK[currwk] };
    Dim<int, int> pos { WK.C.size() ? 
      Dim<int, int> { 
        WK.C[WK.currc].pos.x() + VO, WK.C[WK.currc].pos.y() + VO } : 
      Dim<int, int> { }
    };

    xlib.move_win(PARW, pos.x(), pos.y());
    Client const C { 
      .win = WIN,
      .parw = PARW,
      .pos = pos,
      .size = size
    };
    
    WK.C.emplace_back(C);
    if (WK.C.size() > 1) {
      unfocus(WK.C[WK.currc].parw);
      WK.prevc = WK.currc;
      WK.currc = WK.C.size() - 1;
    } 
    
    focus(PARW);
    std::cout << "Map parent window " << PARW << "\n";
    std::cout << "Map window " << WIN << "\n";
  } catch (...) { }
}

void some::Recv::configure(data::L const& DATA) const {
  std::cout << "EV: Configure Notify\n";
  auto const WIN { static_cast<xlib::Win>(DATA[1]) }; 
  if (WIN == rootw) {
    Dim<int, int> const POS { DATA[2], DATA[3] };
    Dim<int, int> const SIZE { DATA[4], DATA[5] };
  }
}

void some::Recv::configurerequest(data::L const&) const {
  std::cout << "EV: Config Request\n";
}

void some::Recv::property(data::L const&) const {
  std::cout << "EV: Prop Notify\n";
}

void some::Recv::clientmessage(data::Msg const& DATA) const {
  std::cout << "EV: Client Message\n";
}
