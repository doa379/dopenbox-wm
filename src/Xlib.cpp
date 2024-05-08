#include <stdexcept>
#include <X11/Xutil.h>
#include <Xlib.h>

::Display* some::Display::ptr;
bool some::Xlib::DefaultXError::xerror;

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
    f = [](T&) { return [](const T&) { }; };
}

template<typename T>
bool some::Ev<T>::next() noexcept { return ::XNextEvent(dpy.ptr, &xev) == 0; }

template<typename T>
void some::Ev<T>::sync() const noexcept { ::XSync(dpy.ptr, false); }

template<typename T>
some::Ev<T>::S
some::Ev<T>::call(T& data) const noexcept { return F[xev.type](data); }

template<typename T>
void some::Ev<T>::init_key(const S& f) noexcept {
  F[KeyPress] = [this, f](T& data) { return key(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::key(const S& f, T& data) const noexcept {
  data[0] = xev.xkey.state;
  data[1] = xev.xkey.keycode;
  return f;
}
/*
template<typename T>
void some::Ev<T>::init_button(const S& f) noexcept {
  F[ButtonPress] = [this, f](T& data) { return button(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::button(const S& f, T& data) const noexcept {
  data[0] = xev.xbutton.window;
  data[1] = xev.xbutton.state;
  data[2] = xev.xbutton.button;
  return f;
}

template<typename T>
void some::Ev<T>::init_motion(const S& f) noexcept {
  F[MotionNotify] = [this, f](T& data) { return motion(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::motion(const S& f, T& data) const noexcept {
  data[0] = xev.xmotion.window;
  data[1] = xev.xmotion.root;
  data[2] = xev.xmotion.subwindow;
  data[3] = xev.xmotion.time;
  data[4] = xev.xmotion.x;
  data[5] = xev.xmotion.y;
  data[6] = xev.xmotion.x_root;
  data[7] = xev.xmotion.y_root;
  data[8] = xev.xmotion.state;
  data[9] = xev.xmotion.is_hint;
  data[10] = xev.xmotion.same_screen;
  return f;
}

template<typename T>
void some::Ev<T>::init_crossing(const S& f) noexcept {
  F[EnterNotify] = [this, f](T& data) { return crossing(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::crossing(const S& f, T& data) const noexcept {
  data[0] = xev.xcrossing.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_focuschange(const S& f) noexcept {
  F[FocusIn] = F[FocusOut] = [this, f](T& data) { 
    return focuschange(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::focuschange(const S& f, T& data) const noexcept {
  data[0] = xev.xfocus.window;
  data[1] = xev.xfocus.mode;
  data[2] = xev.xfocus.detail;
  return f;
}

template<typename T>
void some::Ev<T>::init_expose(const S& f) noexcept {
  F[Expose] = [this, f](T& data) { return expose(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::expose(const S& f, T& data) const noexcept {
  data[0] = xev.xexpose.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_graphicsexpose(const S& f) noexcept {
  F[GraphicsExpose] = [this, f](T& data) { return graphicsexpose(f, data); };
}

template<typename T>
some::Ev<T>::S
some::Ev<T>::graphicsexpose(const S& f, T& data) const noexcept {
  data[0] = xev.xgraphicsexpose.drawable;
  data[1] = xev.xgraphicsexpose.x;
  data[2] = xev.xgraphicsexpose.y;
  data[3] = xev.xgraphicsexpose.width;
  data[4] = xev.xgraphicsexpose.height;
  data[5] = xev.xgraphicsexpose.count;
  data[6] = xev.xgraphicsexpose.major_code;
  data[7] = xev.xgraphicsexpose.minor_code;
  return f;
}

template<typename T>
void some::Ev<T>::init_noexpose(const S& f) noexcept {
  F[NoExpose] = [this, f](T& data) { return noexpose(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::noexpose(const S& f, T& data) const noexcept {
  data[0] = xev.xnoexpose.drawable;
  data[1] = xev.xnoexpose.major_code;
  data[2] = xev.xnoexpose.minor_code;
  return f;
}

template<typename T>
void some::Ev<T>::init_visibility(const S& f) noexcept {
  F[VisibilityNotify] = [this, f](T& data) { return visibility(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::visibility(const S& f, T& data) const noexcept {
  data[0] = xev.xvisibility.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_createwindow(const S& f) noexcept {
  F[CreateNotify] = [this, f](T& data) { return createwindow(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::createwindow(const S& f, T& data) const noexcept {
  data[0] = xev.xcreatewindow.parent;
  data[1] = xev.xcreatewindow.window;
  data[2] = xev.xcreatewindow.x;
  data[3] = xev.xcreatewindow.y;
  data[4] = xev.xcreatewindow.width;
  data[5] = xev.xcreatewindow.height;
  data[6] = xev.xcreatewindow.border_width;
  data[7] = xev.xcreatewindow.override_redirect;
  return f;
}

template<typename T>
void some::Ev<T>::init_destroywindow(const S& f) noexcept {
  F[DestroyNotify] = [this, f](T& data) { return destroywindow(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::destroywindow(const S& f, T& data) const noexcept {
  data[0] = xev.xdestroywindow.event;
  data[1] = xev.xdestroywindow.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_unmap(const S& f) noexcept {
  F[UnmapNotify] = [this, f](T& data) { return unmap(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::unmap(const S& f, T& data) const noexcept {
  data[0] = xev.xunmap.event;
  data[1] = xev.xunmap.window;
  data[2] = xev.xunmap.from_configure;
  return f;
}

template<typename T>
void some::Ev<T>::init_map(const S& f) noexcept {
  F[MapNotify] = [this, f](T& data) { return map(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::map(const S& f, T& data) const noexcept {
  data[0] = xev.xmap.event;
  data[1] = xev.xmap.window;
  data[2] = xev.xmap.override_redirect;
  return f;
}

template<typename T>
void some::Ev<T>::init_maprequest(const S& f) noexcept {
  F[MapRequest] = [this, f](T& data) { return maprequest(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::maprequest(const S& f, T& data) const noexcept {
  data[0] = xev.xmaprequest.parent;
  data[1] = xev.xmaprequest.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_reparent(const S& f) noexcept {
  F[ReparentNotify] = [this, f](T& data) { return reparent(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::reparent(const S& f, T& data) const noexcept {
  data[0] = xev.xreparent.event;
  data[1] = xev.xreparent.window;
  data[2] = xev.xreparent.parent;
  data[3] = xev.xreparent.x;
  data[4] = xev.xreparent.y;
  data[5] = xev.xreparent.override_redirect;
  return f;
}

template<typename T>
void some::Ev<T>::init_configure(const S& f) noexcept {
  F[ConfigureNotify] = [this, f](T& data) { return configure(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::configure(const S& f, T& data) const noexcept {
  data[0] = xev.xconfigure.event;
  data[1] = xev.xconfigure.window;
  data[2] = xev.xconfigure.x;
  data[3] = xev.xconfigure.y;
  data[4] = xev.xconfigure.width;
  data[5] = xev.xconfigure.height;
  data[6] = xev.xconfigure.border_width;
  data[7] = xev.xconfigure.above;
  data[8] = xev.xconfigure.override_redirect;
  return f;
}

template<typename T>
void some::Ev<T>::init_gravity(const S& f) noexcept {
  F[GravityNotify] = [this, f](T& data) { return gravity(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::gravity(const S& f, T& data) const noexcept {
  data[0] = xev.xgravity.event;
  data[1] = xev.xgravity.window;
  data[2] = xev.xgravity.x;
  data[3] = xev.xgravity.y;
  return f;
}

template<typename T>
void some::Ev<T>::init_resizerequest(const S& f) noexcept {
  F[ResizeRequest] = [this, f](T& data) { return resizerequest(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::resizerequest(const S& f, T& data) const noexcept {
  data[0] = xev.xresizerequest.window;
  data[1] = xev.xresizerequest.width;
  data[2] = xev.xresizerequest.height;
  return f;
}

template<typename T>
void some::Ev<T>::init_configurerequest(const S& f) noexcept {
  F[ConfigureRequest] = 
    [this, f](T& data) { return configurerequest(f, data); };
}

template<typename T>
some::Ev<T>::S
some::Ev<T>::configurerequest(const S& f, T& data) const noexcept {
  const ::XConfigureRequestEvent& CONF { xev.xconfigurerequest };
  ::XWindowChanges wc {
    CONF.x, CONF.y, CONF.width, CONF.height,
    CONF.border_width, CONF.above, CONF.detail };
  ::XConfigureWindow(dpy.ptr, CONF.window, CONF.value_mask, &wc);
  return f;
}

template<typename T>
void some::Ev<T>::init_circulate(const S& f) noexcept {
  F[CirculateNotify] = [this, f](T& data) { return circulate(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::circulate(const S& f, T& data) const noexcept {
  data[0] = xev.xcirculate.event;
  data[1] = xev.xcirculate.window;
  data[2] = xev.xcirculate.place;
  return f;
}

template<typename T>
void some::Ev<T>::init_circulaterequest(const S& f) noexcept {
  F[CirculateNotify] = [this, f](T& data) { 
    return circulaterequest(f, data); };
}

template<typename T>
some::Ev<T>::S
some::Ev<T>::circulaterequest(const S& f, T& data) const noexcept {
  data[0] = xev.xcirculaterequest.parent;
  data[1] = xev.xcirculaterequest.window;
  data[2] = xev.xcirculaterequest.place;
  return f;
}

template<typename T>
void some::Ev<T>::init_property(const S& f) noexcept {
  F[PropertyNotify] = [this, f](T& data) { return property(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::property(const S& f, T& data) const noexcept {
  data[0] = xev.xproperty.window;
  return f;
}

template<typename T>
void some::Ev<T>::init_selectionclear(const S& f) noexcept {
  F[SelectionClear] = [this, f](T& data) { return selectionclear(f, data); };
}

template<typename T>
some::Ev<T>::S
some::Ev<T>::selectionclear(const S& f, T& data) const noexcept {
  data[0] = xev.xselectionclear.window;
  data[1] = xev.xselectionclear.selection;
  data[2] = xev.xselectionclear.time;
  return f;
}

template<typename T>
void some::Ev<T>::init_selectionrequest(const S& f) noexcept {
  F[SelectionRequest] = [this, f](T& data) { 
    return selectionrequest(f, data); };
}

template<typename T>
some::Ev<T>::S
some::Ev<T>::selectionrequest(const S& f, T& data) const noexcept {
  data[0] = xev.xselectionrequest.owner;
  data[1] = xev.xselectionrequest.requestor;
  data[2] = xev.xselectionrequest.selection;
  data[3] = xev.xselectionrequest.target;
  data[4] = xev.xselectionrequest.property;
  data[5] = xev.xselectionrequest.time;
  return f;
}

template<typename T>
void some::Ev<T>::init_selection(const S& f) noexcept {
  F[SelectionNotify] = [this, f](T& data) { return selection(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::selection(const S& f, T& data) const noexcept {
  data[0] = xev.xselection.requestor;
  data[1] = xev.xselection.selection;
  data[2] = xev.xselection.target;
  data[3] = xev.xselection.property;
  data[4] = xev.xselection.time;
  return f;
}

template<typename T>
void some::Ev<T>::init_colormap(const S& f) noexcept {
  F[ColormapNotify] = [this, f](T& data) { return colormap(f, data); };
}

template<typename T>
some::Ev<T>::S
some::Ev<T>::colormap(const S& f, T& data) const noexcept {
  data[0] = xev.xcolormap.window;
  data[1] = xev.xcolormap.colormap;
  data[2] = *((bool*) &xev.xcolormap + sizeof(XColormapEvent) - sizeof(int));
  data[3] = xev.xcolormap.state;
  return f;
}
*/
template<typename T>
void some::Ev<T>::init_clientmessage(const S& f) noexcept {
  F[ClientMessage] = [this, f](T& data) { return clientmessage(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::clientmessage(const S& f, T& data) const noexcept {
  data[0] = xev.xclient.window;
  data[1] = xev.xclient.message_type;
  std::copy(xev.xclient.data.b, 
    // !!Warning flaw: This C union may not biject with C++ union!!
    xev.xclient.data.b + sizeof(union Data::Msg), data.msg.B);
  return f;
}
/*
template<typename T>
void some::Ev<T>::init_mapping(const S& f) noexcept {
  F[MappingNotify] = [this, f](T& data) { return mapping(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::mapping(const S& f, T& data) const noexcept {
  data[0] = xev.xmapping.request;
  data[1] = xev.xmapping.first_keycode;
  data[2] = xev.xmapping.count;
  return f;
}
*/
template<typename T>
void some::Ev<T>::init_keymap(const S& f) noexcept {
  F[KeymapNotify] = [this, f](T& data) { return keymap(f, data); };
}

template<typename T>
some::Ev<T>::S some::Ev<T>::keymap(const S& f, T& data) const noexcept {
  data[0] = xev.xkeymap.window;
  std::copy(xev.xkeymap.key_vector, 
    xev.xkeymap.key_vector + sizeof data.KEYMAP_VECTOR, data.KEYMAP_VECTOR);
  return f;
}

template class some::Ev<some::Data>;
using Win = some::Xlib::Win;

Win some::Xlib::Xlib::root() const noexcept {
  return ::XRootWindow(dpy.ptr, DefaultScreen(dpy.ptr));
}

Win 
some::Xlib::Xlib::create_window(const Win PARW, const int W, const int H) 
const noexcept {
  return ::XCreateSimpleWindow(dpy.ptr, W, 0, 0, W, H, 0, 0, 0);
}

void some::Xlib::Xlib::destroy_window(const Win WIN) const noexcept {
  ::XDestroyWindow(dpy.ptr, WIN);
}

void some::Xlib::Xlib::mapwindow(const Win WIN) const noexcept {
  ::XMapRaised(dpy.ptr, WIN);
}

void some::Xlib::Xlib::unmapwindow(const Win WIN) const noexcept {
  ::XUnmapWindow(dpy.ptr, WIN);
}

void some::Xlib::Xlib::set_bdrcolor(const Win WIN, const std::size_t COL) 
const noexcept {
  ::XSetWindowBorder(dpy.ptr, WIN, COL);
}

void 
some::Xlib::Xlib::set_bdrwidth(const Win WIN, const int PX) const noexcept {
  XSetWindowBorderWidth(dpy.ptr, WIN, PX);
}

void some::Xlib::Xlib::movewindow(const Win WIN, const int X, const int Y) 
const noexcept {
  ::XMoveWindow(dpy.ptr, WIN, X, Y);
}

void some::Xlib::Xlib::reparent(const Win WIN, const Win PARW, const int X, 
const int Y) 
const noexcept {
  ::XReparentWindow(dpy.ptr, WIN, PARW, X, Y);
}

void some::Xlib::Input::select(const Win WIN, const long MASK) const noexcept {
  ::XSelectInput(dpy.ptr, WIN, MASK);
}

void some::Xlib::Input::set_focus(const Win WIN) const noexcept {
  ::XSetInputFocus(dpy.ptr, WIN, RevertToPointerRoot, CurrentTime);
}

unsigned some::Xlib::Input::modmask() const noexcept {
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

void some::Xlib::Input::grab_key(const Win WIN, const int MOD, const int KEY) 
const noexcept {
  ::XGrabKey(dpy.ptr, ::XKeysymToKeycode(dpy.ptr, KEY), MOD, WIN, true, 
    GrabModeAsync, GrabModeAsync);
}

void 
some::Xlib::Input::ungrab_key(const Win WIN, const int MOD, const int KEY) 
const noexcept {
  ::XUngrabKey(dpy.ptr, ::XKeysymToKeycode(dpy.ptr, KEY), MOD, WIN);
}

void some::Xlib::Input::ungrab_allkey(const Win WIN) const noexcept {
  ungrab_key(WIN, AnyModifier, AnyKey);
}

void some::Xlib::Input::grab_btn(const Win WIN, const int MOD, const int BTN) 
const noexcept {
  static constexpr auto MASK { ButtonPressMask | ButtonReleaseMask };
  ::XGrabButton(dpy.ptr, BTN, MOD, WIN, false, MASK, GrabModeSync, 
  GrabModeSync, None, None);
}

void
some::Xlib::Input::ungrab_btn(const Win WIN, const int MOD, const int BTN) 
const noexcept {
  ::XUngrabButton(dpy.ptr, BTN, MOD, WIN);
}

void some::Xlib::Input::ungrab_pointer() const noexcept {
  ::XUngrabPointer(dpy.ptr, CurrentTime);
}

void some::Xlib::Input::warp_pointer(const Win WIN, const int X, const int Y) 
const noexcept {
  ::XWarpPointer(dpy.ptr, None, WIN, 0, 0, 0, 0, X, Y);
}

some::Xlib::QueryTree::QueryTree(const Win WIN) {
  Win root;
  Win parw;
  ::XQueryTree(dpy.ptr, WIN, &root, &parw, &wins, &n);
}

some::Xlib::QueryTree::~QueryTree() {
  ::XFree(wins);
}

std::vector<Win> some::Xlib::QueryTree::get() const noexcept {
  std::vector<Win> WINS;
  for (unsigned i { }; i < n; i++)
    WINS.emplace_back(wins[i]);

  return WINS;
}
