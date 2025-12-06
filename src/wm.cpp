#include <iostream>
#include <utility>
#include <algorithm>
#include <fstream>

#include "wm.h"

class Log {
  public:
  Log() { fs.open("/tmp/dopenboxwm.log",
    std::fstream::app); }
  ~Log() { }
  template<typename T>
  Log& operator<<(T const& t) {
    fs << t;
    std::cerr << t;
    return *this;
  }

  private:
  std::fstream fs;
};

std::unordered_map<unsigned, unsigned>
  some::Kbd::kcode_ksym;
std::vector<std::pair<unsigned, unsigned>>
  some::Kbd::KMOD_KSYM;
std::map<std::pair<unsigned, unsigned>, 
  std::function<void()>> some::Kbd::call;
std::map<std::pair<unsigned, unsigned>, 
  std::string_view> some::Kbd::shell;
unsigned some::Kbd::numlockmask;
///////////////////////////////////////////////////////////
std::vector<std::pair<unsigned, unsigned>> 
  some::Btn::KMOD_BTN;
std::map<std::pair<unsigned, unsigned>, 
  std::function<void()>> some::Btn::btn;
///////////////////////////////////////////////////////////
std::pair<int, int>
some::Arrange::cascade(::Window const win, int const x,
  int const y) 
const noexcept {
  //Calc. and apply next pos to win
  std::pair<int, int> const next { 
    x + ui.cascoset, y + ui.cascoset };
  //win.move(win, next.x(), next.y());
  return next;
}

void
some::Arrange::center(::Window const win,
  int const x, int const y, int const w, int const h) 
const noexcept {
  std::pair<int, int> next { };
  //win.move(win, next.x(), next.y());
}
///////////////////////////////////////////////////////////
some::Kbd::Kbd() noexcept {

}

some::Kbd::~Kbd() {
  ::XSetInputFocus(dpy.ptr, dpy.root, RevertToPointerRoot, 
    CurrentTime);
}

void
some::Kbd::init(unsigned const kmod, unsigned const ksym,
  std::function<void()> const& f)
noexcept {
  call[{ kmod, ksym }] = f;
  KMOD_KSYM.emplace_back(kmod, ksym);
}

void
some::Kbd::init(unsigned const kmod, unsigned const ksym,
  char const* cmd)
noexcept {
  shell[{ kmod, ksym }] = cmd;
  KMOD_KSYM.emplace_back(kmod, ksym);
}

void
some::Kbd::grab_keys() {
  xlib::KeyMod keymod;
  numlockmask = keymod.numlock_mask();
  std::array<unsigned, 4> const MOD { 
    0, LockMask, numlockmask, numlockmask | LockMask
  };

  kcode_ksym.clear();
  xlib::KeySym keysym;
  auto const KSYM { keysym.get_ksyms() };
  for (auto const ksym : KSYM) {
    auto const kcode { ::XKeysymToKeycode(dpy.ptr, ksym) };
    kcode_ksym[kcode] = ksym;
  }

  ::XUngrabKey(dpy.ptr, AnyKey, AnyModifier, dpy.root);
  for (auto const mod : MOD)
    for (auto const& k : KMOD_KSYM) {
      auto const kmod { std::get<0>(k) };
      auto const kcode {
        ::XKeysymToKeycode(dpy.ptr, std::get<1>(k))
      };

      ::XGrabKey(dpy.ptr, kcode, kmod | mod, dpy.root, 
        true, GrabModeAsync, GrabModeAsync);
    }
}

unsigned
some::Kbd::kmod(unsigned const kstate) {
  static auto constexpr MASK { 
    ShiftMask | 
    ControlMask | 
    Mod1Mask |
    Mod2Mask |
    Mod3Mask |
    Mod4Mask |
    Mod5Mask
  };

  return kstate & ~(numlockmask | LockMask) & MASK;
}

void
some::Btn::init(unsigned const kmod,
  unsigned const sym, std::function<void()> const& f)
noexcept {
  btn[{ kmod, sym }] = f;
  KMOD_BTN.emplace_back(kmod, sym);
}
///////////////////////////////////////////////////////////
some::Wg::Wg(::Window const parw, int const w, int const h, int const d)
noexcept :
  win { ::XCreateSimpleWindow(dpy.ptr, parw, 0, 0, w, h, 
    0, 0, 0) },
  gc { win },
  draw { win, w, h, d },
  w { w }, 
  h { h }
{
  static auto constexpr MASK {
    SubstructureRedirectMask |
    SubstructureNotifyMask |
    ButtonPressMask |
    ButtonReleaseMask |
    PointerMotionMask |
    EnterWindowMask |
    LeaveWindowMask |
    PropertyChangeMask |
    ExposureMask
  };
  
  ::XSelectInput(dpy.ptr, win, MASK);
}

