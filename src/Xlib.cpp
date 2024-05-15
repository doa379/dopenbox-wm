#include <stdexcept>
#include <unistd.h>
#include <X11/Xutil.h>
#include <Xlib.h>

::Display* some::Display::ptr;
bool some::xlib::DefaultXError::xerror;

void some::Display::init() {
  ptr = ::XOpenDisplay(nullptr);
  if (ptr == nullptr)
    throw std::runtime_error("Failed to open display");
}

void some::Display::deinit() {
  ::XCloseDisplay(ptr);
}

some::Ev::Ev() {
  for (auto& f : F)
    f = [](data::T&) { return [](data::T const&) { }; };
}

bool some::Ev::next() noexcept { return ::XNextEvent(dpy.ptr, &xev) == 0; }

void some::Ev::sync() const noexcept { ::XSync(dpy.ptr, false); }

some::Ev::S some::Ev::call(data::T& data_var) const noexcept {
  return F[xev.type](data_var);
}

void some::Ev::init_key(S const& F) noexcept {
  this->F[KeyPress] = [&, F](data::T& data) { 
    return key(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::key(S const& F, data::L& data) const noexcept {
  data[0] = xev.xkey.state;
  data[1] = xev.xkey.keycode;
  return F;
}

void some::Ev::init_button(S const& F) noexcept {
  this->F[ButtonPress] = [&, F](data::T& data) { 
    return button(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::button(S const& F, data::L& data) const noexcept {
  data[0] = xev.xbutton.window;
  data[1] = xev.xbutton.state;
  data[2] = xev.xbutton.button;
  return F;
}

void some::Ev::init_motion(S const& F) noexcept {
  this->F[MotionNotify] = [&, F](data::T& data) { 
    return motion(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::motion(S const& F, data::L& data) const noexcept {
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
  return F;
}

void some::Ev::init_crossing(S const& F) noexcept {
  this->F[EnterNotify] = [&, F](data::T& data) { 
    return crossing(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::crossing(S const& F, data::L& data) const noexcept {
  data[0] = xev.xcrossing.window;
  return F;
}

void some::Ev::init_focuschange(S const& F) noexcept {
  this->F[FocusIn] = this->F[FocusOut] = [&, F](data::T& data) { 
    return focuschange(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::focuschange(S const& F, data::L& data) const noexcept {
  data[0] = xev.xfocus.window;
  data[1] = xev.xfocus.mode;
  data[2] = xev.xfocus.detail;
  return F;
}

void some::Ev::init_expose(S const& F) noexcept {
  this->F[Expose] = [&, F](data::T& data) { 
    return expose(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::expose(S const& F, data::L& data) const noexcept {
  data[0] = xev.xexpose.window;
  return F;
}

void some::Ev::init_graphicsexpose(S const& F) noexcept {
  this->F[GraphicsExpose] = [&, F](data::T& data) { 
    return graphicsexpose(F, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::graphicsexpose(S const& F, data::L& data) const noexcept {
  data[0] = xev.xgraphicsexpose.drawable;
  data[1] = xev.xgraphicsexpose.x;
  data[2] = xev.xgraphicsexpose.y;
  data[3] = xev.xgraphicsexpose.width;
  data[4] = xev.xgraphicsexpose.height;
  data[5] = xev.xgraphicsexpose.count;
  data[6] = xev.xgraphicsexpose.major_code;
  data[7] = xev.xgraphicsexpose.minor_code;
  return F;
}

void some::Ev::init_noexpose(S const& F) noexcept {
  this->F[NoExpose] = [&, F](data::T& data) { 
    return noexpose(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::noexpose(S const& F, data::L& data) const noexcept {
  data[0] = xev.xnoexpose.drawable;
  data[1] = xev.xnoexpose.major_code;
  data[2] = xev.xnoexpose.minor_code;
  return F;
}

void some::Ev::init_visibility(S const& F) noexcept {
  this->F[VisibilityNotify] = [&, F](data::T& data) { 
    return visibility(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::visibility(S const& F, data::L& data) const noexcept {
  data[0] = xev.xvisibility.window;
  return F;
}

void some::Ev::init_createwindow(S const& F) noexcept {
  this->F[CreateNotify] = [&, F](data::T& data) { 
    return createwindow(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::createwindow(S const& F, data::L& data) const noexcept {
  data[0] = xev.xcreatewindow.parent;
  data[1] = xev.xcreatewindow.window;
  data[2] = xev.xcreatewindow.x;
  data[3] = xev.xcreatewindow.y;
  data[4] = xev.xcreatewindow.width;
  data[5] = xev.xcreatewindow.height;
  data[6] = xev.xcreatewindow.border_width;
  data[7] = xev.xcreatewindow.override_redirect;
  return F;
}

void some::Ev::init_destroywindow(S const& F) noexcept {
  this->F[DestroyNotify] = [&, F](data::T& data) { 
    return destroywindow(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::destroywindow(S const& F, data::L& data) const noexcept {
  data[0] = xev.xdestroywindow.event;
  data[1] = xev.xdestroywindow.window;
  return F;
}

void some::Ev::init_unmap(S const& F) noexcept {
  this->F[UnmapNotify] = [&, F](data::T& data) { 
    return unmap(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::unmap(S const& F, data::L& data) const noexcept {
  data[0] = xev.xunmap.event;
  data[1] = xev.xunmap.window;
  data[2] = xev.xunmap.from_configure;
  return F;
}

void some::Ev::init_map(S const& F) noexcept {
  this->F[MapNotify] = [&, F](data::T& data) { 
    return map(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::map(S const& F, data::L& data) const noexcept {
  data[0] = xev.xmap.event;
  data[1] = xev.xmap.window;
  data[2] = xev.xmap.override_redirect;
  return F;
}

void some::Ev::init_maprequest(S const& F) noexcept {
  this->F[MapRequest] = [&, F](data::T& data) { 
    return maprequest(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::maprequest(S const& F, data::L& data) const noexcept {
  data[0] = xev.xmaprequest.parent;
  data[1] = xev.xmaprequest.window;
  return F;
}

void some::Ev::init_reparent(S const& F) noexcept {
  this->F[ReparentNotify] = [&, F](data::T& data) { 
    return reparent(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::reparent(S const& F, data::L& data) const noexcept {
  data[0] = xev.xreparent.event;
  data[1] = xev.xreparent.window;
  data[2] = xev.xreparent.parent;
  data[3] = xev.xreparent.x;
  data[4] = xev.xreparent.y;
  data[5] = xev.xreparent.override_redirect;
  return F;
}

void some::Ev::init_configure(S const& F) noexcept {
  this->F[ConfigureNotify] = [&, F](data::T& data) { 
    return configure(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::configure(S const& F, data::L& data) const noexcept {
  data[0] = xev.xconfigure.event;
  data[1] = xev.xconfigure.window;
  data[2] = xev.xconfigure.x;
  data[3] = xev.xconfigure.y;
  data[4] = xev.xconfigure.width;
  data[5] = xev.xconfigure.height;
  data[6] = xev.xconfigure.border_width;
  data[7] = xev.xconfigure.above;
  data[8] = xev.xconfigure.override_redirect;
  return F;
}

void some::Ev::init_gravity(S const& F) noexcept {
  this->F[GravityNotify] = [&, F](data::T& data) { 
    return gravity(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::gravity(S const& F, data::L& data) const noexcept {
  data[0] = xev.xgravity.event;
  data[1] = xev.xgravity.window;
  data[2] = xev.xgravity.x;
  data[3] = xev.xgravity.y;
  return F;
}

void some::Ev::init_resizerequest(S const& F) noexcept {
  this->F[ResizeRequest] = [&, F](data::T& data) { 
    return resizerequest(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::resizerequest(S const& F, data::L& data) const noexcept {
  data[0] = xev.xresizerequest.window;
  data[1] = xev.xresizerequest.width;
  data[2] = xev.xresizerequest.height;
  return F;
}

void some::Ev::init_configurerequest(S const& F) noexcept {
  this->F[ConfigureRequest] = 
    [&, F](data::T& data) { 
      return configurerequest(F, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::configurerequest(S const& F, data::L& data) const noexcept {
  const ::XConfigureRequestEvent& CONF { xev.xconfigurerequest };
  ::XWindowChanges wc {
    CONF.x, CONF.y, CONF.width, CONF.height,
    CONF.border_width, CONF.above, CONF.detail };
  ::XConfigureWindow(dpy.ptr, CONF.window, CONF.value_mask, &wc);
  return F;
}

void some::Ev::init_circulate(S const& F) noexcept {
  this->F[CirculateNotify] = [&, F](data::T& data) { 
    return circulate(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::circulate(S const& F, data::L& data) const noexcept {
  data[0] = xev.xcirculate.event;
  data[1] = xev.xcirculate.window;
  data[2] = xev.xcirculate.place;
  return F;
}

void some::Ev::init_circulaterequest(S const& F) noexcept {
  this->F[CirculateNotify] = [&, F](data::T& data) { 
    return circulaterequest(F, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::circulaterequest(S const& F, data::L& data) const noexcept {
  data[0] = xev.xcirculaterequest.parent;
  data[1] = xev.xcirculaterequest.window;
  data[2] = xev.xcirculaterequest.place;
  return F;
}

void some::Ev::init_property(S const& F) noexcept {
  this->F[PropertyNotify] = [&, F](data::T& data) { 
    return property(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::property(S const& F, data::L& data) const noexcept {
  data[0] = xev.xproperty.window;
  return F;
}

void some::Ev::init_selectionclear(S const& F) noexcept {
  this->F[SelectionClear] = [&, F](data::T& data) { 
    return selectionclear(F, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::selectionclear(S const& F, data::L& data) const noexcept {
  data[0] = xev.xselectionclear.window;
  data[1] = xev.xselectionclear.selection;
  data[2] = xev.xselectionclear.time;
  return F;
}

void some::Ev::init_selectionrequest(S const& F) noexcept {
  this->F[SelectionRequest] = [&, F](data::T& data) { 
    return selectionrequest(F, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::selectionrequest(S const& F, data::L& data) const noexcept {
  data[0] = xev.xselectionrequest.owner;
  data[1] = xev.xselectionrequest.requestor;
  data[2] = xev.xselectionrequest.selection;
  data[3] = xev.xselectionrequest.target;
  data[4] = xev.xselectionrequest.property;
  data[5] = xev.xselectionrequest.time;
  return F;
}

void some::Ev::init_selection(S const& F) noexcept {
  this->F[SelectionNotify] = [&, F](data::T& data) { 
    return selection(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::selection(S const& F, data::L& data) const noexcept {
  data[0] = xev.xselection.requestor;
  data[1] = xev.xselection.selection;
  data[2] = xev.xselection.target;
  data[3] = xev.xselection.property;
  data[4] = xev.xselection.time;
  return F;
}

void some::Ev::init_colormap(S const& F) noexcept {
  this->F[ColormapNotify] = [&, F](data::T& data) { 
    return colormap(F, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::colormap(S const& F, data::L& data) const noexcept {
  data[0] = xev.xcolormap.window;
  data[1] = xev.xcolormap.colormap;
  data[2] = *((bool*) &xev.xcolormap + sizeof(XColormapEvent) - sizeof(int));
  data[3] = xev.xcolormap.state;
  return F;
}

void some::Ev::init_clientmessage(S const& F) noexcept {
  this->F[ClientMessage] = [&, F](data::T& data) { 
    return clientmessage(F, std::get<data::Msg>(data)); };
}

some::Ev::S
some::Ev::clientmessage(S const& F, data::Msg& data) const noexcept {
  data[0] = xev.xclient.window;
  data[1] = xev.xclient.message_type;
  std::copy(xev.xclient.data.b, 
    // !!Warning flaw: This C union may not biject with C++ union!!
    xev.xclient.data.b + sizeof(data::Msg) - sizeof(data::L), data.B);
  return F;
}

void some::Ev::init_mapping(S const& F) noexcept {
  this->F[MappingNotify] = [&, F](data::T& data) { 
    return mapping(F, std::get<data::L>(data)); };
}

some::Ev::S some::Ev::mapping(S const& F, data::L& data) const noexcept {
  data[0] = xev.xmapping.request;
  data[1] = xev.xmapping.first_keycode;
  data[2] = xev.xmapping.count;
  return F;
}

void some::Ev::init_keymap(S const& F) noexcept {
  this->F[KeymapNotify] = [&, F](data::T& data) { 
    return keymap(F, std::get<data::Keymap>(data)); };
}

some::Ev::S some::Ev::keymap(S const& F, data::Keymap& data) const noexcept {
  data[0] = xev.xkeymap.window;
  std::copy(xev.xkeymap.key_vector, 
    xev.xkeymap.key_vector + sizeof(data::Keymap) - sizeof(data::L),
      data.KEYMAP_VECTOR);
  return F;
}

void some::Sys::spawn(char const CMD[]) const noexcept {
  if (::fork() == 0) {
    ::close(dpy.connection());
    ::setsid();
    ::system(CMD);
  }
}

using Win = some::xlib::Win;

Win some::xlib::Xlib::root() const noexcept {
  return ::XRootWindow(dpy.ptr, DefaultScreen(dpy.ptr));
}

Win 
some::xlib::Xlib::create_win(Win const PARW, int const W, int const H) 
const noexcept {
  return ::XCreateSimpleWindow(dpy.ptr, PARW, 0, 0, W, H, 0, 0, 0);
}

void some::xlib::Xlib::destroy_win(Win const WIN) const noexcept {
  ::XDestroyWindow(dpy.ptr, WIN);
}

void some::xlib::Xlib::map_win(Win const WIN) const noexcept {
  ::XMapRaised(dpy.ptr, WIN);
}

void some::xlib::Xlib::unmap_win(Win const WIN) const noexcept {
  ::XUnmapWindow(dpy.ptr, WIN);
}

void some::xlib::Xlib::set_winbg(Win const WIN, std::size_t const COL) 
const noexcept {
  ::XSetWindowBackground(dpy.ptr, WIN, COL);
  ::XClearWindow(dpy.ptr, WIN);
}

void some::xlib::Xlib::set_bdrcolor(Win const WIN, std::size_t const COL) 
const noexcept {
  ::XSetWindowBorder(dpy.ptr, WIN, COL);
}

void 
some::xlib::Xlib::set_bdrwidth(Win const WIN, int const PX) const noexcept {
  XSetWindowBorderWidth(dpy.ptr, WIN, PX);
}

void some::xlib::Xlib::move_win(Win const WIN, int const X, int const Y) 
const noexcept {
  ::XMoveWindow(dpy.ptr, WIN, X, Y);
}

void some::xlib::Xlib::repar_win(Win const WIN, Win const PARW, int const X, 
int const Y) 
const noexcept {
  ::XReparentWindow(dpy.ptr, WIN, PARW, X, Y);
}

void some::xlib::Input::select(Win const WIN, long const MASK) const noexcept {
  ::XSelectInput(dpy.ptr, WIN, MASK);
}

void some::xlib::Input::set_focus(Win const WIN) const noexcept {
  ::XSetInputFocus(dpy.ptr, WIN, RevertToPointerRoot, CurrentTime);
}

unsigned some::xlib::Input::modmask() const noexcept {
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

::KeyCode
some::xlib::Input::keysym_keycode(::KeySym const KSYM) const noexcept {
  return ::XKeysymToKeycode(dpy.ptr, KSYM);
}

::KeySym
some::xlib::Input::keycode_keysym(::KeyCode const KCODE) const noexcept {
  /* Depr. */
  return ::XKeycodeToKeysym(dpy.ptr, KCODE, 0);
}

void 
some::xlib::Input::grab_key(Win const WIN, int const MOD, int const KCODE) 
const noexcept {
  ::XGrabKey(dpy.ptr, KCODE, MOD, WIN, true, GrabModeAsync, GrabModeAsync);
}

void 
some::xlib::Input::ungrab_key(Win const WIN, int const MOD, int const KCODE) 
const noexcept {
  ::XUngrabKey(dpy.ptr, KCODE, MOD, WIN);
}

void some::xlib::Input::ungrab_allkey(Win const WIN) const noexcept {
  ungrab_key(WIN, AnyModifier, AnyKey);
}

void some::xlib::Input::grab_btn(Win const WIN, int const MOD, int const BTN) 
const noexcept {
  static constexpr auto MASK { ButtonPressMask | ButtonReleaseMask };
  ::XGrabButton(dpy.ptr, BTN, MOD, WIN, false, MASK, GrabModeSync, 
    GrabModeSync, None, None);
}

void
some::xlib::Input::ungrab_btn(Win const WIN, int const MOD, int const BTN) 
const noexcept {
  ::XUngrabButton(dpy.ptr, BTN, MOD, WIN);
}

void some::xlib::Input::ungrab_pointer() const noexcept {
  ::XUngrabPointer(dpy.ptr, CurrentTime);
}

void some::xlib::Input::warp_pointer(Win const WIN, int const X, int const Y) 
const noexcept {
  ::XWarpPointer(dpy.ptr, None, WIN, 0, 0, 0, 0, X, Y);
}

some::xlib::WinAttr::WinAttr(Win const WIN) {
  if (const auto STATUS { ::XGetWindowAttributes(dpy.ptr, WIN, &wa) };
      STATUS == BadDrawable || STATUS == BadWindow)
    throw std::runtime_error("Failed to retrieve window attributes");
}

some::xlib::QueryTree::QueryTree(Win const WIN) {
  Win root;
  Win parw;
  ::XQueryTree(dpy.ptr, WIN, &root, &parw, &wins, &n);
}

some::xlib::QueryTree::~QueryTree() {
  ::XFree(wins);
}

std::vector<Win> some::xlib::QueryTree::get() const noexcept {
  std::vector<Win> WINS;
  for (unsigned i { }; i < n; i++)
    WINS.emplace_back(wins[i]);

  return WINS;
}
