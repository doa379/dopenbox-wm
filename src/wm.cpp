#include <iostream>
#include <csignal>
#include <wm.h>
#include <../config.h>

some::Wm::Wm() : rootw { xlib.root() } {
  xlib::DefaultXError error; 
  input.select(rootw, xlib::ROOTMASK);
  if (error.get())
    throw std::runtime_error("Initialization error (another wm running?)");

  input.ungrab_allkey(rootw);
  const auto MASK { input.modmask() };
  for (const auto& K : wmconf::KBD) {
    const auto KCODE { input.keysym2keycode(K.sym) };
    KCODE_KSYM[KCODE] = K.sym;
    input.grab_key(rootw, K.mod & MASK, KCODE);
  }
    
  CALL[wmconf::WK0] = [] { };
  CALL[wmconf::WK1] = [] { };
  CALL[wmconf::WK2] = [] { };
  CALL[wmconf::WK3] = [] { };
  CALL[wmconf::WK4] = [] { };
  CALL[wmconf::WK5] = [] { };
  CALL[wmconf::WK6] = [] { };
  CALL[wmconf::WK7] = [] { };
  CALL[wmconf::WK8] = [] { };
  CALL[wmconf::WK9] = [] { };
  CALL[wmconf::MON0] = [] { };
  CALL[wmconf::MON1] = [] { };
  CALL[wmconf::MON2] = [] { };
  CALL[wmconf::MON3] = [] { };
  CALL[wmconf::MON4] = [] { };
  CALL[wmconf::MON5] = [] { };
  CALL[wmconf::MON6] = [] { };
  CALL[wmconf::MON7] = [] { };
  CALL[wmconf::MON8] = [] { };
  CALL[wmconf::MON9] = [] { };
  CALL[wmconf::UNMAPALL] = [] { };
  CALL[wmconf::REMAPALL] = [] { };
  CALL[wmconf::KILL] = [&] { kill_client(); };
  CALL[wmconf::SWFOCUS] = [] { };
  CALL[wmconf::TOGGLEMODE] = [] { };
  CALL[wmconf::PREVCLI] = [&] { prev_client(); };
  CALL[wmconf::NEXTCLI] = [&] { next_client(); };
  CALL[wmconf::SELTOGGLE] = [] { };
  CALL[wmconf::SELCLEAR] = [] { };
  CALL[wmconf::MOVEUP] = [] { };
  CALL[wmconf::MOVEDOWN] = [] { };
  CALL[wmconf::MOVELEFT] = [] { };
  CALL[wmconf::MOVERIGHT] = [] { };
  CALL[wmconf::RESIZEVINC] = [] { };
  CALL[wmconf::RESIZEVDEC] = [] { };
  CALL[wmconf::RESIZEHDEC] = [] { };
  CALL[wmconf::RESIZEHINC] = [] { };
  CALL[wmconf::QUIT] = []{ std::raise(SIGINT); };
  CALL[wmconf::SELECT] = [] { };
  CALL[wmconf::RESIZE] = [] { };
  CALL[wmconf::STATE] = [] { };

  for (auto i { 0 }; i < wmconf::NWKS; i++)
    WK.emplace_back(Wk { .n = i });

  wk[0] = wk[1] = WK.begin();

  some::xlib::QueryTree query { rootw };
  const auto W { query.get() };
  for (const auto W_ : W)
    ;

  some::xlib::Xinerama xinerama;
  std::cout << wmconf::WMNAME << " initialized, have a nice day!\n";
}

some::Wm::~Wm() {
  input.set_focus(rootw);
  std::cout << wmconf::WMNAME << " exit\n";
}

void some::Wm::unfocus(const xlib::Win WIN) {
  const auto MASK { input.modmask() };
  for (const auto& B : wmconf::BTN)
    input.ungrab_btn(WIN, B.mod & MASK, B.sym);

  //delprop_active(wk[1]->client[1]->w);
  xlib.set_bdrcolor(WIN, 0);
}

void some::Wm::focus(const xlib::Win WIN) {
  input.set_focus(WIN);
  const auto MASK { input.modmask() };
  for (const auto& B : wmconf::BTN)
    input.grab_btn(WIN, B.mod & MASK, B.sym);
  
  xlib.set_bdrcolor(WIN, wmconf::BDRCOLOR);
}