some::Wg::Wg(Wg&& wg)
noexcept :
  win { std::exchange(wg.win, 0) },
  gc { std::move(wg.gc) },
  draw { std::move(wg.draw) },
  w { wg.w },
  h { wg.h }
{

}

some::Wg&
some::Wg::operator=(Wg&& wg)
noexcept {
  win = std::exchange(wg.win, 0);
  gc = std::move(wg.gc);
  draw = std::move(wg.draw);
  w = wg.w;
  h = wg.h;
  return *this;
}

some::Wg::~Wg() {
  if (win) {
    Log() << "Destroy widget " << win << "\n";
    ::XDestroyWindow(dpy.ptr, win);
  }
}
///////////////////////////////////////////////////////////
some::Client::Client(::Window const win, int const x, 
  int const y, int const w, int const h) 
noexcept :
  win { win },
  par { dpy.root, w, h + ui.clen, dpy.depth },
  btn0 { par.win, ui.clen, ui.clen, dpy.depth },
  btn1 { par.win, ui.clen, ui.clen, dpy.depth },
  btn2 { par.win, ui.clen, ui.clen, dpy.depth },
  icon { dpy.root, ui.icow, ui.clen, dpy.depth },
  x { x },
  y { y },
  w { w }, 
  h { h + ui.clen }
{
  ::XReparentWindow(dpy.ptr, win, par.win, 0, ui.clen);
  ::XSetWindowBorderWidth(dpy.ptr, par.win, ui.bdrw);
  ::XSetWindowBorderWidth(dpy.ptr, win, 0);
  ::XMapRaised(dpy.ptr, par.win);
  ::XMapRaised(dpy.ptr, win);
  ::XMapRaised(dpy.ptr, btn0.win);
  ::XMapRaised(dpy.ptr, btn1.win);
  ::XMapRaised(dpy.ptr, btn2.win);
  ::XSetWindowBorderWidth(dpy.ptr, icon.win, 0);
}

some::Client::Client(Client&& client) 
noexcept :
  win { std::exchange(client.win, 0) },
  par { std::move(client.par) },
  btn0 { std::move(client.btn0) },
  btn1 { std::move(client.btn1) },
  btn2 { std::move(client.btn2) },
  icon { std::move(client.icon) },
  x { client.x },
  y { client.y },
  w { client.w },
  h { client.h },
  mode { client.mode },
  sel { client.sel }
{

}

some::Client& 
some::Client::operator=(some::Client&& client)
noexcept { 
  win = std::exchange(client.win, 0);
  par = std::move(client.par);
  btn0 = std::move(client.btn0);
  btn1 = std::move(client.btn1);
  btn2 = std::move(client.btn2);
  icon = std::move(client.icon);
  x = client.x;
  y = client.y;
  w = client.w;
  h = client.h;
  mode = client.mode;
  sel = client.sel;
  return *this;
}

some::Client::~Client() {
  if (win) {
    ::XReparentWindow(dpy.ptr, win, dpy.root, x, y);
    ::XUngrabButton(dpy.ptr, AnyButton, AnyModifier, win);
    ::XMapRaised(dpy.ptr, win);
    Log() << "Reparented " << win << " to root\n";
  }
}

void
some::Client::map()
const noexcept {
  ::XMapRaised(dpy.ptr, par.win);
}

void
some::Client::unmap()
const noexcept {
  ::XUnmapWindow(dpy.ptr, par.win);
}

void
some::Client::set_bg(std::size_t const col)
const noexcept {
  ::XSetWindowBackground(dpy.ptr, par.win, col);
  ::XClearWindow(dpy.ptr, par.win);
  ::XSetWindowBackground(dpy.ptr, icon.win, col);
  ::XClearWindow(dpy.ptr, icon.win);
}

void
some::Client::set_bdrcolor(std::size_t const col)
const noexcept {
  ::XSetWindowBorder(dpy.ptr, par.win, col);
}

void
some::Client::grab_btns()
const noexcept {
  xlib::KeyMod keymod;
  auto const numlockmask { keymod.numlock_mask() };
  std::array<unsigned, 4> const MOD { 
    0, LockMask, numlockmask, numlockmask | LockMask
  };
 	
  ::XUngrabButton(dpy.ptr, AnyButton, AnyModifier, 
    par.win);
  static auto constexpr MASK {
    ButtonPressMask | ButtonReleaseMask
  };

  for (auto const& b : btn.KMOD_BTN)
    for (auto mod : MOD)
      ::XGrabButton(dpy.ptr, std::get<1>(b), 
        std::get<0>(b) | mod, par.win, false, MASK,
            GrabModeSync, GrabModeSync, None, None);
}

void
some::Client::grab_any_btn()
const noexcept {
  static auto constexpr MASK {
    ButtonPressMask | ButtonReleaseMask
  };
 	
  ::XGrabButton(dpy.ptr, AnyButton, AnyModifier, par.win, 
    false, MASK, GrabModeSync, GrabModeSync, None, None);
}

