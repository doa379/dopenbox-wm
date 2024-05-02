#include <iostream>
#include <wm.h>
#include <../config.h>

bool some::Wm::xerror;

some::Wm::Wm() : rootw { xlib.root() } {
  xlib.set_err(handler);
  input.select(rootw, Xlib::ROOTMASK);
  if (xerror)
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
  CALL[KILL] = [] { };
  CALL[SWFOCUS] = [] { };
  CALL[TOGGLEMODE] = [] { };
  CALL[PREVCLI] = [] { };
  CALL[NEXTCLI] = [] { };
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

  some::Xlib::QueryTree query { rootw };
  const auto W { query.get() };
  for (const auto& W_ : W)
    ;

  some::Xlib::Xinerama xinerama;
  std::cout << WMNAME << " initialized, have a nice day!\n";
}

some::Wm::~Wm() {
  input.set_focus(rootw);
  std::cout << WMNAME << " exit\n";
}

template<typename T>
void some::Recv<T>::key(const T&) {
  std::cout << "EV: Key Press\n";

}

template<typename T>
void some::Recv<T>::button(const T& DATA) {
  std::cout << "EV: Btn Press\n";
  const auto W { static_cast<::Window>(DATA[0]) };
  const auto STATE { static_cast<int>(DATA[1]) };
  const auto CODE { static_cast<int>(DATA[2]) };
  // ungrab_pointer();
}

template<typename T>
void some::Recv<T>::motion(const T& DATA) {
  std::cout << "EV: Motionnotify on Window " << DATA[0] << "\n";

}

template<typename T>
void some::Recv<T>::crossing(const T&) {
  std::cout << "EV: Enter Notify\n";

}

template<typename T>
void some::Recv<T>::expose(const T&) {
  std::cout << "EV: Expose\n";

}

template<typename T>
void some::Recv<T>::unmap(const T&) {
  std::cout << "EV: Unmapnotify\n";

}

template<typename T>
void some::Recv<T>::map(const T&) {
  std::cout << "EV: Mapnotify\n";

}

template<typename T>
void some::Recv<T>::maprequest(const T& DATA) {
  std::cout << "EV: Map Request\n";
  /*
  ::XWindowAttributes wa;
  if (::XGetWindowAttributes(dpy.ptr, W, &wa) == 0 || wa.override_redirect)
    wm.maprequest(W, wa.width, wa.height);
  */
}

template<typename T>
void some::Recv<T>::configure(const T& DATA) {
  std::cout << "EV: Configure Notify\n";
  if (DATA[0] == rootw) {

  }
}

template<typename T>
void some::Recv<T>::configurerequest(const T&) {
  std::cout << "EV: Config Request\n";

}

template<typename T>
void some::Recv<T>::property(const T&) {
  std::cout << "EV: Prop Notify\n";

}

template<typename T>
void some::Recv<T>::clientmessage(const T&) {
  std::cout << "EV: Client Message\n";

}

template struct some::Recv<some::Data>;
