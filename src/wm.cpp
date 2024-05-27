#include <iostream>
#include <csignal>
#include <utility>
#include <algorithm>
#include <cwchar>

#include "../inc/wm.h"
#include "../config.h"

some::Wm::Wm() : rootwin { xlib.root() }, 
  font { wmconf::FONT},
  rootpix { rootwin, xlib.dpy_width(), xlib.dpy_height(),
    xlib.depth() } {
  xlib::DefaultXError error; 
  input.select(rootwin, xlib::ROOTMASK);
  if (error.get())
    throw std::runtime_error("Initialization error (another wm running?)");

  input.ungrab_allkey(rootwin);
  auto const mask { input.modmask() };
  for (auto const& k : wmconf::KBD) {
    auto const kcode { input.keysym_keycode(k.sym) };
    KCODE_KSYM[kcode] = k.sym;
    input.grab_key(rootwin, k.mod & mask, kcode);
  }
  
  CALL[static_cast<std::size_t>(wmconf::Calls::WK0)] =
    [&] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK1)] =
    // wk enum begin
    [&] { sw_wk(1); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK2)] =
    [&] { sw_wk(2); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK3)] =
    [&] { sw_wk(3); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK4)] =
    [&] { sw_wk(4); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK5)] =
    [&] { sw_wk(5); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK6)] =
    [&] { sw_wk(6); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK7)] =
    [&] { sw_wk(7); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK8)] =
    [&] { sw_wk(8); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK9)] =
    [&] { sw_wk(9); };
  CALL[static_cast<std::size_t>(wmconf::Calls::LOADWK)] =
    [&] { load_wk(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::UNLOADWK)] =
    [&] { unload_wk(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON0)] =
    [&] { sw_mon(0); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON1)] =
    [&] { sw_mon(1); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON2)] =
    [&] { sw_mon(2); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON3)] =
    [&] { sw_mon(3); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON4)] =
    [&] { sw_mon(4); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON5)] =
    [&] { sw_mon(5); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON6)] =
    [&] { sw_mon(6); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON7)] =
    [&] { sw_mon(7); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON8)] =
    [&] { sw_mon(8); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON9)] =
    [&] { sw_mon(9); };
  CALL[static_cast<std::size_t>(wmconf::Calls::UNMAPALL)] =
    [&] { unmap_all(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MAPALL)] =
    [&] { map_all(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::SWFOCUS)] =
    [] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::TOGGLEMODE)] =
    [] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::PREVCLI)] =
    [&] { prev_client(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::NEXTCLI)] =
    [&] { next_client(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::KILL)] =
    [&] { kill_client(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::SELTOGGLE)] =
    [&] { seltoggle(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::SELCLEAR)] =
    [&] { selclear(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::QUIT)] =
    []{ std::raise(SIGINT); };
  CALL[static_cast<std::size_t>(wmconf::Calls::RESIZE)] =
    [] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::STATE)] =
    [] { };

  for (auto i { 0 }; i < wmconf::NWKS; i++)
    WK.emplace_back(Wk { });

  some::xlib::QueryTree query { rootwin };
  auto const wins { query.get() };
  for (auto const win : wins)
    ;

  try {
    some::xlib::Xinerama xinerama;
    auto const n { xinerama.number() };
    for (std::size_t i { }; i < n; i++) {
      auto const query { xinerama.query(i) };
      // queries { pos, size }
      MON.emplace_back(Mon { 
        std::get<0>(query), std::get<1>(query) });
    }

  } catch (...) {
    MON.emplace_back(Mon { { }, 
      { xlib.dpy_width(), xlib.dpy_height() } });
  }

  xlib.set_winbg(rootwin, ROOTBG);
  refresh_panel();
  std::cout << wmconf::WMNAME << " initialized\n";
}

some::Wm::~Wm() {
  for (auto const &wk : WK)
    std::ranges::for_each(wk.C, 
      [this](auto const& c) { xlib.map_win(c.parw); });
    
  input.set_focus(rootwin);
  xlib.set_winbg(rootwin, Black);
  std::cout << wmconf::WMNAME << " exit\n";
}

void
some::Wm::sw_wk(unsigned const n)
noexcept {
  if (n - 1 == currwk || n > WK.size())
    return;

  unmap_all();
  prevwk = currwk;
  currwk = n - 1;
  map_all();
}

void
some::Wm::sw_mon(unsigned const n)
const noexcept {
  if (n > 0)
    return;

}

void
some::Wm::unmap_all()
const noexcept {
  auto const& wk { WK[currwk] };
  for (auto const& c : wk.C) {
    xlib.unmap_win(c.parw);
    unfocus(c.parw);
  }
}

void
some::Wm::map_all()
const noexcept {
  auto const& wk { WK[currwk] };
  for (auto const& c : wk.C)
    xlib.map_win(c.parw);

  if (wk.C.size()) {
    focus(wk.C[wk.currc].parw);
    xlib.map_win(wk.C[wk.currc].parw);
  }
}