void
some::Client::set_focus()
const noexcept {
  ::XSetInputFocus(dpy.ptr, par.win, RevertToPointerRoot, 
    CurrentTime);
}

void
some::Client::toggle_sel(std::size_t const selcol, 
  std::size_t const col)
noexcept {
  sel = !sel;
  ::XSetWindowBorder(dpy.ptr, par.win, sel ? selcol : col);
}

void
some::Client::clear_sel(std::size_t const col)
noexcept {
  sel = false;
  ::XSetWindowBorder(dpy.ptr, par.win, col);
}

void
some::Client::refresh()
const noexcept {
  ::XSetWindowBorder(dpy.ptr, par.win,
    sel ? ui.COLORS[ui.SEL] : ui.COLORS[ui.BG]);

  int w0 { };
  if (auto const name { prop.get_name(win) };
    name.has_value()) {
    par.gc.set_fg(ui.COLORS[ui.FG]);
    par.draw.string(name.value().data(), 
      name.value().length(), par.gc.get(), 4, 
        font.get_ascent());
    w0 = font.text_width(name.value().data(), 
      name.value().length());
  }
  
  if (auto const icon { prop.get_icon(win) };
    icon.has_value()) {
    this->icon.gc.set_fg(ui.COLORS[ui.FG]);
    this->icon.gc.set_bg(ui.COLORS[ui.BG]);
    this->icon.draw.string(icon.value().data(), 
      ui.icostrlen, this->icon.gc.get(), 4, 
        font.get_ascent());
  }

  int const w1 { w - 20 };
  int const h { ui.clen };
  par.gc.set_fg(ui.COLORS[ui.BG]);
  for (int i { w0 }; i < w1; i += 3)
    for (int j { }; j < h; j += 3)
      par.draw.fill(par.gc.get(), i, j, 2, 2);
}

bool
some::Client::move(int const x, int const y)
noexcept {
  return y > ui.bdrw && y < ui.clen;
}

void
some::Client::move(int const x, int const y, 
  int const x_root, int const y_root)
noexcept {
  static auto constexpr MASK {
    ButtonPressMask |
    ButtonReleaseMask |
    PointerMotionMask
  };

  auto const cursor { this->cursor.move };
  ::XGrabPointer(dpy.ptr, dpy.root, false, MASK, 
    GrabModeAsync, GrabModeAsync, None, cursor, 
    CurrentTime);
  int const x0 { this->x };
  int const y0 { this->y };
  auto const on_motion {
    [&](::XMotionEvent const& xmotion) {
      auto const x { xmotion.x };
      auto const y { xmotion.y };
      auto const next_x { x0 + x - x_root };
      auto const next_y { y0 + y - y_root };
      ::XMoveWindow(dpy.ptr, par.win, next_x, next_y);
      this->x = next_x;
      this->y = next_y;
    }
  };
  
  Ev ev;
  for (auto gen { ev.seq() };;) {
    /*
    ev.mask_event(ButtonPressMask |
      ButtonReleaseMask |
      PointerMotionMask |
      ExposureMask |
      SubstructureRedirectMask);
    */
    Log() << "EV: Event on client window\n";
    if (auto const xev { gen.next() };
      xev.type == ButtonRelease)
      break;
    else if (xev.type == MotionNotify)
      on_motion(xev.xmotion);

    //::XSync(dpy.ptr, false);
  }

  ::XUngrabPointer(dpy.ptr, CurrentTime);
}

bool
some::Client::resize(int const x, int const y)
noexcept {
  static auto constexpr MASK {
    ButtonPressMask |
    ButtonReleaseMask |
    PointerMotionMask
  };

  if (x > w - ui.bdrw && x < w + ui.bdrw &&
    y > h - ui.bdrw && y < h + ui.bdrw)
    // Bottom Right
    return ::XGrabPointer(dpy.ptr, par.win, false, MASK, 
      GrabModeAsync, GrabModeAsync, None, cursor.resize, 
      CurrentTime) == GrabSuccess;
  else if (x > -ui.bdrw && x < ui.bdrw)
    // Left
    return ::XGrabPointer(dpy.ptr, par.win, false, MASK, 
      GrabModeAsync, GrabModeAsync, None, cursor.h, 
      CurrentTime) == GrabSuccess;
  else if (x > w - ui.bdrw && x < w + ui.bdrw)
    // Right
    return ::XGrabPointer(dpy.ptr, par.win, false, MASK, 
      GrabModeAsync, GrabModeAsync, None, cursor.h, 
      CurrentTime) == GrabSuccess;
  else if (y > -ui.bdrw && y < ui.bdrw)
    // Top
    return ::XGrabPointer(dpy.ptr, par.win, false, MASK, 
      GrabModeAsync, GrabModeAsync, None, cursor.v, 
      CurrentTime) == GrabSuccess;
  else if (y > h - ui.bdrw && y < h + ui.bdrw)
    // Bottom
    return ::XGrabPointer(dpy.ptr, par.win, false, MASK, 
      GrabModeAsync, GrabModeAsync, None, cursor.v, 
      CurrentTime) == GrabSuccess;
  
  ::XUngrabPointer(dpy.ptr, CurrentTime);
  return false;
}

