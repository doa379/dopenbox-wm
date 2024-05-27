#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

#include "../inc/Xlib.h"

::Display* some::Display::ptr;
bool some::xlib::DefaultXError::xerror;

void
some::Display::init() {
  ptr = ::XOpenDisplay(nullptr);
  if (ptr == nullptr)
    throw std::runtime_error("Failed to open display");
}

void 
some::Display::deinit()
noexcept {
  ::XCloseDisplay(ptr);
}

int
some::Display::connection() 
const noexcept { 
  return ConnectionNumber(ptr);
}

some::Ev::Ev() {
  for (auto& f : F)
    f = [](data::T&) { return [](data::T const&) { }; };
}

bool
some::Ev::next()
noexcept {
  return ::XNextEvent(dpy.ptr, &xev) == 0;
}

void
some::Ev::sync()
const noexcept {
  ::XSync(dpy.ptr, false);
}

some::Ev::S
some::Ev::call(data::T& data_var) 
const noexcept {
  return F[xev.type](data_var);
}

void
some::Ev::init_key(S const& f)
noexcept {
  F[KeyPress] = [&, f](data::T& data) { 
    return key(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::key(S const& f, data::L& data) 
const noexcept {
  data[0] = xev.xkey.state;
  data[1] = xev.xkey.keycode;
  return f;
}

void
some::Ev::init_button(S const& f)
noexcept {
  F[ButtonPress] = [&, f](data::T& data) { 
    return button(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::button(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xbutton.window;
  data[1] = xev.xbutton.state;
  data[2] = xev.xbutton.button;
  return f;
}

void
some::Ev::init_motion(S const& f)
noexcept {
  F[MotionNotify] = [&, f](data::T& data) { 
    return motion(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::motion(S const& f, data::L& data)
const noexcept {
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

void
some::Ev::init_crossing(S const& f)
noexcept {
  F[EnterNotify] = [&, f](data::T& data) { 
    return crossing(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::crossing(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xcrossing.window;
  return f;
}

void
some::Ev::init_focuschange(S const& f)
noexcept {
  F[FocusIn] = F[FocusOut] = [&, f](data::T& data) { 
    return focuschange(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::focuschange(S const& f, data::L& data) 
const noexcept {
  data[0] = xev.xfocus.window;
  data[1] = xev.xfocus.mode;
  data[2] = xev.xfocus.detail;
  return f;
}

void
some::Ev::init_expose(S const& f)
noexcept {
  F[Expose] = [&, f](data::T& data) { 
    return expose(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::expose(S const& f, data::L& data) 
const noexcept {
  data[0] = xev.xexpose.window;
  return f;
}

void
some::Ev::init_graphicsexpose(S const& f)
noexcept {
  F[GraphicsExpose] = [&, f](data::T& data) { 
    return graphicsexpose(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::graphicsexpose(S const& f, data::L& data)
const noexcept {
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

void
some::Ev::init_noexpose(S const& f) 
noexcept {
  F[NoExpose] = [&, f](data::T& data) { 
    return noexpose(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::noexpose(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xnoexpose.drawable;
  data[1] = xev.xnoexpose.major_code;
  data[2] = xev.xnoexpose.minor_code;
  return f;
}

void
some::Ev::init_visibility(S const& f)
noexcept {
  F[VisibilityNotify] = [&, f](data::T& data) { 
    return visibility(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::visibility(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xvisibility.window;
  return f;
}

void
some::Ev::init_createwindow(S const& f)
noexcept {
  F[CreateNotify] = [&, f](data::T& data) { 
    return createwindow(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::createwindow(S const& f, data::L& data)
const noexcept {
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

void
some::Ev::init_destroywindow(S const& f)
noexcept {
  F[DestroyNotify] = [&, f](data::T& data) { 
    return destroywindow(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::destroywindow(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xdestroywindow.event;
  data[1] = xev.xdestroywindow.window;
  return f;
}

void
some::Ev::init_unmap(S const& f)
noexcept {
  F[UnmapNotify] = [&, f](data::T& data) { 
    return unmap(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::unmap(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xunmap.event;
  data[1] = xev.xunmap.window;
  data[2] = xev.xunmap.from_configure;
  return f;
}

void
some::Ev::init_map(S const& f)
noexcept {
  F[MapNotify] = [&, f](data::T& data) { 
    return map(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::map(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xmap.event;
  data[1] = xev.xmap.window;
  data[2] = xev.xmap.override_redirect;
  return f;
}

void
some::Ev::init_maprequest(S const& f)
noexcept {
  F[MapRequest] = [&, f](data::T& data) { 
    return maprequest(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::maprequest(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xmaprequest.parent;
  data[1] = xev.xmaprequest.window;
  return f;
}

void
some::Ev::init_reparent(S const& f)
noexcept {
  F[ReparentNotify] = [&, f](data::T& data) { 
    return reparent(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::reparent(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xreparent.event;
  data[1] = xev.xreparent.window;
  data[2] = xev.xreparent.parent;
  data[3] = xev.xreparent.x;
  data[4] = xev.xreparent.y;
  data[5] = xev.xreparent.override_redirect;
  return f;
}

void
some::Ev::init_configure(S const& f)
noexcept {
  F[ConfigureNotify] = [&, f](data::T& data) { 
    return configure(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::configure(S const& f, data::L& data)
const noexcept {
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

void
some::Ev::init_gravity(S const& f) noexcept {
  F[GravityNotify] = [&, f](data::T& data) { 
    return gravity(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::gravity(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xgravity.event;
  data[1] = xev.xgravity.window;
  data[2] = xev.xgravity.x;
  data[3] = xev.xgravity.y;
  return f;
}

void
some::Ev::init_resizerequest(S const& f)
noexcept {
  F[ResizeRequest] = [&, f](data::T& data) { 
    return resizerequest(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::resizerequest(S const& f, data::L& data) 
const noexcept {
  data[0] = xev.xresizerequest.window;
  data[1] = xev.xresizerequest.width;
  data[2] = xev.xresizerequest.height;
  return f;
}

void
some::Ev::init_configurerequest(S const& f) 
noexcept {
  F[ConfigureRequest] = [&, f](data::T& data) { 
    return configurerequest(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::configurerequest(S const& f, data::L& data) 
const noexcept {
  ::XConfigureRequestEvent const& conf { 
    xev.xconfigurerequest };
  ::XWindowChanges wc {
    conf.x, conf.y, conf.width, conf.height,
    conf.border_width, conf.above, conf.detail };
  ::XConfigureWindow(dpy.ptr, conf.window, conf.value_mask, &wc);
  return f;
}

void
some::Ev::init_circulate(S const& f)
noexcept {
  F[CirculateNotify] = [&, f](data::T& data) { 
    return circulate(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::circulate(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xcirculate.event;
  data[1] = xev.xcirculate.window;
  data[2] = xev.xcirculate.place;
  return f;
}

void
some::Ev::init_circulaterequest(S const& f)
noexcept {
  F[CirculateNotify] = [&, f](data::T& data) { 
    return circulaterequest(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::circulaterequest(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xcirculaterequest.parent;
  data[1] = xev.xcirculaterequest.window;
  data[2] = xev.xcirculaterequest.place;
  return f;
}

void
some::Ev::init_property(S const& f)
noexcept {
  F[PropertyNotify] = [&, f](data::T& data) { 
    return property(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::property(S const& f, data::L& data) 
const noexcept {
  data[0] = xev.xproperty.window;
  return f;
}

void
some::Ev::init_selectionclear(S const& f)
noexcept {
  F[SelectionClear] = [&, f](data::T& data) { 
    return selectionclear(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::selectionclear(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xselectionclear.window;
  data[1] = xev.xselectionclear.selection;
  data[2] = xev.xselectionclear.time;
  return f;
}

void
some::Ev::init_selectionrequest(S const& f)
noexcept {
  F[SelectionRequest] = [&, f](data::T& data) { 
    return selectionrequest(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::selectionrequest(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xselectionrequest.owner;
  data[1] = xev.xselectionrequest.requestor;
  data[2] = xev.xselectionrequest.selection;
  data[3] = xev.xselectionrequest.target;
  data[4] = xev.xselectionrequest.property;
  data[5] = xev.xselectionrequest.time;
  return f;
}

void
some::Ev::init_selection(S const& f)
noexcept {
  F[SelectionNotify] = [&, f](data::T& data) { 
    return selection(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::selection(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xselection.requestor;
  data[1] = xev.xselection.selection;
  data[2] = xev.xselection.target;
  data[3] = xev.xselection.property;
  data[4] = xev.xselection.time;
  return f;
}

void
some::Ev::init_colormap(S const& f)
noexcept {
  F[ColormapNotify] = [&, f](data::T& data) { 
    return colormap(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::colormap(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xcolormap.window;
  data[1] = xev.xcolormap.colormap;
  data[2] = *((bool*) &xev.xcolormap + 
    sizeof(XColormapEvent) - sizeof(int));
  data[3] = xev.xcolormap.state;
  return f;
}

void
some::Ev::init_clientmessage(S const& f)
noexcept {
  F[ClientMessage] = [&, f](data::T& data) { 
    return clientmessage(f, std::get<data::Msg>(data)); };
}

some::Ev::S
some::Ev::clientmessage(S const& f, data::Msg& data) 
const noexcept {
  data[0] = xev.xclient.window;
  data[1] = xev.xclient.message_type;
  std::copy(xev.xclient.data.b, 
    // !!Warning flaw: This C union may not biject with C++ union!!
    xev.xclient.data.b + sizeof(data::Msg) - 
      sizeof(data::L), data.B);
  return f;
}

void 
some::Ev::init_mapping(S const& f)
noexcept {
  F[MappingNotify] = [&, f](data::T& data) { 
    return mapping(f, std::get<data::L>(data)); };
}

some::Ev::S
some::Ev::mapping(S const& f, data::L& data)
const noexcept {
  data[0] = xev.xmapping.request;
  data[1] = xev.xmapping.first_keycode;
  data[2] = xev.xmapping.count;
  return f;
}

void
some::Ev::init_keymap(S const& f)
noexcept {
  F[KeymapNotify] = [&, f](data::T& data) { 
    return keymap(f, std::get<data::Keymap>(data)); };
}

some::Ev::S
some::Ev::keymap(S const& f, data::Keymap& data)
const noexcept {
  data[0] = xev.xkeymap.window;
  std::copy(xev.xkeymap.key_vector, 
    xev.xkeymap.key_vector + sizeof(data::Keymap) - sizeof(data::L),
      data.KEYMAP_VECTOR);
  return f;
}

void
some::Sys::spawn(char const CMD[])
const noexcept {
  if (::fork() == 0) {
    ::close(dpy.connection());
    ::setsid();
    ::system(CMD);
  }
}

some::xlib::DefaultXError::DefaultXError() 
noexcept { 
  ::XSetErrorHandler(handler);
}

bool
some::xlib::DefaultXError::get()
const noexcept { 
  return xerror;
}

int
some::xlib::DefaultXError::handler(::Display*, 
::XErrorEvent* xev)
noexcept {
  xerror = (xev->error_code == BadAccess ||
    xev->error_code == BadWindow);
  return 0;
};

using Win = some::xlib::Win;

Win
some::xlib::Xlib::root()
const noexcept {
  return ::XRootWindow(dpy.ptr, DefaultScreen(dpy.ptr));
}

int
some::xlib::Xlib::dpy_width()
const noexcept {
  return DisplayWidth(dpy.ptr, DefaultScreen(dpy.ptr));
};

int
some::xlib::Xlib::dpy_height() 
const noexcept {
  return DisplayHeight(dpy.ptr, DefaultScreen(dpy.ptr));
};

int
some::xlib::Xlib::depth()
const noexcept {
  return DefaultDepth(dpy.ptr, DefaultScreen(dpy.ptr));
};

Win 
some::xlib::Xlib::create_win(Win const win, int const w, 
int const h)
const noexcept {
  return ::XCreateSimpleWindow(dpy.ptr, win, 0, 0, w, h, 
    0, 0, 0);
}

void
some::xlib::Xlib::destroy_win(Win const win)
const noexcept {
  ::XDestroyWindow(dpy.ptr, win);
}

void
some::xlib::Xlib::map_win(Win const win) 
const noexcept {
  ::XMapRaised(dpy.ptr, win);
}

void
some::xlib::Xlib::unmap_win(Win const win)
const noexcept {
  ::XUnmapWindow(dpy.ptr, win);
}

void 
some::xlib::Xlib::set_winbg(Win const win, 
std::size_t const col) 
const noexcept {
  ::XSetWindowBackground(dpy.ptr, win, col);
  ::XClearWindow(dpy.ptr, win);
}

void
some::xlib::Xlib::set_bdrcolor(Win const win, 
std::size_t const col)
const noexcept {
  ::XSetWindowBorder(dpy.ptr, win, col);
}

void 
some::xlib::Xlib::set_bdrwidth(Win const win, int const px)
const noexcept {
  ::XSetWindowBorderWidth(dpy.ptr, win, px);
}

void
some::xlib::Xlib::move_win(Win const win, int const x,
int const y)
const noexcept {
  ::XMoveWindow(dpy.ptr, win, x, y);
}

void
some::xlib::Xlib::repar_win(Win const win, Win const parw,
int const x, int const y)
const noexcept {
  ::XReparentWindow(dpy.ptr, win, parw, x, y);
}

::GC
some::xlib::Xlib::default_gc()
const noexcept {
  return DefaultGC(dpy.ptr, DefaultScreen(dpy.ptr));
}

void
some::xlib::Input::select(Win const win, long const mask) 
const noexcept {
  ::XSelectInput(dpy.ptr, win, mask);
}

void
some::xlib::Input::set_focus(Win const win)
const noexcept {
  ::XSetInputFocus(dpy.ptr, win, RevertToPointerRoot, 
    CurrentTime);
}

unsigned
some::xlib::Input::modmask()
const noexcept {
  ::XModifierKeymap* map { ::XGetModifierMapping(dpy.ptr) };
  unsigned numlockmask { };
  for (int k { }; k < 8; k++)
    for (int j { }; j < map->max_keypermod; j++)
      if (map->modifiermap[map->max_keypermod * k + j] == 
        ::XKeysymToKeycode(dpy.ptr, XK_Num_Lock))
        numlockmask = (1 << k);
  
  ::XFreeModifiermap(map);
  return ~(numlockmask | LockMask);
}

::KeyCode
some::xlib::Input::keysym_keycode(::KeySym const ksym) 
const noexcept {
  return ::XKeysymToKeycode(dpy.ptr, ksym);
}

::KeySym
some::xlib::Input::keycode_keysym(::KeyCode const kcode)
const noexcept {
  /* Depr. */
  return ::XKeycodeToKeysym(dpy.ptr, kcode, 0);
}

void 
some::xlib::Input::grab_key(Win const win, int const mod, 
int const kcode) 
const noexcept {
  ::XGrabKey(dpy.ptr, kcode, mod, win, true, 
    GrabModeAsync, GrabModeAsync);
}

void 
some::xlib::Input::ungrab_key(Win const win, int const mod,
int const kcode) 
const noexcept {
  ::XUngrabKey(dpy.ptr, kcode, mod, win);
}

void
some::xlib::Input::ungrab_allkey(Win const win)
const noexcept {
  ungrab_key(win, AnyModifier, AnyKey);
}

void
some::xlib::Input::grab_btn(Win const win, int const mod,
int const btn) 
const noexcept {
  static auto constexpr MASK { 
    ButtonPressMask | ButtonReleaseMask };
  ::XGrabButton(dpy.ptr, btn, mod, win, false, MASK, 
      GrabModeSync, GrabModeSync, None, None);
}

void
some::xlib::Input::ungrab_btn(Win const win, int const mod,
int const btn) 
const noexcept {
  ::XUngrabButton(dpy.ptr, btn, mod, win);
}

void
some::xlib::Input::ungrab_pointer() 
const noexcept {
  ::XUngrabPointer(dpy.ptr, CurrentTime);
}

void
some::xlib::Input::warp_pointer(Win const win, int const x,
int const y) 
const noexcept {
  ::XWarpPointer(dpy.ptr, None, win, 0, 0, 0, 0, x, y);
}

some::xlib::WinAttr::WinAttr(Win const win) {
  if (auto const status { 
    ::XGetWindowAttributes(dpy.ptr, win, &wa) };
      status == BadDrawable || status == BadWindow)
    throw std::runtime_error("Failed to retrieve window attributes");
}

bool
some::xlib::WinAttr::override_redirect() 
const noexcept { 
  return wa.override_redirect;
}

std::pair<int, int> 
some::xlib::WinAttr::size()
const noexcept { 
  return { wa.width, wa.height };
}

std::pair<int, int>
some::xlib::WinAttr::pos() 
const noexcept {
  return { wa.x, wa.y };
}

some::xlib::QueryTree::QueryTree(Win const win) {
  Win root;
  Win parw;
  ::XQueryTree(dpy.ptr, win, &root, &parw, &wins, &n);
}

some::xlib::QueryTree::~QueryTree() {
  ::XFree(wins);
}

std::vector<Win>
some::xlib::QueryTree::get() 
const noexcept {
  std::vector<Win> WINS;
  for (unsigned i { }; i < n; i++)
    WINS.emplace_back(wins[i]);

  return WINS;
}

some::xlib::Xinerama::Xinerama() {
  if (!XineramaIsActive(dpy.ptr))
    throw std::runtime_error("Xinerama is not active");
  
  screeninfo = ::XineramaQueryScreens(dpy.ptr, &n);
}

some::xlib::Xinerama::~Xinerama() {
  ::XFree(screeninfo);
}

int
some::xlib::Xinerama::number()
const noexcept {
  return n;
}

std::pair<std::pair<int, int>, std::pair<int, int>>
some::xlib::Xinerama::query(std::size_t const n)
const noexcept {
  return { { screeninfo[n].x_org, screeninfo[n].y_org },
    { screeninfo[n].width, screeninfo[n].height } };
}

some::xlib::Prop::Prop() noexcept :
  wm_protocols { 
    ::XInternAtom(dpy.ptr, "WM_PROTOCOLS", false) },
  wm_name { ::XInternAtom(dpy.ptr, "WM_NAME", false) },
  wm_delete_window { 
    ::XInternAtom(dpy.ptr, "WM_DELETE_WINDOW", false) },
  wm_state { ::XInternAtom(dpy.ptr, "WM_STATE", false) },
  wm_take_focus { 
    ::XInternAtom(dpy.ptr, "WM_TAKE_FOCUS", false) },
  wm_icon_name {
    ::XInternAtom(dpy.ptr, "WM_ICON_NAME", false) },
  net_supported { 
    ::XInternAtom(dpy.ptr, "_NET_SUPPORTED", false) },
  net_wm_state { 
    ::XInternAtom(dpy.ptr, "_NET_WM_STATE", false) },
  net_wm_name { 
    ::XInternAtom(dpy.ptr, "_NET_WM_NAME", false) },
  net_wm_window_opacity {
    ::XInternAtom(dpy.ptr, "_NET_WM_WINDOW_OPACITY", 
      false) },
  net_active_window { 
    ::XInternAtom(dpy.ptr, "_NET_ACTIVE_WINDOW", false) },
  net_wm_state_fullscreen {
    ::XInternAtom(dpy.ptr, "_NET_WM_STATE_FULLSCREEN", 
      false) },
  net_wm_window_type { 
    ::XInternAtom(dpy.ptr, "_NET_WM_WINDOW_TYPE", false) },
  net_wm_window_type_dialog {
    ::XInternAtom(dpy.ptr, "_NET_WM_WINDOW_TYPE_DIALOG", 
      false) },
  net_client_list { 
    ::XInternAtom(dpy.ptr, "_NET_CLIENT_LIST", false) },
  net_number_of_desktops {
    ::XInternAtom(dpy.ptr, "_NET_NUMBER_OF_DESKTOPS",
      false) },
  net_wm_desktop {
    ::XInternAtom(dpy.ptr, "_NET_WM_DESKTOP", false) },
  net_current_desktop {
    ::XInternAtom(dpy.ptr, "_NET_CURRENT_DESKTOP", 
      false) },
  net_showing_desktop {
    ::XInternAtom(dpy.ptr, "_NET_SHOWING_DESKTOP", 
      false) },
  net_wm_icon { 
    ::XInternAtom(dpy.ptr, "_NET_WM_ICON", false) },
  net_wm_icon_name {
    ::XInternAtom(dpy.ptr, "_NET_WM_ICON_NAME", false) }
{ }

void
some::xlib::Prop::change_state(Win const win) 
const noexcept {
  ::XChangeProperty(dpy.ptr, win, wm_state, 
    XA_WINDOW, 
    32, PropModeReplace, (unsigned char*) &win, 1);
}

some::xlib::draw::Font::Font(char const FONT[]) :
  fn { ::XLoadQueryFont(dpy.ptr, FONT) } {
  if (fn == nullptr)
    throw std::runtime_error("No font found");

  scent = fn->ascent + fn->descent;
}

some::xlib::draw::Font::~Font() {
  ::XFreeFont(dpy.ptr, fn);
}

int
some::xlib::draw::Font::get_scent()
const noexcept {
  return scent;
}

int
some::xlib::draw::Font::get_ascent() 
const noexcept {
  return fn->ascent;
}

int 
some::xlib::draw::Font::get_descent() 
const noexcept {
  return fn->descent;
}

int
some::xlib::draw::Font::text_width(char const S[])
const noexcept {
  return ::XTextWidth(fn, S, ::strlen(S));
}

int
some::xlib::draw::Font::text_width16(wchar_t const S[]) 
const noexcept {
  // Not impl
  //return ::XTextWidth16(fn, S, ::wcslen(S));
  return 0;
}

some::xlib::draw::Gc::Gc(Win const win)
noexcept :
  gc { ::XCreateGC(dpy.ptr, win, 0, NULL) } {
  ::XSetLineAttributes(dpy.ptr, gc, 1, LineSolid, CapButt,
    JoinMiter);
}

some::xlib::draw::Gc::~Gc() {
  ::XFreeGC(dpy.ptr, gc);
}

::GC
some::xlib::draw::Gc::get()
const noexcept {
  return gc;
}

some::xlib::draw::Pixmap::Pixmap(Win const win,
int const w, int const h, int const d) 
noexcept :
  drawable { ::XCreatePixmap(dpy.ptr, win, w, h, d) } {

}

some::xlib::draw::Pixmap::~Pixmap() {
  ::XFreePixmap(dpy.ptr, drawable);
}

void
some::xlib::draw::Pixmap::fill(Win const win, 
::GC const gc, std::size_t col, int const x, int const y, 
int const w, int const h)
const noexcept {
  ::XSetForeground(dpy.ptr, gc, col);
  ::XFillRectangle(dpy.ptr, win, gc, x, y, w, h);
}

void
some::xlib::draw::Pixmap::draw_string(char const S[], 
Win const win, ::GC const gc, std::size_t const col,
int const x, int const y)
const noexcept {
  ::XSetForeground(dpy.ptr, gc, col);
  ::XDrawString(dpy.ptr, win, gc, x, y, S, strlen(S));
}
