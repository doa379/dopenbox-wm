#include <iostream>
#include <wm.h>
#include <../config.h>

some::Wm::Wm() : rootw { xlib.root() } {
  xlib::DefaultXError error; 
  input.select(rootw, xlib::ROOTMASK);
  if (error.get())
    throw std::runtime_error("Initialization error (another wm running?)");

  input.ungrab_allkey(rootw);
  const auto MASK { input.modmask() };
  for (const auto& K : KBD)
    input.grab_key(rootw, K.mod & MASK, K.key);
    
  CALL[WK0] = [] { };
  CALL[WK1] = [] { };
  CALL[WK2] = [] { };
  CALL[WK3] = [] { };
  CALL[WK4] = [] { };
  CALL[WK5] = [] { };
  CALL[WK6] = [] { };
  CALL[WK7] = [] { };
  CALL[WK8] = [] { };
  CALL[WK9] = [] { };
  CALL[MON0] = [] { };
  CALL[MON1] = [] { };
  CALL[MON2] = [] { };
  CALL[MON3] = [] { };
  CALL[MON4] = [] { };
  CALL[MON5] = [] { };
  CALL[MON6] = [] { };
  CALL[MON7] = [] { };
  CALL[MON8] = [] { };
  CALL[MON9] = [] { };
  CALL[UNMAPALL] = [] { };
  CALL[REMAPALL] = [] { };
  CALL[KILL] = [&] { kill_client(); };
  CALL[SWFOCUS] = [] { };
  CALL[TOGGLEMODE] = [] { };
  CALL[PREVCLI] = [&] { prev_client(); };
  CALL[NEXTCLI] = [&] { next_client(); };
  CALL[SELTOGGLE] = [] { };
  CALL[SELCLEAR] = [] { };
  CALL[MOVEUP] = [] { };
  CALL[MOVEDOWN] = [] { };
  CALL[MOVELEFT] = [] { };
  CALL[MOVERIGHT] = [] { };
  CALL[RESIZEVINC] = [] { };
  CALL[RESIZEVDEC] = [] { };
  CALL[RESIZEHDEC] = [] { };
  CALL[RESIZEHINC] = [] { };
  CALL[QUIT] = []{ };
  CALL[SELECT] = [] { };
  CALL[RESIZE] = [] { };
  CALL[STATE] = [] { };

  for (auto i { 0 }; i < NWKS; i++)
    WK.emplace_back(Wk { .n = i });

  some::xlib::QueryTree query { rootw };
  const auto W { query.get() };
  for (const auto W_ : W)
    ;

  some::xlib::Xinerama xinerama;
  std::cout << WMNAME << " initialized, have a nice day!\n";
}

some::Wm::~Wm() {
  input.set_focus(rootw);
  std::cout << WMNAME << " exit\n";
}

void some::Wm::focus() {
  if (wk[1]->client[1] < wk[1]->C.end()) {
  // set unfocus

  }
  //set focus
  //input.grab_btn();
  //input.grab_key();
}

void some::Wm::spawn() {
  
}

void some::Wm::prev_client() {
  std::prev(wk[1]->client[1]);
}

void some::Wm::next_client() {
  std::next(wk[1]->client[1]);

}

void some::Wm::kill_client() {
  (void) wk[1]->client[1];

}



void some::Recv::key(const data::L& DATA) {
  std::cout << "EV: Key Press\n";

}

void some::Recv::button(const data::L& DATA) {
  std::cout << "EV: Btn Press\n";
  const auto WIN { static_cast<xlib::Win>(DATA[0]) };
  const auto STATE { static_cast<int>(DATA[1]) };
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
  xlib::WinAttr wa { WIN };
  if (wa.isvalid()) {
    const auto SIZE { wa.size() };
    const xlib::Win PARW { 
      xlib.create_window(WIN, std::get<0>(SIZE), std::get<1>(SIZE)) };
    input.select(PARW, xlib::PARMASK);
    xlib.reparent(WIN, PARW, 0, 0);
    xlib.mapwindow(PARW);
    xlib.set_bdrwidth(PARW, BDRW_PX);
    //xlib.set_bdrcolor(PARW, BDRCOL);
    const Client C { 
      .w = PARW
    };
    
    wk[1]->client[0] = wk[1]->client[1];
    wk[1]->C.emplace_back(C);
    wk[1]->client[1] = wk[1]->C.end() - 1;
    focus();
  }
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

void some::Recv::clientmessage(const data::L&) {
  std::cout << "EV: Client Message\n";
}