void
some::Client::resize(int const x, int const y, 
  int const x_root, int const y_root)
noexcept {
  //int const w0 { this->w };
  //int const h0 { this->h };
  auto const on_motion {
    [&](::XMotionEvent const& xmotion) {
      auto const x { xmotion.x };
      auto const y { xmotion.y };
      //auto const next_w { w0 + x - x_root };
      auto const next_w { x - this->x };
      //auto const next_h { h0 + y - y_root };
      auto const next_h { y - this->y };
      //::XResizeWindow(dpy.ptr, par.win, next_w, next_h);
      w = par.w = std::max(next_w, 1);
      h = std::max(next_h, 1);
      ::XWindowChanges wc;
      wc.width = w;
      wc.height = h - ui.clen;
      ::XConfigureWindow(dpy.ptr, win, CWWidth | CWHeight,
        &wc);
      ::XWindowChanges par_wc { wc };
      par_wc.height += ui.clen;
      ::XConfigureWindow(dpy.ptr, par.win, 
          CWWidth | CWHeight, &par_wc);
    }
  };
  
  Ev ev;
  for (auto gen { ev.seq() };;) {
    /*
    ev.mask_event(ButtonReleaseMask |
      PointerMotionMask |
      ExposureMask |
      SubstructureRedirectMask);
    */
  
    Log() << "EV: Btn Press client window\n";
    if (auto const xev { gen.next() }; 
      xev.type == ButtonRelease)
      break;
    else if (xev.type == MotionNotify)
      on_motion(xev.xmotion);

    //::XSync(dpy.ptr, false);
  }
  
  ::XUngrabPointer(dpy.ptr, CurrentTime);
}
///////////////////////////////////////////////////////////
/*
some::Root::Wk::Wk(int const w, int const h) 
noexcept :
wg {
  ::XCreateSimpleWindow(dpy.ptr, dpy.root, 0, 0, 
    w, h, 0, 0, 0), 
    w, h, dpy.depth
} {
  ::XSelectInput(dpy.ptr, wg.win, ButtonPressMask);
  ::XSetWindowBorderWidth(dpy.ptr, wg.win, 0);
  ::XMapRaised(dpy.ptr, wg.win);
}
*/
some::Root::Wk::Wk()
noexcept {
  C.reserve(100);
}

some::Root::Wk::Wk(Wk&& wk)
noexcept :
  C { std::move(wk.C) },
  wg { std::move(wk.wg) } {

}

some::Root::Wk&
some::Root::Wk::operator=(some::Root::Wk&& wk)
noexcept {
  C = std::move(wk.C);
  wg = std::move(wk.wg);
  return *this;
}

void
some::Root::Wk::init_wg(int const w, int const h) 
noexcept {
  wg = { dpy.root, w, h, dpy.depth };
  //::XSelectInput(dpy.ptr, wg.win, ButtonPressMask);
  ::XSetWindowBorderWidth(dpy.ptr, wg.win, 0);
  ::XMapRaised(dpy.ptr, wg.win);
}
///////////////////////////////////////////////////////////
some::Root::Root(char const NAME[]) noexcept :
wmname { NAME } {
  wk.reserve(10);
  mon.reserve(5);
  static auto constexpr MASK {
    SubstructureRedirectMask |
    SubstructureNotifyMask |
    ButtonPressMask |
    ButtonReleaseMask |
    PointerMotionMask |
    EnterWindowMask |
    LeaveWindowMask |
    StructureNotifyMask |
    PropertyChangeMask |
    ExposureMask
  };

  ::XSelectInput(dpy.ptr, dpy.root, MASK);
  ::remove("/tmp/dopenboxwm.log");
}

some::Root::~Root() {

}

void
some::Root::init_wks(unsigned const n)
noexcept {
  if (n == 0)
    wk.emplace_back();
  else {
    for (unsigned i { }; i < n; i++)
      wk.emplace_back();
  }

  prevwk = currwk = wk.begin();
}

void
some::Root::init_tree()
noexcept {
  some::xlib::QueryTree query { dpy.root };
  auto const wins { query.get() };
  for (auto const win : wins) {
    if (::XWindowAttributes wa;
      ::XGetWindowAttributes(dpy.ptr, win, &wa) &&
        wa.map_state == IsViewable)
      map_request(::XMapRequestEvent {
        .send_event { false },
        .parent { dpy.root },
        .window { win }
      });
  }
}

