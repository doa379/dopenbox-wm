#include <stdexcept>
#include <iostream>
#include <X11/Xutil.h>
#include <Xlib.h>
#include <wm.h>

::Display* some::Display::ptr { ::XOpenDisplay(nullptr) };

some::Display::Display() {
  if (ptr ==  nullptr)
    throw std::runtime_error("Failed to open display");
}

some::Display::~Display() {
  ::XCloseDisplay(ptr);
}

some::Ev::Ev(Wm& wm) {
  for (auto& f : F)
    f = [] { };
  
  F[MapNotify] = [this, &wm] { mapnotify(wm); };
  F[UnmapNotify] = [this, &wm] { unmapnotify(wm); };
  F[ClientMessage] = [this, &wm] { clientmessage(wm); };
  F[ConfigureNotify] = [this, &wm] { configurenotify(wm); };
  F[MapRequest] = [this, &wm] { maprequest(wm); };
  F[ConfigureRequest] = [this, &wm] { configurerequest(wm); };
  F[MotionNotify] = [this, &wm] { motionnotify(wm); };
  F[KeyPress] = [this, &wm] { keypress(wm); };
  F[ButtonPress] = [this, &wm] { btnpress(wm); };
  F[EnterNotify] = [this, &wm] { enternotify(wm); };
  F[PropertyNotify] = [this, &wm] { propertynotify(wm); };
  F[Expose] = [this, &wm] { expose(wm); };
}

some::Ev::~Ev() {

};

void some::Ev::mapnotify(Wm& wm) noexcept {
  std::cout << "EV: Mapnotify\n";
  wm.mapnotify();
}

void some::Ev::unmapnotify(Wm& wm) noexcept {
  std::cout << "EV: Unmapnotify\n";
  const ::Window W { xev.xunmap.window };
  wm.unmapnotify();
}

void some::Ev::clientmessage(Wm& wm) noexcept {
  std::cout << "EV: Client Message\n";
  const ::Window W { xev.xclient.window };
  wm.mapnotify();
}

void some::Ev::configurenotify(Wm& wm) noexcept {
  std::cout << "EV: Configure Notify\n";
  ::Display* dpy { xev.xconfigure.display };
  const ::Window W { xev.xconfigure.window };
  const auto WIDTH { xev.xconfigure.width };
  const auto HEIGHT { xev.xconfigure.height };
  // want to reconfigure root window
  if (W == ::XRootWindow(dpy, DefaultScreen(dpy))) {
    wm.configurenotify();
  }
}

void some::Ev::maprequest(Wm& wm) noexcept {
  std::cout << "EV: Map Request\n";
  ::XWindowAttributes wa;
  ::Display* dpy { xev.xmaprequest.display };
  const ::Window W { xev.xmaprequest.window };
  if (::XGetWindowAttributes(dpy, W, &wa) == 0 || wa.override_redirect) {
    wm.maprequest();
  }
}

void some::Ev::configurerequest(Wm& wm) noexcept {
  std::cout << "EV: Config Request\n";
  ::Display* dpy { xev.xconfigurerequest.display };
  const ::XConfigureRequestEvent& CONF { xev.xconfigurerequest };
  ::XWindowChanges wc = {
    CONF.x, CONF.y, CONF.width, CONF.height,
    CONF.border_width, CONF.above, CONF.detail };
  ::XConfigureWindow(dpy, CONF.window, CONF.value_mask, &wc);
}

void some::Ev::motionnotify(Wm& wm) noexcept {
  const ::Window W { xev.xmotion.window };
  std::cout << "Motion\n";
  wm.motionnotify();
}

void some::Ev::keypress(Wm& wm) noexcept {
  std::cout << "EV: Key Press\n";
  const auto STATE { xev.xkey.state };
  const auto CODE { xev.xkey.keycode };
  wm.keypress();
}

void some::Ev::btnpress(Wm& wm) noexcept {
  std::cout << "EV: Btn Press\n";
  ::Display* dpy { xev.xbutton.display };
  const ::Window W { xev.xbutton.window };
  const auto STATE { xev.xbutton.state };
  const auto CODE { xev.xbutton.button };
  ::XUngrabPointer(dpy, CurrentTime);
  wm.btnpress();
}

void some::Ev::enternotify(Wm& wm) noexcept {
  std::cout << "EV: Enter Notify\n";
  const ::Window W { xev.xcrossing.window };
  wm.enternotify();
}

void some::Ev::propertynotify(Wm& wm) noexcept {
  std::cout << "EV: Prop Notify\n";
  const ::Window W { xev.xproperty.window };
  wm.propertynotify();
}

void some::Ev::expose(Wm& wm) noexcept {
  std::cout << "EV: Expose\n";
  const ::Window W { xev.xexpose.window };
  wm.expose();
}

some::Xlib::Input::Input(::Display* dpy) : dpy { dpy } {

}

some::Xlib::Input::~Input() {

}

unsigned some::Xlib::Input::modmask() {
  ::XModifierKeymap* map { ::XGetModifierMapping(dpy) };
  unsigned numlockmask { };
  for (int k { 0 }; k < 8; k++)
    for (int j { 0 }; j < map->max_keypermod; j++)
      if (map->modifiermap[map->max_keypermod * k + j] == 
        ::XKeysymToKeycode(dpy, XK_Num_Lock))
        numlockmask = (1 << k);
  
  ::XFreeModifiermap(map);
  return ~(numlockmask | LockMask);
}

void some::Xlib::Input::grab_key(const ::Window W, const int MOD, 
const int KEY) {
  ::XGrabKey(dpy, ::XKeysymToKeycode(dpy, KEY), MOD, W, true, 
    GrabModeAsync, GrabModeAsync);
}

void some::Xlib::Input::ungrab_key(const ::Window W, const int MOD, 
const int KEY) {
  ::XUngrabKey(dpy, ::XKeysymToKeycode(dpy, KEY), MOD, W);
}

void some::Xlib::Input::grab_btn(const ::Window W, const int MOD, 
const int BTN) {
  static const long MASK { ButtonPressMask | ButtonReleaseMask };
  ::XGrabButton(dpy, BTN, MOD, W, false, MASK, GrabModeSync, 
  GrabModeSync, None, None);
}

void some::Xlib::Input::ungrab_btn(const ::Window W, const int MOD, 
const int BTN) {
  ::XUngrabButton(dpy, BTN, MOD, W);
}

void some::Xlib::Input::warp_pointer(const ::Window W, const int X, 
const int Y) {
  ::XWarpPointer(dpy, None, W, 0, 0, 0, 0, X, Y);
}

some::Xlib::QueryTree::QueryTree(::Display* dpy, const ::Window W) {
  ::Window root;
  ::Window parw;
  ::XQueryTree(dpy, W, &root, &parw, &w, &n);
}

some::Xlib::QueryTree::~QueryTree() {
  ::XFree(w);
}

std::vector<::Window> some::Xlib::QueryTree::get() {
  std::vector<::Window> W;
  for (unsigned i { }; i < n; i++)
    W.emplace_back(w[i]);

  return W;
}