void
some::Wm::load_wk()
noexcept {
  WK.emplace_back(Wk { });
}

void
some::Wm::unload_wk()
noexcept {
  auto const wk { WK.cbegin() + currwk };
  auto next { 
    std::next(WK.cbegin() + currwk) == WK.cend() ? 
    WK.begin() + currwk - 1 :
    WK.begin() + currwk + 1 };
  
  for (auto const& c : wk->C)
    next->C.emplace_back(c);

  currwk = std::distance(WK.begin(), next);
  WK.erase(wk);
  // propagate prop change
}

void
some::Wm::unfocus(xlib::Win const win)
const noexcept {
  auto const mask { input.modmask() };
  for (auto const& b : wmconf::BTN)
    input.ungrab_btn(win, b.mod & mask, b.sym);

  //delprop_active(wk[1]->client[1]->w);
  xlib.set_winbg(win, wmconf::COLORS[BG]);
  xlib.set_bdrcolor(win, wmconf::COLORS[BG]);
}

void
some::Wm::focus(xlib::Win const win)
const noexcept {
  input.set_focus(win);
  auto const mask { input.modmask() };
  for (auto const& b : wmconf::BTN)
    input.grab_btn(win, b.mod & mask, b.sym);
  
  xlib.set_winbg(win, wmconf::COLORS[SEL]);
  xlib.set_bdrcolor(win, wmconf::COLORS[BG]);
}

void
some::Wm::prev_client()
noexcept {
  auto& wk { WK[currwk] };
  if (wk.C.size() < 2)
    return;

  unfocus(wk.C[wk.currc].parw);
  wk.prevc = wk.currc;
  wk.currc = wk.currc == 0 ? wk.C.size() - 1 :
    wk.currc - 1;
  xlib.map_win(wk.C[wk.currc].parw);
  focus(wk.C[wk.currc].parw);
}

void
some::Wm::next_client()
noexcept {
  auto& wk { WK[currwk] };
  if (wk.C.size() < 2)
    return;

  unfocus(wk.C[wk.currc].parw);
  wk.prevc = wk.currc;
  wk.currc = wk.currc == wk.C.size() - 1 ? 0 : 
    wk.currc + 1;
  xlib.map_win(wk.C[wk.currc].parw);
  focus(wk.C[wk.currc].parw);
}

void
some::Wm::kill_client() {

}

void
some::Wm::seltoggle()
noexcept {
  auto& wk { WK[currwk] };
  auto& c { wk.C[wk.currc] };
  c.sel = !c.sel;
  xlib.set_bdrcolor(c.parw, c.sel ? Yellow : 
    wmconf::COLORS[BG]);
}

void
some::Wm::selclear()
noexcept {
  auto& wk { WK[currwk] };
  for (auto& c : wk.C) {
    c.sel = false;
    xlib.set_bdrcolor(c.parw, wmconf::COLORS[BG]);
  }
}

void
some::Wm::refresh_panel()
const noexcept {
  // Draw panel on first mon
  auto const& mon { MON[0] };
  auto const monw { mon.size.w() };
  auto const monh { mon.size.h() };
  rootpix.fill(rootwin, xlib.default_gc(), 
    wmconf::COLORS[BG], 0, monh - font.get_scent(), 
    monw, font.get_scent());
  unsigned o { };
  static auto constexpr BOX { 20 };
  static auto constexpr BOXBDR { 2 };
  for (std::size_t i { }; auto const& wk : WK) {
    rootpix.fill(rootwin, xlib.default_gc(), 
      wmconf::COLORS[FG], o, monh - font.get_scent(), 
      BOX, font.get_scent());
    rootpix.fill(rootwin, xlib.default_gc(), 
      i == currwk ? wmconf::COLORS[SEL] : 
      wmconf::COLORS[BG], o + BOXBDR, 
      monh - font.get_scent() + BOXBDR, 
      BOX - 2 * BOXBDR, font.get_scent() - 2 * BOXBDR);
    if (wk.C.size())
      rootpix.draw_string("#", rootwin, xlib.default_gc(), 
        wmconf::COLORS[FG], o + 4, 
        monh - font.get_descent());

    o += BOX - BOXBDR;
    i++;
  }

  auto const rpad { font.text_width(wmconf::WMNAME) };
  rootpix.fill(rootwin, xlib.default_gc(), 
    ROOTBG, monw - rpad, monh - font.get_scent(), 
    rpad, font.get_scent());
  rootpix.draw_string(wmconf::WMNAME, rootwin, 
    xlib.default_gc(), wmconf::COLORS[FG], 
    monw - rpad, monh - font.get_descent());
}

void some::Wm::change_root_state() const noexcept {
  atom.change_state(rootwin);
}