void
some::Root::init_mons()
noexcept {
  try {
    some::xlib::Xinerama xinerama;
    auto const n { xinerama.number() };
    for (int i { }; i < n; i++) {
      auto const query { xinerama.query(i) };
      // queries { pos, size }
      auto const& pos { std::get<0>(query) };
      auto const& size { std::get<1>(query) };
      mon.emplace_back(std::get<0>(pos),
        std::get<1>(pos),
        std::get<0>(size),
        std::get<1>(size));
    }
  } catch (...) {
    mon.emplace_back(0, 0, dpy.width, dpy.height);
  }

  Log() << "Number of mons " << mon.size() << "\n";
  for (auto const& mon : this->mon)
    Log() << "mon " << 
      "pos (" << mon.x << "," << mon.y << ")" <<
      " size (" << mon.w << "," << mon.h << ")" << "\n";
}
  ///////////////////////////////////////////////////
  /*
  std::vector<char> A;
  for (auto a : xbmp_bits)
    A.emplace_back(static_cast<char>(a));
  auto const kill { 
    rootpix.create_bitmap(root.win, A.data(), 100, 100) };
  rootpix.copy_plane(kill, root.win, win.default_gc(),
    0, 0, 100, 100, 0, 0);
  */
  ///////////////////////////////////////////////////
void
some::Root::init_wgs()
noexcept {
  if (wk.size() > 1) {
    for (auto& wk : this->wk)
      wk.init_wg(ui.clen, ui.clen);
  }

  status = Wg { dpy.root, 1, 1, dpy.depth };
  ::XMapRaised(dpy.ptr, status.win);
  icon = Wg { dpy.root, 1, 1, dpy.depth };
}

void
some::Root::deinit() 
noexcept {
  wk.clear();
  ::XDestroyWindow(dpy.ptr, status.win);
  ::XDestroyWindow(dpy.ptr, icon.win);
}

void
some::Root::sw_wk(unsigned const n)
noexcept {
  if (wk.cbegin() + n - 1 == currwk || n > wk.size())
    return;

  for (auto const& c : currwk->C) {
    c.unmap();
    ::XUnmapWindow(dpy.ptr, c.icon.win);
  }
  
  prevwk = currwk;
  currwk = wk.begin() + n - 1;
  for (auto const& c : currwk->C)
    c.map();

  if (currwk->C.size()) {
    focus();
  }
}

void
some::Root::sw_mon(unsigned const n)
const noexcept {
  if (n > 0)
    return;

}

void
some::Root::load_wk()
noexcept {
  wk.emplace_back();
  wk.back().init_wg(ui.clen, ui.clen);
}

void
some::Root::unload_wk()
noexcept {
  if (wk.size() == 1)
    return;

  auto next { 
    std::next(currwk) == wk.cend() ? currwk - 1 :
    currwk + 1
  };
  
  for (auto& c : currwk->C)
    next->C.emplace_back(std::move(c));

  wk.erase(currwk);
  currwk = next;
}

void
some::Root::focus()
noexcept {
  if (currwk->C.size() == 0)
    return;
  if (currwk->C.size() == 1)
    currwk->prevc = currwk->currc = 0;

  auto const prevc { currwk->C.cbegin() + currwk->prevc };
  auto const currc { currwk->C.cbegin() + currwk->currc };
  prevc->grab_any_btn();
  prevc->set_bg(ui.COLORS[ui.BG]);
  currc->set_focus();
  currc->grab_btns();
  currc->set_bg(ui.COLORS[ui.ACTSEL]);
  currc->map();
}

void
some::Root::next()
noexcept {
  //if (currwk->C.size() < 2)
    //return;

  currwk->prevc = currwk->currc;
  ++currwk->currc %= currwk->C.size();
  //currwk->currc == currwk->C.end() - 1 ? 
    //currwk->C.begin() : currwk->currc + 1;

  focus();
}

void
some::Root::prev()
noexcept {
  //if (currwk->C.size() < 2)
    //return;
  
  currwk->prevc = currwk->currc;
  if (--currwk->currc < 0)
    currwk->currc = currwk->C.size() + currwk->currc;
  //currwk->currc == currwk->C.begin() ? 
    //currwk->C.end() - 1 : currwk->currc - 1;

  focus();
}

void
some::Root::rotate_next()
noexcept {

}

void
some::Root::rotate_prev()
noexcept {

}

void
some::Root::kill_client() {
  auto const c { currwk->C.cbegin() + currwk->currc };
  ::XDestroyWindow(dpy.ptr, c->win);
  Log() << "Kill Client " << c->win << "\n";
}

