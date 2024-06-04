#include <iostream>
#include <csignal>
#include <algorithm>
#include <cwchar>
#include <cstring>

#include "../inc/wm.h"
//#include "/tmp/xbmp.xbm"
#include "../config.h"

some::Client::Client(some::xlib::Win const win, 
some::xlib::Win const parw, 
Dim<int, int> const& pos, Dim<int, int> const& size, 
int const depth) 
noexcept :
win { win },
parw { parw },
gc { parw },
draw { parw, size.w(), size.h(), depth },
pos { pos },
size { size }
{ }

some::Client::Client(Client&& client)
noexcept :
win { client.win },
parw { client.parw },
gc { std::move(client.gc) },
draw { std::move(client.draw) },
pos { client.pos },
size { client.size }
{ }

some::Client&
some::Client::operator=(Client&& client)
noexcept { 
  win = client.win;
  parw = client.parw;
  gc = std::move(client.gc);
  draw = std::move(client.draw);
  pos = std::move(client.pos);
  size = std::move(client.size);
  return *this;
}

some::Wm::Wm() : rootwin { xlib.root() }, 
font { wmconf::FONT},
root { rootwin, xlib.dpy_width(), xlib.dpy_height(),
  xlib.depth() },
rootgc { rootwin } {

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
    [] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK1)] =
    // wk enum begin
    [this] { sw_wk(1); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK2)] =
    [this] { sw_wk(2); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK3)] =
    [this] { sw_wk(3); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK4)] =
    [this] { sw_wk(4); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK5)] =
    [this] { sw_wk(5); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK6)] =
    [this] { sw_wk(6); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK7)] =
    [this] { sw_wk(7); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK8)] =
    [this] { sw_wk(8); };
  CALL[static_cast<std::size_t>(wmconf::Calls::WK9)] =
    [this] { sw_wk(9); };
  CALL[static_cast<std::size_t>(wmconf::Calls::LOADWK)] =
    [this] { load_wk(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::UNLOADWK)] =
    [this] { unload_wk(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON0)] =
    [this] { sw_mon(0); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON1)] =
    [this] { sw_mon(1); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON2)] =
    [this] { sw_mon(2); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON3)] =
    [this] { sw_mon(3); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON4)] =
    [this] { sw_mon(4); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON5)] =
    [this] { sw_mon(5); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON6)] =
    [this] { sw_mon(6); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON7)] =
    [this] { sw_mon(7); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON8)] =
    [this] { sw_mon(8); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MON9)] =
    [this] { sw_mon(9); };
  CALL[static_cast<std::size_t>(wmconf::Calls::UNMAPALL)] =
    [this] { unmap_all(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::MAPALL)] =
    [this] { map_all(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::SWFOCUS)] =
    [] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::TOGGLEMODE)] =
    [] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::PREVCLI)] =
    [this] { prev_client(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::NEXTCLI)] =
    [this] { next_client(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::KILL)] =
    [this] { kill_client(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::SELTOGGLE)] =
    [this] { seltoggle(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::SELCLEAR)] =
    [this] { selclear(); };
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
  ///////////////////////////////////////////////////
  /*
  std::vector<char> A;
  for (auto a : xbmp_bits)
    A.emplace_back(static_cast<char>(a));
  auto const kill { 
    rootpix.create_bitmap(rootwin, A.data(), 100, 100) };
  rootpix.copy_plane(kill, rootwin, xlib.default_gc(),
    0, 0, 100, 100, 0, 0);
  */
  ///////////////////////////////////////////////////
  refresh_root();
  refresh_panel();
  std::cout << wmconf::WMNAME << " initialized\n";
}

