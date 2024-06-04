#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <utility>

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
    f = { [] { }, [](Data const&) { } };
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

void
some::Ev::call() 
const noexcept {
  auto const ts { F[xev.type] };
  std::get<T>(ts)();
  std::get<S>(ts)(data);
}

void
some::Ev::init_key(S const& f)
noexcept {
  F[KeyPress] = { [this] { key(); }, f };

}

void
some::Ev::key() 
noexcept {
  data[0] = xev.xkey.state;
  data[1] = xev.xkey.keycode;
  /*
  std::memcpy(data.buffer, 
    &xev.xkey + sizeof(::XAnyEvent) - sizeof(::Window),
    sizeof xev.xkey - sizeof(::XAnyEvent) + 
      sizeof(::Window));
  */
}

void
some::Ev::init_button(S const& f)
noexcept {
  F[ButtonPress] =  { [this] { button(); }, f };
}

void
some::Ev::button()
noexcept {
  data[0] = xev.xbutton.window;
  data[1] = xev.xbutton.state;
  data[2] = xev.xbutton.button;
}

void
some::Ev::init_motion(S const& f)
noexcept {
  F[MotionNotify] = { [this] { motion(); }, f };
}

void
some::Ev::motion()
noexcept {
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
}

void
some::Ev::init_crossing(S const& f)
noexcept {
  F[EnterNotify] = { [this] { crossing(); }, f };
}

void
some::Ev::crossing()
noexcept {
  data[0] = xev.xcrossing.window;
}

void
some::Ev::init_focuschange(S const& f)
noexcept {
  F[FocusIn] = F[FocusOut] = { 
    [this] { focuschange(); }, f
  };
}

void
some::Ev::focuschange() 
noexcept {
  data[0] = xev.xfocus.window;
  data[1] = xev.xfocus.mode;
  data[2] = xev.xfocus.detail;
}

void
some::Ev::init_expose(S const& f)
noexcept {
  F[Expose] = { [this] { expose(); }, f };
}

void
some::Ev::expose() 
noexcept {
  data[0] = xev.xexpose.window;
}

void
some::Ev::init_graphicsexpose(S const& f)
noexcept {
  F[GraphicsExpose] = { [this] { graphicsexpose(); }, f };
}

void
some::Ev::graphicsexpose()
noexcept {
  data[0] = xev.xgraphicsexpose.drawable;
  data[1] = xev.xgraphicsexpose.x;
  data[2] = xev.xgraphicsexpose.y;
  data[3] = xev.xgraphicsexpose.width;
  data[4] = xev.xgraphicsexpose.height;
  data[5] = xev.xgraphicsexpose.count;
  data[6] = xev.xgraphicsexpose.major_code;
  data[7] = xev.xgraphicsexpose.minor_code;
}

void
some::Ev::init_noexpose(S const& f) 
noexcept {
  F[NoExpose] = { [this] { noexpose(); }, f };
}

void
some::Ev::noexpose()
noexcept {
  data[0] = xev.xnoexpose.drawable;
  data[1] = xev.xnoexpose.major_code;
  data[2] = xev.xnoexpose.minor_code;
}

void
some::Ev::init_visibility(S const& f)
noexcept {
  F[VisibilityNotify] = { [this] { visibility(); }, f };
}

void
some::Ev::visibility()
noexcept {
  data[0] = xev.xvisibility.window;
}

void
some::Ev::init_createwindow(S const& f)
noexcept {
  F[CreateNotify] = { [this] { createwindow(); }, f };
}

void
some::Ev::createwindow()
noexcept {
  data[0] = xev.xcreatewindow.parent;
  data[1] = xev.xcreatewindow.window;
  data[2] = xev.xcreatewindow.x;
  data[3] = xev.xcreatewindow.y;
  data[4] = xev.xcreatewindow.width;
  data[5] = xev.xcreatewindow.height;
  data[6] = xev.xcreatewindow.border_width;
  data[7] = xev.xcreatewindow.override_redirect;
}

void
some::Ev::init_destroywindow(S const& f)
noexcept {
  F[DestroyNotify] = { [this] { destroywindow(); }, f };
}