void
some::Root::toggle_sel()
noexcept {
  auto const currc { currwk->C.begin() + currwk->currc };
  currc->toggle_sel(ui.COLORS[ui.SEL],
    ui.COLORS[ui.ACTSEL]);
}

void
some::Root::clear_sel()
noexcept {
  for (auto& c : currwk->C)
    c.clear_sel(ui.COLORS[ui.BG]);

  auto currc { currwk->C.cbegin() + currwk->currc };
  currc->set_bdrcolor(ui.COLORS[ui.ACTSEL]);
}

void
some::Root::refresh_root()
noexcept {
  auto const& mon { this->mon[0] };
}

void
some::Root::refresh_panels()
noexcept {
  // Draw panels on first monitor
  auto const& mon { this->mon[0] };
  auto const Y { mon.h - ui.clen }; 

  unsigned pos_wk { };
  if (wk.size() > 1) {
    for (auto& wk : this->wk) {
      ::XSetWindowBackground(dpy.ptr, wk.wg.win, 
        ui.COLORS[ui.FG]);
      //::XClearWindow(dpy.ptr, wk.wg.win);
      wk.wg.gc.set_fg(wk.wg.win == currwk->wg.win ? 
        ui.COLORS[ui.ACTSEL] : ui.COLORS[ui.BG]);

      wk.wg.draw.fill(wk.wg.gc.get(),
        ui.bdrw, ui.bdrw, 
        wk.wg.w - 2 * ui.bdrw,
        wk.wg.h - 2 * ui.bdrw);

      if (wk.C.size()) {
        wk.wg.gc.set_fg(ui.COLORS[ui.FG]);
        wk.wg.draw.string("#", 1, wk.wg.gc.get(), 
          4, 
          wk.wg.h - 4);
      }

      ::XMoveWindow(dpy.ptr, wk.wg.win, pos_wk, Y);
      pos_wk += wk.wg.w;
    }
  }

  ::XSetWindowBackground(dpy.ptr, status.win,
    ui.COLORS[ui.FG]);
  //::XClearWindow(dpy.ptr, status.win);
  char const* s { wmname.data() };
  auto const len { wmname.length() };
  auto const textw { font.text_width(s, len) };
  ::XResizeWindow(dpy.ptr, status.win, textw, ui.clen);
  auto const pos_status { mon.w - textw };
  ::XMoveWindow(dpy.ptr, status.win, pos_status, Y);
  status.gc.set_fg(ui.COLORS[ui.BG]);
  status.draw.fill(status.gc.get(), 
    0, 0, textw, font.get_scent());
  status.gc.set_fg(ui.COLORS[ui.FG]);
  status.draw.string(s, len, status.gc.get(),
    4, 14 - font.get_descent());

  if (currwk->C.size() && 
    (pos_status - pos_wk) / currwk->C.size() < 40) {
    for (auto const& c : currwk->C)
      ::XUnmapWindow(dpy.ptr, c.icon.win);

    //auto const c { currwk->currc };
    auto const c { currwk->C.cbegin() + currwk->currc };
    ::XSetWindowBackground(dpy.ptr, icon.win,
      ui.COLORS[ui.ACTSEL]);
    icon.gc.set_fg(ui.COLORS[ui.FG]);
    if (auto const icon { prop.get_icon(c->win) };
      icon.has_value())
      this->icon.draw.string(icon.value().data(), 
        icon.value().length(), this->icon.gc.get(), 4, 
          font.get_ascent());
    else
      this->icon.draw.string("...", 3, 
        this->icon.gc.get(), 4, font.get_ascent());
    
    ::XMapRaised(dpy.ptr, icon.win);
    ::XResizeWindow(dpy.ptr, icon.win, 
      pos_status - pos_wk - 1, 14);
    ::XMoveWindow(dpy.ptr, icon.win, pos_wk + 1, Y);
    return;
  }

  ::XUnmapWindow(dpy.ptr, icon.win);
  unsigned pos_client { pos_wk + 1 };
  for (auto const& c : currwk->C) {
    ::XMapRaised(dpy.ptr, c.icon.win);
    ::XMoveWindow(dpy.ptr, c.icon.win, pos_client, Y);
    c.refresh();
    pos_client += c.icon.w + 1;
  }
  // TODO: call this only if wks change or 
  // clients change or 
  // status changes or
  // first mon size changes
}

void
some::Root::change_state() 
const noexcept {
  prop.change_state(dpy.root);
}

void
some::Root::move_client() 
const noexcept {
  if (currwk->C.size() > 0) {
    //auto const c { currwk->currc };
    auto const c { currwk->C.begin() + currwk->currc };
    c->move(c->ptr_x, c->ptr_y, 
      c->ptr_x_root, c->ptr_y_root);
  }
}