some::Wm::~Wm() {
  for (auto const &wk : WK)
    std::ranges::for_each(wk.C, 
      [this](auto const& c) { xlib.map_win(c.parw); });
    
  input.set_focus(rootwin);
  //xlib.set_winbg(rootwin, Black);
  rootgc.set_fg(Black);
  for (auto const& mon : MON)
    root.fill(rootgc.get(), mon.pos.x(), mon.pos.y(),
      mon.size.w(), mon.size.h());
    
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
    unfocus(c);
    xlib.unmap_win(c.parw);
    //xlib.iconify_win(c.parw);
  }
}

void
some::Wm::map_all()
const noexcept {
  auto const& wk { WK[currwk] };
  for (auto const& c : wk.C)
    xlib.map_win(c.parw);

  if (wk.C.size()) {
    focus(wk.C[wk.currc]);
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
  auto const wk { WK.begin() + currwk };
  auto next { 
    std::next(WK.cbegin() + currwk) == WK.cend() ? 
    WK.begin() + currwk - 1 :
    WK.begin() + currwk + 1 };
  
  for (auto& c : wk->C)
    next->C.emplace_back(std::move(c));

  currwk = std::distance(WK.begin(), next);
  WK.erase(wk);
  // propagate prop change
}

void
some::Wm::unfocus(Client const& c)
const noexcept {
  auto const mask { input.modmask() };
  for (auto const& b : wmconf::BTN)
    input.ungrab_btn(c.parw, b.mod & mask, b.sym);

  //delprop_active(wk[1]->client[1]->w);
  xlib.set_winbg(c.parw, wmconf::COLORS[BG]);
}

void
some::Wm::focus(Client const& c)
const noexcept {
  input.set_focus(c.parw);
  auto const mask { input.modmask() };
  for (auto const& b : wmconf::BTN)
    input.grab_btn(c.parw, b.mod & mask, b.sym);
  
  xlib.set_winbg(c.parw, wmconf::COLORS[SEL]);
  c.gc.set_fg(wmconf::COLORS[BG]);
  int const w { c.size.w() - 20 };
  int const h { font.get_scent() };
  for (int i { }; i < w; i += 3)
    for (int j { }; j < h; j += 3) {
      c.draw.fill(c.gc.get(), i, j, 2, 2);
    }
}

void
some::Wm::prev_client()
noexcept {
  auto& wk { WK[currwk] };
  if (wk.C.size() < 2)
    return;

  unfocus(wk.C[wk.currc]);
  wk.prevc = wk.currc;
  wk.currc = wk.currc == 0 ? wk.C.size() - 1 :
    wk.currc - 1;
  xlib.map_win(wk.C[wk.currc].parw);
  focus(wk.C[wk.currc]);
}

void
some::Wm::next_client()
noexcept {
  auto& wk { WK[currwk] };
  if (wk.C.size() < 2)
    return;

  unfocus(wk.C[wk.currc]);
  wk.prevc = wk.currc;
  wk.currc = wk.currc == wk.C.size() - 1 ? 0 : 
    wk.currc + 1;
  xlib.map_win(wk.C[wk.currc].parw);
  focus(wk.C[wk.currc]);
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
some::Wm::refresh_root()
const noexcept {
  rootgc.set_fg(ROOTBG);
  root.stipple(rootgc.get(), 32, 8, xlib.dpy_width(), 
    xlib.dpy_height());
}

void
some::Wm::refresh_panel()
const noexcept {
  // Draw panel on first mon
  auto const& mon { MON[0] };
  auto const monw { mon.size.w() };
  auto const monh { mon.size.h() };
  rootgc.set_fg(wmconf::COLORS[BG]);
  root.fill(rootgc.get(), 0, monh - font.get_scent(), 
    monw, font.get_scent());
  unsigned o { };
  static auto constexpr BOX { 20 };
  static auto constexpr BOXBDR { 2 };
  for (std::size_t i { }; auto const& wk : WK) {
    rootgc.set_fg(wmconf::COLORS[FG]);
    root.fill(rootgc.get(), o, monh - font.get_scent(), 
      BOX, font.get_scent());
    rootgc.set_fg(i == currwk ? wmconf::COLORS[SEL] : 
      wmconf::COLORS[BG]);
    root.fill(rootgc.get(), o + BOXBDR, 
      monh - font.get_scent() + BOXBDR, 
      BOX - 2 * BOXBDR, font.get_scent() - 2 * BOXBDR);
    if (wk.C.size()) {
      rootgc.set_fg(wmconf::COLORS[FG]);
      root.string("#", 1, rootgc.get(), o + 4, 
        monh - font.get_descent());
    }

    o += BOX - BOXBDR;
    i++;
  }

  auto const rpad { 
    font.text_width(wmconf::WMNAME, strlen(wmconf::WMNAME)) };
  rootgc.set_fg(ROOTBG);
  root.fill(rootgc.get(), monw - rpad, 
    monh - font.get_scent(), rpad, font.get_scent());
  rootgc.set_fg(wmconf::COLORS[FG]);
  root.string(wmconf::WMNAME, strlen(wmconf::WMNAME), 
    rootgc.get(), monw - rpad, monh - font.get_descent());
}

void some::Wm::change_root_state() const noexcept {
  atom.change_state(rootwin);
}

void
some::Recv::key(Data const& data)
noexcept {
  std::cout << "EV: Key Press\n";
  auto const mask { input.modmask() };
  auto const kmod { static_cast<int>(data[0]) & mask };
  auto const kcode { static_cast<int>(data[1]) };
  auto const ksym { KCODE_KSYM[kcode] };
  for (auto const& k : wmconf::KBD)
    if (k.mod == kmod && k.sym == ksym) {
      using Call = wmconf::Calls; 
      if (std::holds_alternative<char const*>(k.var)) {
        Sys const sys;
        sys.spawn(std::get<char const*>(k.var));
      } else if (std::holds_alternative<Call>(k.var)) {
        auto const i {
          static_cast<std::size_t>(std::get<Call>(k.var)) };
        CALL[i]();
    }
      
      change_root_state();
      break;
    }
}

void
some::Recv::button(Data const& data)
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
some::Recv::motion(Data const& data)
const noexcept {
  auto const win { static_cast<xlib::Win>(data[0]) };
  if (win == rootwin) {

  }
}

void
some::Recv::crossing(Data const&)
const noexcept {
  std::cout << "EV: Enter Notify\n";
  change_root_state();
}

void
some::Recv::expose(Data const&)
const noexcept {
  std::cout << "EV: Expose\n";
  change_root_state();
}

void
some::Recv::unmap(Data const& data) 
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
      focus(wk.C[wk.currc]);
      wk.C.erase(c);
      change_root_state();
      break;
    }
}