void
some::Ev::destroywindow()
noexcept {
  data[0] = xev.xdestroywindow.event;
  data[1] = xev.xdestroywindow.window;
}

void
some::Ev::init_unmap(S const& f)
noexcept {
  F[UnmapNotify] = { [this] { unmap(); }, f };
}

void
some::Ev::unmap()
noexcept {
  data[0] = xev.xunmap.event;
  data[1] = xev.xunmap.window;
  data[2] = xev.xunmap.from_configure;
}

void
some::Ev::init_map(S const& f)
noexcept {
  F[MapNotify] = { [this] { map(); }, f };
}

void
some::Ev::map()
noexcept {
  data[0] = xev.xmap.event;
  data[1] = xev.xmap.window;
  data[2] = xev.xmap.override_redirect;
}

void
some::Ev::init_maprequest(S const& f)
noexcept {
  F[MapRequest] = { [this] { maprequest(); }, f };
}

void
some::Ev::maprequest()
noexcept {
  data[0] = xev.xmaprequest.parent;
  data[1] = xev.xmaprequest.window;
}

void
some::Ev::init_reparent(S const& f)
noexcept {
  F[ReparentNotify] = { [this] { reparent(); }, f };
}

void
some::Ev::reparent()
noexcept {
  data[0] = xev.xreparent.event;
  data[1] = xev.xreparent.window;
  data[2] = xev.xreparent.parent;
  data[3] = xev.xreparent.x;
  data[4] = xev.xreparent.y;
  data[5] = xev.xreparent.override_redirect;
}

void
some::Ev::init_configure(S const& f)
noexcept {
  F[ConfigureNotify] = { [this] { configure(); }, f };
}

void
some::Ev::configure()
noexcept {
  data[0] = xev.xconfigure.event;
  data[1] = xev.xconfigure.window;
  data[2] = xev.xconfigure.x;
  data[3] = xev.xconfigure.y;
  data[4] = xev.xconfigure.width;
  data[5] = xev.xconfigure.height;
  data[6] = xev.xconfigure.border_width;
  data[7] = xev.xconfigure.above;
  data[8] = xev.xconfigure.override_redirect;
}

void
some::Ev::init_gravity(S const& f) noexcept {
  F[GravityNotify] = { [this] { gravity(); }, f };
}

void
some::Ev::gravity()
noexcept {
  data[0] = xev.xgravity.event;
  data[1] = xev.xgravity.window;
  data[2] = xev.xgravity.x;
  data[3] = xev.xgravity.y;
}

void
some::Ev::init_resizerequest(S const& f)
noexcept {
  F[ResizeRequest] = { [this] { resizerequest(); }, f };
}

void
some::Ev::resizerequest() 
noexcept {
  data[0] = xev.xresizerequest.window;
  data[1] = xev.xresizerequest.width;
  data[2] = xev.xresizerequest.height;
}

void
some::Ev::init_configurerequest(S const& f) 
noexcept {
  F[ConfigureRequest] = { 
    [this] { configurerequest(); }, f
  };
}

void
some::Ev::configurerequest() 
const noexcept {
  ::XConfigureRequestEvent const& conf { 
    xev.xconfigurerequest };
  ::XWindowChanges wc {
    conf.x, conf.y, conf.width, conf.height,
    conf.border_width, conf.above, conf.detail };
  ::XConfigureWindow(dpy.ptr, conf.window, conf.value_mask, &wc);
}

void
some::Ev::init_circulate(S const& f)
noexcept {
  F[CirculateNotify] = { [this] { circulate(); }, f };
}

void
some::Ev::circulate()
noexcept {
  data[0] = xev.xcirculate.event;
  data[1] = xev.xcirculate.window;
  data[2] = xev.xcirculate.place;
}

void
some::Ev::init_circulaterequest(S const& f)
noexcept {
  F[CirculateNotify] = { 
    [this] { circulaterequest(); }, f
  };
}

void
some::Ev::circulaterequest()
noexcept {
  data[0] = xev.xcirculaterequest.parent;
  data[1] = xev.xcirculaterequest.window;
  data[2] = xev.xcirculaterequest.place;
}