void some::Wm::prev_client() {
  if (wk[1]->client[1] < wk[1]->C.cend())
    unfocus(wk[1]->client[1]->win);

  if (const auto CURR { std::prev(wk[1]->client[1]) };
    CURR < wk[1]->C.cend()) {
    wk[1]->client[0] = wk[1]->client[1];
    wk[1]->client[1] = CURR;
    focus(CURR->win);
  }
}

void some::Wm::next_client() {
  if (wk[1]->client[1] < wk[1]->C.cend())
    unfocus(wk[1]->client[1]->win);

  if (const auto CURR { std::next(wk[1]->client[1]) };
    CURR < wk[1]->C.cend()) {
    wk[1]->client[0] = wk[1]->client[1];
    wk[1]->client[1] = CURR;
    focus(CURR->win);
  }
}

void some::Wm::kill_client() {
  (void) wk[1]->client[1];

}

void some::Recv::key(const data::L& DATA) {
  std::cout << "EV: Key Press\n";
  const auto MASK { input.modmask() };
  const auto KMOD { static_cast<int>(DATA[0]) & MASK };
  const auto KCODE { static_cast<int>(DATA[1]) };
  for (const auto& K : wmconf::KBD)
    if (K.mod == KMOD && K.sym == KCODE_KSYM[KCODE]) {
      if (K.cmd) {
        Sys sys;
        sys.spawn(K.cmd);
      } else
        CALL[K.call]();
      
      break;
    }
}

void some::Recv::button(const data::L& DATA) {
  std::cout << "EV: Btn Press\n";
  const auto MASK { input.modmask() };
  const auto WIN { static_cast<xlib::Win>(DATA[0]) };
  const auto KMOD { static_cast<int>(DATA[1]) & MASK };
  const auto CODE { static_cast<int>(DATA[2]) };
  input.ungrab_pointer();
}

void some::Recv::motion(const data::L& DATA) {
  const auto WIN { static_cast<xlib::Win>(DATA[0]) };
  if (WIN == rootw) {

  }
}

void some::Recv::crossing(const data::L&) {
  std::cout << "EV: Enter Notify\n";

}

void some::Recv::expose(const data::L&) {
  std::cout << "EV: Expose\n";

}

void some::Recv::unmap(const data::L&) {
  std::cout << "EV: Unmapnotify\n";

}

void some::Recv::map(const data::L&) {
  std::cout << "EV: Mapnotify\n";

}

void some::Recv::maprequest(const data::L& DATA) {
  std::cout << "EV: Map Request\n";
  const auto PARW { static_cast<xlib::Win>(DATA[0]) };
  const auto WIN { static_cast<xlib::Win>(DATA[1]) };

  /*
  ::XWindowAttributes wa;
  if (::XGetWindowAttributes(dpy.ptr, W, &wa) == 0 || wa.override_redirect)
    wm.maprequest(W, wa.width, wa.height);
  */
  try {
    xlib::WinAttr wa { WIN };
    const auto SIZE { wa.size() };
    const xlib::Win PARW { 
      xlib.create_window(WIN, std::get<0>(SIZE), std::get<1>(SIZE)) };
    input.select(PARW, xlib::PARMASK);
    xlib.reparent(WIN, PARW, 0, 0);
    xlib.mapwindow(PARW);
    xlib.set_bdrwidth(PARW, wmconf::BDRW_PX);
    const Client C { 
      .win = PARW
    };
    
    if (wk[1]->client[1] < wk[1]->C.cend())
      unfocus(wk[1]->client[1]->win);

    wk[1]->client[0] = wk[1]->client[1];
    wk[1]->C.emplace_back(C);
    wk[1]->client[1] = wk[1]->C.end() - 1;
    focus(wk[1]->client[1]->win);
  } catch (...) { }
}

void some::Recv::configure(const data::L& DATA) {
  std::cout << "EV: Configure Notify\n";
  const auto WIN { static_cast<xlib::Win>(DATA[1]) }; 
  if (WIN == rootw) {
    const auto X { static_cast<int>(DATA[2]) };
    const auto Y { static_cast<int>(DATA[3]) };
    const auto W { static_cast<int>(DATA[4]) };
    const auto H { static_cast<int>(DATA[5]) };
  }
}

void some::Recv::configurerequest(const data::L&) {
  std::cout << "EV: Config Request\n";
}

void some::Recv::property(const data::L&) {
  std::cout << "EV: Prop Notify\n";
}

void some::Recv::clientmessage(const data::Msg& DATA) {
  std::cout << "EV: Client Message\n";
}
