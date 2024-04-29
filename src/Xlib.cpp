#include <stdexcept>
#include <X11/Xutil.h>
#include <Xlib.h>

::Display* some::Display::ptr;

void some::Display::init() {
  ptr = ::XOpenDisplay(nullptr);
  if (ptr == nullptr)
    throw std::runtime_error("Failed to open display");
}

void some::Display::deinit() {
  ::XCloseDisplay(ptr);
}

template<typename T>
some::Ev<T>::Ev() {
  for (auto& f : F)
    f = [](T&) { return [](T&) { }; };
}

template<typename T>
bool some::Ev<T>::next() noexcept { return ::XNextEvent(dpy.ptr, &xev) == 0; }

template<typename T>
void some::Ev<T>::sync() const noexcept { ::XSync(dpy.ptr, false); }

template<typename T>
some::Ev<T>::S
some::Ev<T>::call(T& data) const noexcept { return F[xev.type](data); }

template<typename T>
void some::Ev<T>::init_mapnotify(const S& f) noexcept {
  F[MapNotify] = [this, f](T& data) { return mapnotify(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::mapnotify(const S& f, T& data) noexcept {
  data[0] = 0;
  return f;
}

template<typename T>
void some::Ev<T>::init_unmapnotify(const S& f) noexcept {
  F[UnmapNotify] = [this, f](T& data) { return unmapnotify(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::unmapnotify(const S& f, T& data) noexcept {
  data[0] = 0;
  return f;
}

template<typename T>
void some::Ev<T>::init_clientmessage(const S& f) noexcept {
  F[ClientMessage] = [this, f](T& data) { return clientmessage(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::clientmessage(const S& f, T& data) noexcept {
  data[0] = 0;
  return f;
}

template<typename T>
void some::Ev<T>::init_configurenotify(const S& f) noexcept {
  F[ConfigureNotify] = [this, f](T& data) { return configurenotify(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::configurenotify(const S& f, T& data) noexcept {
  data[0] = xev.xconfigure.window;
  data[1] = xev.xconfigure.width;
  data[2] = xev.xconfigure.height;
  return f;
}

template<typename T>
void some::Ev<T>::init_maprequest(const S& f) noexcept {
  F[MapRequest] = [this, f](T& data) { return maprequest(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::maprequest(const S& f, T& data) noexcept {
  data[0] = xev.xmaprequest.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_configurerequest(const S& f) noexcept {
  F[ConfigureRequest] = 
    [this, f](T& data) { return configurerequest(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::configurerequest(const S& f, T& data) noexcept {
  const ::XConfigureRequestEvent& CONF { xev.xconfigurerequest };
  ::XWindowChanges wc {
    CONF.x, CONF.y, CONF.width, CONF.height,
    CONF.border_width, CONF.above, CONF.detail };
  ::XConfigureWindow(dpy.ptr, CONF.window, CONF.value_mask, &wc);
  return f;
}

template<typename T>
void some::Ev<T>::init_motionnotify(const S& f) noexcept {
  F[MotionNotify] = [this, f](T& data) { return motionnotify(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::motionnotify(const S& f, T& data) noexcept {
  const ::Window W { xev.xmotion.window };
  data[0] = W;
  return f;
}

template<typename T>
void some::Ev<T>::init_keypress(const S& f) noexcept {
  F[KeyPress] = [this, f](T& data) { return keypress(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::keypress(const S& f, T& data) noexcept {
  data[0] = xev.xkey.state;
  data[1] = xev.xkey.keycode;
  return f;
}

template<typename T>
void some::Ev<T>::init_btnpress(const S& f) noexcept {
  F[ButtonPress] = [this, f](T& data) { return btnpress(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::btnpress(const S& f, T& data) noexcept {
  data[0] = xev.xbutton.window;
  data[1] = xev.xbutton.state;
  data[2] = xev.xbutton.button;
  return f;
}

template<typename T>
void some::Ev<T>::init_enternotify(const S& f) noexcept {
  F[EnterNotify] = [this, f](T& data) { return enternotify(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::enternotify(const S& f, T& data) noexcept {
  data[0] = xev.xcrossing.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_propertynotify(const S& f) noexcept {
  F[PropertyNotify] = [this, f](T& data) { return propertynotify(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::propertynotify(const S& f, T& data) noexcept {
  data[0] = xev.xproperty.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_expose(const S& f) noexcept {
  F[Expose] = [this, f](T& data) { return expose(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::expose(const S& f, T& data) noexcept {
  data[0] = xev.xexpose.window;
  return f;
}

template class some::Ev<long[8]>;

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