void
some::Root::resize_client() 
const noexcept {
  Log() << "Resize client\n";

  if (currwk->C.size() > 0) { 
    //auto const c { currwk->currc };
    auto const c { currwk->C.begin() + currwk->currc };
    ::XWarpPointer(dpy.ptr, None, c->par.win, 0, 0, 0, 0, 
      c->w + ui.bdrw - 1, c->h + c->ui.bdrw - 1); 
    if (c->resize(c->w, c->h)) {
      c->resize(c->ptr_x, c->ptr_y, 
        c->ptr_x_root, c->ptr_y_root);
    }
  }
}
///////////////////////////////////////////////////////////
void
some::Root::key(::XKeyEvent const& xkey)
noexcept {
  Log() << "EV: Key Press\n";
  auto const win { xkey.window };
  (void) win;
  auto const x { xkey.x };
  (void) x;
  auto const y { xkey.y };
  (void) y;
  auto const x_root { xkey.x_root };
  (void) x_root;
  auto const y_root { xkey.y_root };
  (void) y_root;

  auto const kmod { kbd.kmod(xkey.state) };
  auto const kcode { xkey.keycode };
  auto const ksym { kbd.kcode_ksym.at(kcode) };
  try {
    auto const& call { kbd.call.at({ kmod, ksym }) };
    call();
  } catch (...) { }

  try {
    auto const& shell { kbd.shell.at({ kmod, ksym }) };
    sys.spawn(shell.data());
  } catch (...) { }
  
  change_state();
}

void
some::Root::button_press(::XButtonEvent const& xbutton)
noexcept {
  auto const win { xbutton.window };
  auto const x { xbutton.x };
  auto const y { xbutton.y };
  auto const x_root { xbutton.x_root };
  auto const y_root { xbutton.y_root };

  auto const kmod { kbd.kmod(xbutton.state) };
  auto const btn { xbutton.button };
  if (win == dpy.root) {
    Log() << "EV: Btn Press root window\n";

  } else if (auto const wk { 
    std::ranges::find_if(this->wk, [win](auto const& wk) {
      return wk.wg.win == win; }) }; wk < this->wk.cend())
    // Workspace counts from logical index 1
    sw_wk(std::distance(this->wk.begin(), wk) + 1);

  else if (auto const c { std::ranges::find_if(currwk->C, 
    [win](auto const& c) { 
      return c.par.win == win || 
        c.icon.win == win ||
        c.btn0.win == win ||
        c.btn1.win == win ||
        c.btn2.win == win; }) }; c < currwk->C.cend()) {
    c->ptr_x = x;
    c->ptr_y = y;
    c->ptr_x_root = x_root;
    c->ptr_y_root = y_root;
    try {
      auto const& call { this->btn.btn.at({ kmod, btn }) };
      call();
    } catch (...) { }

    if (win == c->par.win && btn == Button1) {
      Log() << "EV: Btn Press client window " << 
      win << "\n";
      if (c->move(x, y))
        c->move(x, y, x_root, y_root);
      else if (c->resize(x, y))
        c->resize(x, y, x_root, y_root);
    }

    else if (win == c->icon.win) {
      // Btn Press client icon
      c->map();
    } else if (win == c->btn0.win) {
      // Btn Press client btn0w
      c->unmap();
    } else if (win == c->btn1.win) {
      // Btn Press client btn1w
      // Do maximize
      ;
    } else if (win == c->btn2.win) {
      // Btn Press client btn2w
      // Do close
      ;
    }
  }

  change_state();
}

void
some::Root::button_release(::XButtonEvent const& xbutton)
noexcept {
  Log() << "EV: Btn Release\n";
  auto const btn { xbutton.button };
  (void) btn;
}

void
some::Root::motion(::XMotionEvent const& xmotion)
const noexcept {
  auto const win { xmotion.window };
  auto const x { xmotion.x };
  auto const y { xmotion.y };
  auto const x_root { xmotion.x_root };
  auto const y_root { xmotion.y_root };
  if (win == dpy.root) {
    Log() << "EV: Motion on root window " <<
      win << " (" << x_root << ", " << y_root << ")\n";

  } else if (auto const c { 
    std::ranges::find_if(currwk->C, [win](auto const& c) { 
      return c.par.win == win; }) }; 
      c < currwk->C.cend()) {
    c->ptr_x = x;
    c->ptr_y = y;
    c->ptr_x_root = x_root;
    c->ptr_y_root = y_root;
    c->resize(x, y);
    Log() << "EV: Motion on client window " <<
      win << " (" << x << ", " << y << ")\n";
  }
}

void
some::Root::crossing(::XCrossingEvent const& xcrossing)
noexcept {
  Log() << "EV: Enter Notify\n";
  auto const win { xcrossing.window };
  if (currwk->C.size() < 2)
    return;
  else if (auto const c { std::ranges::find_if(currwk->C,
    [win](auto const& c) { return c.par.win == win; }) };
      c < currwk->C.cend()) {
    currwk->prevc = currwk->currc;
    currwk->currc = c - currwk->C.cbegin();
    focus();
    change_state();
  }
}