void
some::Recv::map(Data const& data)
const noexcept {
  std::cout << "EV: Mapnotify\n";
  auto const win { static_cast<xlib::Win>(data[1]) };
  (void) win;
  change_root_state();
}

void
some::Recv::maprequest(Data const& data)
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
    xlib.set_bdrcolor(parw, wmconf::COLORS[BG]);
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
    Client c { win, parw, pos, size, xlib.depth() };
    focus(c);
    wk.C.push_back(std::move(c));
    
    if (wk.C.size() > 1) {
      unfocus(wk.C[wk.currc]);
      wk.prevc = wk.currc;
      wk.currc = wk.C.size() - 1;
    }
    
    //std::cout << "Map parent window " << parw << "\n";
    //std::cout << "Map window " << win << "\n";
  } catch (...) { }
}

void
some::Recv::configure(Data const& data) 
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
some::Recv::configurerequest(Data const&)
const noexcept {
  std::cout << "EV: Config Request\n";
  change_root_state();
}

void
some::Recv::property(Data const& data)
const noexcept {
  std::cout << "EV: Prop Notify\n";
  if (auto const win { data[0] }; win == rootwin) {
    refresh_root();
    refresh_panel();
  }
}

void
some::Recv::clientmessage(Data const& data) 
const noexcept {
  std::cout << "EV: Client Message\n";
  change_root_state();
}