void
some::Recv::key(data::L const& data)
noexcept {
  std::cout << "EV: Key Press\n";
  auto const mask { input.modmask() };
  auto const kmod { static_cast<int>(data[0]) & mask };
  auto const kcode { static_cast<int>(data[1]) };
  auto const ksym { KCODE_KSYM[kcode] };
  for (auto const& k : wmconf::KBD)
    if (k.mod == kmod && k.sym == ksym) {
      //auto const& k { k.var };
      using CallT = wmconf::Calls; 
      if (std::holds_alternative<char const*>(k.var)) {
        Sys const sys;
        sys.spawn(std::get<char const*>(k.var));
      } else if (std::holds_alternative<CallT>(k.var)) {
        auto const i {
          static_cast<std::size_t>(std::get<CallT>(k.var)) };
        CALL[i]();
    }
      
      change_root_state();
      break;
    }
}

void
some::Recv::button(data::L const& data)
const noexcept {
  std::cout << "EV: Btn Press\n";
  auto const mask { input.modmask() };
  auto const win { static_cast<xlib::Win>(data[0]) };
  auto const kmod { static_cast<int>(data[1]) & mask };
  auto const kcode { static_cast<int>(data[2]) };
  input.ungrab_pointer();
  change_root_state();
}

void
some::Recv::motion(data::L const& data)
const noexcept {
  auto const win { static_cast<xlib::Win>(data[0]) };
  if (win == rootwin) {

  }
}

void
some::Recv::crossing(data::L const&)
const noexcept {
  std::cout << "EV: Enter Notify\n";
  change_root_state();
}

void
some::Recv::expose(data::L const&)
const noexcept {
  std::cout << "EV: Expose\n";
  change_root_state();
}

void
some::Recv::unmap(data::L const& data) 
noexcept{
  std::cout << "EV: Unmapnotify\n";
  auto const win { static_cast<xlib::Win>(data[1]) };
  for (auto& wk : WK)
    if (auto const c { std::ranges::find_if(wk.C, 
      [win](auto const& c) { return c.win == win; })};
        c < wk.C.cend()) {
      xlib.destroy_win(c->parw);
      wk.currc = wk.C.size() > 2 ? 
        std::distance(wk.C.begin(), c - 1) : wk.prevc;
      
      xlib.map_win(wk.C[wk.currc].parw);
      focus(wk.C[wk.currc].parw);
      wk.C.erase(c);
      change_root_state();
      break;
    }
}

void
some::Recv::map(data::L const& data)
const noexcept {
  std::cout << "EV: Mapnotify\n";
  auto const win { static_cast<xlib::Win>(data[1]) };
  (void) win;
  change_root_state();
}

void
some::Recv::maprequest(data::L const& data)
noexcept {
  std::cout << "EV: Map Request\n";
  auto const parw { static_cast<xlib::Win>(data[0]) };
  auto const win { static_cast<xlib::Win>(data[1]) };

  try {
    xlib::WinAttr wa { win };
    if (parw != rootwin && wa.override_redirect())
      throw std::runtime_error("No");

    Dim<int, int> size { wa.size() };
    size.h() += font.get_scent();
    xlib::Win const parw { 
      xlib.create_win(rootwin, size.w(), size.h()) };
    input.select(parw, xlib::PARMASK);
    xlib.repar_win(win, parw, 0, font.get_scent());
    xlib.set_bdrwidth(parw, wmconf::BDRPX);
    xlib.map_win(parw);
    xlib.set_bdrwidth(win, 0);
    xlib.map_win(win);
    auto& wk { WK[currwk] };
    Dim<int, int> pos { wk.C.size() ? 
      Dim<int, int> { 
        wk.C[wk.currc].pos.x() + font.get_scent(), 
        wk.C[wk.currc].pos.y() + font.get_scent() } : 
      Dim<int, int> { }
    };

    xlib.move_win(parw, pos.x(), pos.y());
    Client const c { 
      .win = win,
      .parw = parw,
      .pos = pos,
      .size = size
    };
    
    wk.C.emplace_back(c);
    if (wk.C.size() > 1) {
      unfocus(wk.C[wk.currc].parw);
      wk.prevc = wk.currc;
      wk.currc = wk.C.size() - 1;
    } 
    
    focus(parw);
    std::cout << "Map parent window " << parw << "\n";
    std::cout << "Map window " << win << "\n";
  } catch (...) { }
}

void
some::Recv::configure(data::L const& data) 
const noexcept {
  std::cout << "EV: Configure Notify\n";
  auto const win { static_cast<xlib::Win>(data[1]) }; 
  if (win == rootwin) {
    Dim<int, int> const pos { data[2], data[3] };
    Dim<int, int> const size { data[4], data[5] };
  }
  
  change_root_state();
}

void
some::Recv::configurerequest(data::L const&)
const noexcept {
  std::cout << "EV: Config Request\n";
  change_root_state();
}

void
some::Recv::property(data::L const& data)
const noexcept {
  std::cout << "EV: Prop Notify\n";
  auto const win { data[0] };
  (void) win;
  refresh_panel();
}

void
some::Recv::clientmessage(data::Msg const& data) 
const noexcept {
  std::cout << "EV: Client Message\n";
  change_root_state();
}