void
some::Root::focus_change(::XFocusChangeEvent const& xfocus)
noexcept {
  auto const win { xfocus.window };
  auto const mode { xfocus.mode };
  (void) mode;
  auto const detail { xfocus.detail };
  (void) detail;
  if (auto const c { std::ranges::find_if(currwk->C,
    [win](auto const& c) { return c.win == win; }) };
      c < currwk->C.cend()) {
    currwk->prevc = currwk->currc;
    //currwk->currc = c;
    currwk->currc = c - currwk->C.cbegin();
    focus();
    change_state();
  }
}

void
some::Root::expose(::XExposeEvent const& xexpose)
const noexcept {
  Log() << "EV: Expose\n";
  change_state();
}

void
some::Root::unmap(::XUnmapEvent const& xunmap) 
noexcept{
  auto const win { xunmap.window };
  Log() << "EV: Unmapnotify window " << win << "\n";
  (void) win;
  change_state();
}

void
some::Root::map(::XMapEvent const& xmap)
const noexcept {
  Log() << "EV: Mapnotify\n";
  auto const win { xmap.window };
  (void) win;
  change_state();
}

void
some::Root::map_request(
  ::XMapRequestEvent const& xmaprequest)
noexcept {
  Log() << "EV: Map Request\n";
  auto const parw { xmaprequest.parent };
  auto const win { xmaprequest.window };
  ::XWindowAttributes wa;
  if (::XGetWindowAttributes(dpy.ptr, win, &wa) &&
    wa.override_redirect)
    return;
  
  if (Client c { win, wa.x, wa.y, wa.width, wa.height };
    parw != dpy.root) {
    // A child of some client (eg. boxes)
    Log() << "MapRequest child created\n";
    //focus(c);
    //xlib::WinAttr wa { parw };
    //arrange.center(std::get<::Window>(next), wa.size(),
      //wa.pos());

  } else {
    // A child of root
    Log() << "Maprequest window " << c.win 
      << " Parent " << c.par.win << "\n";
    
    currwk->C.emplace_back(std::move(c));
    currwk->prevc = currwk->currc;
    //currwk->currc = currwk->C.end() - 1;
    currwk->currc++;
    focus();
  }
}

void
some::Root::destroy(
  ::XDestroyWindowEvent const& xdestroy)
noexcept {
  auto const win { xdestroy.window };
  Log() << "EV: Destroy window " << win << "\n";
  for (auto& wk : this->wk)
    if (auto const c { std::ranges::find_if(wk.C, 
      [win](auto const& c) { return c.win == win; })};
        c != wk.C.cend()) {
      //prev();
      Log() << "EV: Erase client " << win << "\n";
      wk.C.erase(c);
      if (currwk->currc)
        currwk->currc--;

      focus();
      break;
    }
}

void
some::Root::configure(::XConfigureEvent const& xconfigure) 
const noexcept {
  Log() << "EV: Configure Notify\n";
  auto const win { xconfigure.window }; 
  auto const x { xconfigure.x };
  (void) x;
  auto const y { xconfigure.y };
  (void) y;
  auto const w { xconfigure.width };
  (void) w;
  auto const h { xconfigure.height };
  (void) h;
  auto const bdrw { xconfigure.border_width };
  (void) bdrw;
  auto const raise { xconfigure.above };
  (void) raise;
  if (win == dpy.root) {
    change_state();
  } else {
  
  }
}

void
some::Root::configure_request(
  ::XConfigureRequestEvent const& xconfreq)
const noexcept {
  Log() << "EV: Config Request\n";
  ::XWindowChanges wc {
    xconfreq.x, 
    xconfreq.y,
    xconfreq.width,
    xconfreq.height,
    xconfreq.border_width,
    xconfreq.above,
    xconfreq.detail
  };
  
  ::XConfigureWindow(dpy.ptr, xconfreq.window, 
    xconfreq.value_mask, &wc);
  change_state();
}

void
some::Root::property(::XPropertyEvent const& xproperty)
noexcept {
  Log() << "EV: Prop Notify\n";
  if (auto const win { xproperty.window }; 
    win == dpy.root) {
    refresh_root();
    refresh_panels();
  }
}

void
some::Root::client_message(
  ::XClientMessageEvent const& xmsg) 
const noexcept {
  Log() << "EV: Client Message\n";
  change_state();
}

void
some::Root::mapping(::XMappingEvent const& xmapping)
noexcept {
  Log() << "EV: Mapping\n";
  ::XMappingEvent _xmapping { xmapping };
  if (::XRefreshKeyboardMapping(&_xmapping);
    _xmapping.request == MappingKeyboard)
    kbd.grab_keys();
}