void
some::Ev::init_property(S const& f)
noexcept {
  F[PropertyNotify] = { [this] { property(); }, f };
}

void
some::Ev::property() 
noexcept {
  data[0] = xev.xproperty.window;
}

void
some::Ev::init_selectionclear(S const& f)
noexcept {
  F[SelectionClear] = { [this] { selectionclear(); }, f };
}

void
some::Ev::selectionclear()
noexcept {
  data[0] = xev.xselectionclear.window;
  data[1] = xev.xselectionclear.selection;
  data[2] = xev.xselectionclear.time;
}

void
some::Ev::init_selectionrequest(S const& f)
noexcept {
  F[SelectionRequest] = {
    [this] { selectionrequest(); }, f
  };
}

void
some::Ev::selectionrequest()
noexcept {
  data[0] = xev.xselectionrequest.owner;
  data[1] = xev.xselectionrequest.requestor;
  data[2] = xev.xselectionrequest.selection;
  data[3] = xev.xselectionrequest.target;
  data[4] = xev.xselectionrequest.property;
  data[5] = xev.xselectionrequest.time;
}

void
some::Ev::init_selection(S const& f)
noexcept {
  F[SelectionNotify] = { [this] { selection(); }, f };
}

void
some::Ev::selection()
noexcept {
  data[0] = xev.xselection.requestor;
  data[1] = xev.xselection.selection;
  data[2] = xev.xselection.target;
  data[3] = xev.xselection.property;
  data[4] = xev.xselection.time;
}

void
some::Ev::init_colormap(S const& f)
noexcept {
  F[ColormapNotify] = { [this] { colormap(); }, f };
}

void
some::Ev::colormap()
noexcept {
  data[0] = xev.xcolormap.window;
  data[1] = xev.xcolormap.colormap;
  data[2] = *((bool*) &xev.xcolormap + 
    sizeof(XColormapEvent) - sizeof(int));
  //data[2] = xev.xcolormap.new;
  data[3] = xev.xcolormap.state;
}

void
some::Ev::init_clientmessage(S const& f)
noexcept {
  F[ClientMessage] = { [this] { clientmessage(); }, f };
}

void
some::Ev::clientmessage() 
noexcept {
  data[0] = xev.xclient.window;
  data[1] = xev.xclient.message_type;
  std::memcpy(&data.msg, &xev.xclient.data, 
    sizeof data.msg);
}

void 
some::Ev::init_mapping(S const& f)
noexcept {
  F[MappingNotify] = { [this] { mapping(); }, f };
}

void
some::Ev::mapping()
noexcept {
  data[0] = xev.xmapping.request;
  data[1] = xev.xmapping.first_keycode;
  data[2] = xev.xmapping.count;
}

void
some::Ev::init_keymap(S const& f)
noexcept {
  F[KeymapNotify] = { [this] { keymap(); }, f };
}

