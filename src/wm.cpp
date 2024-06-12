#include <iostream>
#include <csignal>
#include <algorithm>
#include <cwchar>
#include <cstring>

#include "../inc/wm.h"
//#include "/tmp/xbmp.xbm"
#include "../config.h"

some::Root::Root(some::xlib::Win const win, int const w,
int const h, int const d) :
win { win },
gc { win },
draw { win, w, h, d }
{ }

some::Client::Client(some::xlib::Win const win, 
some::xlib::Win const parw, Dim<int, int> const& pos, 
Dim<int, int> const& size, int const th, 
int const d)
noexcept :
win { win },
parw { parw },
gc { parw },
draw { parw, size.w(), th, d },
pos { pos },
size { size },
th { th }
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

some::Wm::Wm() : 
root { xlib.root_win(),
  xlib.dpy_width(), 
  xlib.dpy_height(),
  xlib.depth() },
font { wmconf::FONT } {

  static auto constexpr MASK {
    xlib::mask::SUBSTRUCTREDIR | 
    xlib::mask::SUBSTRUCTNOTIF | 
    xlib::mask::BTNPRESS |
    xlib::mask::BTNRELEASE |
    xlib::mask::PTRMOTION |
    xlib::mask::ENTERWIN |
    xlib::mask::LEAVEWIN |
    xlib::mask::STRUCTNOTIF |
    xlib::mask::PROPCHANGE |
    xlib::mask::EXPO
  };

  input.select(root.win, MASK);
  input.ungrab_allkey(root.win);
  auto const mask { input.modmask() };
  for (auto const& k : wmconf::KBD) {
    auto const kcode { input.keysym_keycode(k.sym) };
    KCODE_KSYM[kcode] = k.sym;
    input.grab_key(root.win, k.mod & mask, kcode);
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
    [this] { toggle_sel(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::SELCLEAR)] =
    [this] { clear_sel(); };
  CALL[static_cast<std::size_t>(wmconf::Calls::QUIT)] =
    []{ std::raise(SIGINT); };
  CALL[static_cast<std::size_t>(wmconf::Calls::RESIZE)] =
    [] { };
  CALL[static_cast<std::size_t>(wmconf::Calls::STATE)] =
    [] { };

  for (auto i { 0 }; i < wmconf::NWKS; i++)
    WK.emplace_back(Wk { });

  some::xlib::QueryTree query { root.win };
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

  xlib.set_winbg(root.win, ROOTBG);
  ///////////////////////////////////////////////////
  /*
  std::vector<char> A;
  for (auto a : xbmp_bits)
    A.emplace_back(static_cast<char>(a));
  auto const kill { 
    rootpix.create_bitmap(root.win, A.data(), 100, 100) };
  rootpix.copy_plane(kill, root.win, xlib.default_gc(),
    0, 0, 100, 100, 0, 0);
  */
  ///////////////////////////////////////////////////
  refresh_root();
  refresh_panel();
  std::cout << wmconf::WMNAME << " initialized\n";
}

some::Wm::~Wm() {
  // Destr. must also act static because of Display
  // Leave it empty
}

void
some::Wm::exit() const noexcept {
  xlib.set_winbg(root.win, Black);
  for (auto const &wk : WK)
    std::ranges::for_each(wk.C, 
      [this](auto const& c) { 
        xlib.repar_win(c.win, root.win, 0, 0);
        xlib.destroy_win(c.parw);
        xlib.map_win(c.win);
      });
    
  input.set_focus(root.win);
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
noexcept {
  auto const& wk { WK[currwk] };
  for (auto const& c : wk.C)
    xlib.map_win(c.parw);

  if (wk.C.size()) {
    xlib.map_win(wk.C[wk.currc].parw);
    focus(wk.C[wk.currc]);
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
noexcept {
  input.set_focus(c.parw);
  auto const mask { input.modmask() };
  for (auto const& b : wmconf::BTN)
    input.grab_btn(c.parw, b.mod & mask, b.sym);
 
  xlib.set_winbg(c.parw, wmconf::COLORS[SEL]);
  // Offset to ui class
  auto const name { prop.get_name(c.win) };
  if (name.has_value()) {
    c.gc.set_fg(wmconf::COLORS[FG]);
    c.draw.string(name.value().data(), 
      name.value().length(), c.gc.get(), 4, 
        font.get_ascent());
  }

  c.gc.set_fg(wmconf::COLORS[BG]);
  int const w0 { font.text_width(name.value().data(),
    name.value().length()) };
  int const w1 { c.size.w() - 20 };
  int const h { font.get_scent() };
  for (int i { w0 }; i < w1; i += 3)
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
some::Wm::toggle_sel()
noexcept {
  auto& wk { WK[currwk] };
  auto& c { wk.C[wk.currc] };
  c.sel = !c.sel;
  xlib.set_bdrcolor(c.parw, c.sel ? Yellow : 
    wmconf::COLORS[BG]);
}

void
some::Wm::clear_sel()
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
  root.gc.set_fg(ROOTBG);
  root.draw.stipple(root.gc.get(), 32, 8, 
    xlib.dpy_width(), xlib.dpy_height());
}

void
some::Wm::refresh_panel()
const noexcept {
  // Draw panel on first mon
  auto const& mon { MON[0] };
  auto const monw { mon.size.w() };
  auto const monh { mon.size.h() };
  root.gc.set_fg(wmconf::COLORS[BG]);
  root.draw.fill(root.gc.get(), 0, 
    monh - font.get_scent(), monw, font.get_scent());
  unsigned o { };
  static auto constexpr BOX { 20 };
  static auto constexpr BOXBDR { 2 };
  for (std::size_t i { }; auto const& wk : WK) {
    root.gc.set_fg(wmconf::COLORS[FG]);
    root.draw.fill(root.gc.get(), o, 
      monh - font.get_scent(), BOX, font.get_scent());
    root.gc.set_fg(i == currwk ? wmconf::COLORS[SEL] : 
      wmconf::COLORS[BG]);
    root.draw.fill(root.gc.get(), o + BOXBDR, 
      monh - font.get_scent() + BOXBDR, BOX - 2 * BOXBDR,
      font.get_scent() - 2 * BOXBDR);
    if (wk.C.size()) {
      root.gc.set_fg(wmconf::COLORS[FG]);
      root.draw.string("#", 1, root.gc.get(), o + 4, 
        monh - font.get_descent());
    }

    o += BOX - BOXBDR;
    i++;
  }

  auto const rpad { 
    font.text_width(wmconf::WMNAME, strlen(wmconf::WMNAME))
  };
  root.gc.set_fg(ROOTBG);
  root.draw.fill(root.gc.get(), monw - rpad, 
    monh - font.get_scent(), rpad, font.get_scent());
  root.gc.set_fg(wmconf::COLORS[FG]);
  root.draw.string(wmconf::WMNAME, strlen(wmconf::WMNAME), 
    root.gc.get(), monw - rpad, monh - font.get_descent());
}

void some::Wm::change_root_state() const noexcept {
  prop.change_state(root.win);
}

void some::Wm::change_wins_state() noexcept {
  auto& wk { WK[currwk] };
  for (auto const& c : wk.C) {
    auto const name { prop.get_name(c.win) };
    // Offset to ui class
    if (name.has_value()) {
      c.gc.set_fg(wmconf::COLORS[FG]);
      c.draw.string(name.value().data(), 
        name.value().length(), c.gc.get(), 4, 
          font.get_ascent());
    }
  }
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
some::Recv::button_press(Data const& data)
noexcept {
  auto const mask { input.modmask() };
  auto const win { static_cast<xlib::Win>(data[0]) };
  auto const x { static_cast<int>(data[4]) };
  auto const y { static_cast<int>(data[5]) };
  auto const x_root { static_cast<int>(data[6]) };
  auto const y_root { static_cast<int>(data[7]) };
  auto const kmod { static_cast<int>(data[8]) & mask };
  auto const btn { static_cast<int>(data[9]) };
  if (win == root.win) {
    std::cout << "EV: Btn Press root window\n";

  } else if (auto const c { 
    std::ranges::find_if(WK[currwk].C,
      [win](auto const& c) { return c.parw == win; }) }; 
        c < WK[currwk].C.cend()) {

    std::cout << "EV: Btn Press client window " << 
      c->parw << "\n";
    if (x < 20 && y < c->th) {
      btn1_ispressed = true;
      auto const cursor { this->cursor.get_move() };
      auto constexpr MASK {
        xlib::mask::BTNPRESS |
        xlib::mask::BTNRELEASE |
        xlib::mask::PTRMOTION
      };

      input.grab_pointer(win, MASK, cursor);
      std::cout << "Button on client " << x << " " 
        << y << "\n";
      move_origin = Dim<int, int> { x_root, y_root };

      
      Ev ev;
      ev.init_button_release([this](Data const& data) { 
        button_release(data); });
      ev.init_motion([this](Data const& data) { 
        motion(data); });
      ev.sync();
      while (btn1_ispressed) {
        ev.mask_event(//xlib::mask::BTNPRESS |
          xlib::mask::BTNRELEASE |
          xlib::mask::PTRMOTION |
          xlib::mask::EXPO |
          xlib::mask::SUBSTRUCTREDIR);

        std::cout << "EV: Btn Press client window\n";
        ev.call();
        ev.sync();
      }

      
    }
  }

  change_root_state();
}

void
some::Recv::button_release(Data const& data)
noexcept {
  std::cout << "EV: Btn Release\n";
  auto const btn { static_cast<int>(data[9]) };
  //if (btn == Button1)
  btn1_ispressed = false;
  input.ungrab_pointer();
}

void
some::Recv::motion(Data const& data)
const noexcept {
  auto const win { static_cast<xlib::Win>(data[0]) };
  auto const x { static_cast<int>(data[4]) };
  auto const y { static_cast<int>(data[5]) };
  auto const x_root { static_cast<int>(data[6]) };
  auto const y_root { static_cast<int>(data[7]) };
  if (win == root.win) {
    std::cout << "EV: Motion on root window\n";

  } else if (auto const c { 
    std::ranges::find_if(WK[currwk].C,
      [win](auto const& c) { return c.parw == win; }) }; 
        c < WK[currwk].C.cend()) {
      
      if (btn1_ispressed) {
        xlib.move_win(c->parw, x_root - move_origin.x(), 
          y_root - move_origin.y());
      }
      
  }
}

void
some::Recv::crossing(Data const& data)
noexcept {
  std::cout << "EV: Enter Notify\n";
  auto const win { static_cast<xlib::Win>(data[0]) };
  auto& wk { WK[currwk] };
  if (wk.C.size() < 2)
    return;
  else if (auto const c { std::ranges::find_if(wk.C,
      [win](auto const& c) { return c.parw == win; }) }; 
        c < wk.C.cend()) {
    unfocus(wk.C[wk.currc]);
    wk.prevc = wk.currc;
    wk.currc = std::distance(wk.C.begin(), c);
    xlib.map_win(c->parw);
    focus(wk.C[wk.currc]);
  }

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
      wk.currc = wk.C.size() > 1 ? 
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
    if (parw != root.win && wa.override_redirect())
      throw std::runtime_error("No");

    Dim<int, int> size { wa.size() };
    size.h() += font.get_scent();
    xlib::Win const parw { 
      xlib.create_win(root.win, size.w(), size.h()) };
    static auto constexpr MASK { 
      xlib::mask::SUBSTRUCTREDIR | 
      xlib::mask::SUBSTRUCTNOTIF |
      xlib::mask::BTNPRESS |
      xlib::mask::BTNRELEASE |
      xlib::mask::ENTERWIN
    };

    input.select(parw, MASK);
    xlib.repar_win(win, parw, 0, font.get_scent());
    xlib.set_bdrwidth(parw, wmconf::BDRPX);
    xlib.set_bdrcolor(parw, wmconf::COLORS[BG]);
    xlib.map_win(parw);
    xlib.set_bdrwidth(win, 0);
    xlib.map_win(win);
    auto& wk { WK[currwk] };
    Dim<int, int> pos { wk.C.size() ? 
      Dim<int, int> { 
        wk.C[wk.currc].pos.x() + CASC_OSET, 
        wk.C[wk.currc].pos.y() + CASC_OSET } : 
      Dim<int, int> { }
    };

    xlib.move_win(parw, pos.x(), pos.y());
    Client c { win, parw, pos, size, font.get_scent(), 
      xlib.depth() };
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
  if (win == root.win) {
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
noexcept {
  std::cout << "EV: Prop Notify\n";
  if (auto const win { data[0] }; win == root.win) {
    refresh_root();
    refresh_panel();
  }
    
  change_wins_state();
}

void
some::Recv::clientmessage(Data const& data) 
const noexcept {
  std::cout << "EV: Client Message\n";
  change_root_state();
}

void
some::Recv::exit() 
const noexcept {
  Wm::exit();
}
