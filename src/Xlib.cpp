#include <stdexcept>
#include <iostream>
#include <X11/Xutil.h>
#include <Xlib.h>
#include <wm.h>

::Display* some::Display::ptr;

void some::Display::init() {
  ptr = ::XOpenDisplay(nullptr);
  if (ptr == nullptr)
    throw std::runtime_error("Failed to open display");
}

void some::Display::deinit() {
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

bool some::Ev::next() { 
  return ::XNextEvent(dpy.ptr, &xev) == 0;
}

void some::Ev::sync() { 
  ::XSync(dpy.ptr, false);
}

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
  const ::Window W { xev.xconfigure.window };
  const auto WIDTH { xev.xconfigure.width };
  const auto HEIGHT { xev.xconfigure.height };
  wm.configurenotify(W, WIDTH, HEIGHT);
}

void some::Ev::maprequest(Wm& wm) noexcept {
  std::cout << "EV: Map Request\n";
  ::XWindowAttributes wa;
  const ::Window W { xev.xmaprequest.window };
  if (::XGetWindowAttributes(dpy.ptr, W, &wa) == 0 || wa.override_redirect)
    wm.maprequest(W, wa.width, wa.height);
}

void some::Ev::configurerequest(Wm& wm) noexcept {
  std::cout << "EV: Config Request\n";
  ::Display* dpy { xev.xconfigurerequest.display };
  const ::XConfigureRequestEvent& CONF { xev.xconfigurerequest };
  ::XWindowChanges wc {
    CONF.x, CONF.y, CONF.width, CONF.height,
    CONF.border_width, CONF.above, CONF.detail };
  ::XConfigureWindow(dpy, CONF.window, CONF.value_mask, &wc);
}

void some::Ev::motionnotify(Wm& wm) noexcept {
  const ::Window W { xev.xmotion.window };
  std::cout << "EV: Motion\n";
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
  wm.btnpress(W, STATE, CODE);
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

::Window some::Xlib::Xlib::root() {
  return ::XRootWindow(dpy.ptr, DefaultScreen(dpy.ptr));
}

::XErrorHandler 
some::Xlib::Xlib::set_err(int (*handler)(::Display*, ::XErrorEvent*)) {
  return ::XSetErrorHandler(handler);
}

void some::Xlib::Xlib::mapwindow(const ::Window W) {
  ::XMapRaised(dpy.ptr, W);
}

void some::Xlib::Xlib::unmapwindow(const ::Window W) {
  ::XUnmapWindow(dpy.ptr, W);
}

void some::Xlib::Xlib::set_bdrcolor(const ::Window W, const std::size_t COL) {
  ::XSetWindowBorder(dpy.ptr, W, COL);
}

void 
some::Xlib::Xlib::set_bdrwidth(const ::Window W, const std::size_t WIDTH) {
  XSetWindowBorderWidth(dpy.ptr, W, WIDTH);
}

void some::Xlib::Xlib::movewindow(const ::Window W, const int X, const int Y) {
  ::XMoveWindow(dpy.ptr, W, X, Y);
}

void some::Xlib::Input::select(const ::Window W, const long MASK) {
  ::XSelectInput(dpy.ptr, W, MASK);
}

void some::Xlib::Input::set_focus(const ::Window W) {
  ::XSetInputFocus(dpy.ptr, W, RevertToPointerRoot, CurrentTime);
}

unsigned some::Xlib::Input::modmask() {
  ::XModifierKeymap* map { ::XGetModifierMapping(dpy.ptr) };
  unsigned numlockmask { };
  for (int k { 0 }; k < 8; k++)
    for (int j { 0 }; j < map->max_keypermod; j++)
      if (map->modifiermap[map->max_keypermod * k + j] == 
        ::XKeysymToKeycode(dpy.ptr, XK_Num_Lock))
        numlockmask = (1 << k);
  
  ::XFreeModifiermap(map);
  return ~(numlockmask | LockMask);
}

void some::Xlib::Input::grab_key(const ::Window W, const int MOD, 
const int KEY) {
  ::XGrabKey(dpy.ptr, ::XKeysymToKeycode(dpy.ptr, KEY), MOD, W, true, 
    GrabModeAsync, GrabModeAsync);
}

void some::Xlib::Input::ungrab_key(const ::Window W, const int MOD, 
const int KEY) {
  ::XUngrabKey(dpy.ptr, ::XKeysymToKeycode(dpy.ptr, KEY), MOD, W);
}

void some::Xlib::Input::ungrab_allkey(const ::Window W) {
  ungrab_key(W, AnyModifier, AnyKey);
}

void some::Xlib::Input::grab_btn(const ::Window W, const int MOD, 
const int BTN) {
  static constexpr auto MASK { ButtonPressMask | ButtonReleaseMask };
  ::XGrabButton(dpy.ptr, BTN, MOD, W, false, MASK, GrabModeSync, 
  GrabModeSync, None, None);
}

void some::Xlib::Input::ungrab_btn(const ::Window W, const int MOD, 
const int BTN) {
  ::XUngrabButton(dpy.ptr, BTN, MOD, W);
}

void some::Xlib::Input::ungrab_pointer() {
  ::XUngrabPointer(dpy.ptr, CurrentTime);
}

void some::Xlib::Input::warp_pointer(const ::Window W, const int X, 
const int Y) {
  ::XWarpPointer(dpy.ptr, None, W, 0, 0, 0, 0, X, Y);
}

some::Xlib::QueryTree::QueryTree(const ::Window W) {
  ::Window root;
  ::Window parw;
  ::XQueryTree(dpy.ptr, W, &root, &parw, &w, &n);
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