void
some::Ev::keymap()
noexcept {
  data[0] = xev.xkeymap.window;
  std::memcpy(data.KEYMAP_VECTOR, xev.xkeymap.key_vector,
    sizeof data.KEYMAP_VECTOR);
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
some::xlib::Xlib::iconify_win(Win const win)
const noexcept {
  ::XIconifyWindow(dpy.ptr, win, DefaultScreen(dpy.ptr));
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
some::xlib::draw::Font::text_width(char const S[], 
std::size_t const len)
const noexcept {
  return ::XTextWidth(fn, S, len);
}

int
some::xlib::draw::Font::text_width16(wchar_t const S[],
std::size_t const len)
const noexcept {
  ::XChar2b const s { static_cast<unsigned char>(S[0]), 
    static_cast<unsigned char>(S[1]) };
  return ::XTextWidth16(fn, &s, len);
}

some::xlib::draw::Gc::Gc(Win const win)
noexcept :
gc { ::XCreateGC(dpy.ptr, win, 0, NULL) } {
  ::XSetLineAttributes(dpy.ptr, gc, 1, LineSolid, CapButt,
    JoinMiter);
}

some::xlib::draw::Gc::Gc(Gc&& gc)
noexcept :
gc { std::exchange(gc.gc, nullptr) }
{ }

some::xlib::draw::Gc&
some::xlib::draw::Gc::operator=(Gc&& gc)
noexcept {
  this->gc = gc.gc;
  return *this;
}

some::xlib::draw::Gc::~Gc() {
  if (gc)
    ::XFreeGC(dpy.ptr, gc);
}

::GC
some::xlib::draw::Gc::get()
const noexcept {
  return gc;
}

void
some::xlib::draw::Gc::set_fg(std::size_t const col)
const noexcept {
  ::XSetForeground(dpy.ptr, gc, col);
}

void
some::xlib::draw::Gc::set_bg(std::size_t const col)
const noexcept {
  ::XSetBackground(dpy.ptr, gc, col);
}
///////////////////////////////////////////////////////////
some::xlib::draw::Draw::Draw(Win const win, int const w, 
int const h, int const d) 
noexcept :
win { win },
drawable { ::XCreatePixmap(dpy.ptr, win, w, h, d) }
{ }

some::xlib::draw::Draw::Draw(Draw&& draw)
noexcept :
win { draw.win },
drawable { std::exchange(draw.drawable, 0L) }
{ }

some::xlib::draw::Draw&
some::xlib::draw::Draw::operator=(Draw&& draw)
noexcept {
  win = draw.win;
  drawable = draw.drawable;
  return *this;
}

some::xlib::draw::Draw::~Draw() {
  if (drawable)
    ::XFreePixmap(dpy.ptr, drawable);
}

void
some::xlib::draw::Draw::fill(::GC const gc, int const x, 
int const y, int const w, int const h)
const noexcept {
  // set color and other props on the gc first
  ::XFillRectangle(dpy.ptr, win, gc, x, y, w, h);
}

void
some::xlib::draw::Draw::string(char const S[], 
std::size_t const len, ::GC const gc, int const x, 
int const y)
const noexcept {
  // set color and other props on the gc first
  ::XDrawString(dpy.ptr, win, gc, x, y, S, len);
}

void
some::xlib::draw::Draw::stipple(::GC const gc, 
unsigned const d, unsigned const dia, int const w, 
int const h) const noexcept {
  for (int i { }; i < w; i += d)
    for (int j { }; j < h; j += d) {
      ::XFillArc(dpy.ptr, drawable, gc, 
        i, j, dia, dia, 0, 360 * 64);
    }
      
  ::XCopyArea(dpy.ptr, drawable, win, gc, 0, 0, w, h, 0, 0);
}

void
some::xlib::draw::Draw::set_clip(::GC const gc, 
int const x, int const y)
const noexcept {
  ::XSetClipMask(dpy.ptr, gc, drawable);
  ::XSetClipOrigin(dpy.ptr, gc, x, y);
}

void
some::xlib::draw::Draw::copy_plane(::GC const gc, 
Win const win, int const x0, int const y0, int const w, 
int const h, int const x1, int const y1)
const noexcept {
  ::XCopyPlane(dpy.ptr, drawable, win, gc, x0, y0, w, h, x1, y1, 1);

}
///////////////////////////////////////////////////////////
some::xlib::draw::Pixmap::Pixmap(Win const win, 
char const BITS[], unsigned const w, unsigned const h) 
noexcept :
win { win },
pixmap { 
  ::XCreateBitmapFromData(dpy.ptr, win, BITS, w, h) }
{ }

some::xlib::draw::Pixmap::~Pixmap() {
  ::XFreePixmap(dpy.ptr, pixmap);
}

void
some::xlib::draw::Pixmap::copy_plane(::GC const gc, 
int const x0, int const y0, int const w, int const h, 
int const x , int const y)
const noexcept {
  ::XCopyPlane(dpy.ptr, pixmap, win, gc, x0, y0, w, h, x, y, 1);
}
///////////////////////////////////////////////////////////
some::xlib::Hints::Hints() :
  win_hints { ::XAllocWMHints() } {
  if (win_hints == nullptr)
    throw std::runtime_error("Failed to alloc hints");
}

some::xlib::Hints::~Hints() {
  ::XFree(win_hints);
}
