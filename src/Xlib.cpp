#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <iostream>

#include "Xlib.h"
///////////////////////////////////////////////////////////
::Display* some::Display::ptr;
int some::Display::width;
int some::Display::height;
int some::Display::depth;
::Window some::Display::root;
///////////////////////////////////////////////////////////
bool some::xlib::DefaultXError::xerror;
///////////////////////////////////////////////////////////
::XFontStruct* some::xlib::Font::fn;
int some::xlib::Font::scent;
///////////////////////////////////////////////////////////
::Cursor some::xlib::Cursor::ptr;
::Cursor some::xlib::Cursor::move;
::Cursor some::xlib::Cursor::resize;
::Cursor some::xlib::Cursor::h;
::Cursor some::xlib::Cursor::v;
///////////////////////////////////////////////////////////
unsigned char* some::xlib::Prop::data;
::Atom some::xlib::Prop::wm_protocols;
::Atom some::xlib::Prop::wm_name;
::Atom some::xlib::Prop::wm_delete_window;
::Atom some::xlib::Prop::wm_state;
::Atom some::xlib::Prop::wm_take_focus;
::Atom some::xlib::Prop::wm_icon_name;
::Atom some::xlib::Prop::net_supported;
::Atom some::xlib::Prop::net_wm_state;
::Atom some::xlib::Prop::net_wm_name;
::Atom some::xlib::Prop::net_wm_window_opacity;
::Atom some::xlib::Prop::net_active_window;
::Atom some::xlib::Prop::net_wm_state_fullscreen;
::Atom some::xlib::Prop::net_wm_window_type;
::Atom some::xlib::Prop::net_wm_window_type_dialog;
::Atom some::xlib::Prop::net_client_list;
::Atom some::xlib::Prop::net_number_of_desktops;
::Atom some::xlib::Prop::net_wm_desktop;
::Atom some::xlib::Prop::net_current_desktop;
::Atom some::xlib::Prop::net_showing_desktop;
::Atom some::xlib::Prop::net_wm_icon;
::Atom some::xlib::Prop::net_wm_icon_name;
::Atom some::xlib::Prop::actual_type;
int some::xlib::Prop::actual_format;
unsigned long some::xlib::Prop::nitems;
unsigned long some::xlib::Prop::bytes_after;
///////////////////////////////////////////////////////////
void
some::Display::init() {
  if ((ptr = ::XOpenDisplay(nullptr)) == nullptr)
    throw std::runtime_error("Failed to open display");
  
  width = DisplayWidth(ptr, DefaultScreen(ptr));
  height = DisplayHeight(ptr, DefaultScreen(ptr));
  depth = DefaultDepth(ptr, DefaultScreen(ptr));
  root = ::XRootWindow(ptr, DefaultScreen(ptr));
}

void 
some::Display::deinit() {
  ::XCloseDisplay(ptr);
}

int
some::Display::connection() { 
  return ConnectionNumber(ptr);
}
///////////////////////////////////////////////////////////
some::Ev::Ev() {

}

void
some::Ev::mask_event(long const mask)
noexcept {
  ::XMaskEvent(dpy.ptr, mask, &xev);
}
  
lib::Generator<::XEvent>
some::Ev::seq() {
  struct ::pollfd pfd {
    .fd { dpy.connection() },
    .events { POLLIN }
  };

  while (true) {
    if (::poll(&pfd, 1, -1) == -1)
      break;
    else if (pfd.revents == 0)
      continue;

    while (::XPending(dpy.ptr)) {
      ::XNextEvent(dpy.ptr, &xev);
      co_yield xev;
    }
  }
}
///////////////////////////////////////////////////////////
void
some::Sys::spawn(char const CMD[])
const noexcept {
  if (::fork() == 0) {
    ::close(dpy.connection());
    ::setsid();
    ::system(CMD);
  }
}
///////////////////////////////////////////////////////////
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
  return -1;
};
///////////////////////////////////////////////////////////
some::xlib::KeyMod::KeyMod() :
  map { ::XGetModifierMapping(dpy.ptr) },
  numlock_kcode {
    ::XKeysymToKeycode(dpy.ptr, XK_Num_Lock) }
{
  if (map == nullptr)
    throw std::runtime_error(
        "Failed to get modifier mapping");
}

some::xlib::KeyMod::KeyMod(KeyMod&& keymod)
noexcept :
  map { std::exchange(keymod.map, nullptr) },
  numlock_kcode { keymod.numlock_kcode }
{

}

some::xlib::KeyMod&
some::xlib::KeyMod::operator=(KeyMod&& keymod) 
noexcept {
  map = std::exchange(keymod.map, nullptr);
  numlock_kcode = keymod.numlock_kcode;
  return *this;
}

some::xlib::KeyMod::~KeyMod() {
  if (map)
    ::XFreeModifiermap(map);
}

unsigned
some::xlib::KeyMod::numlock_mask()
noexcept {
  unsigned mask { };
  for (int k { }; k < 8; k++)
    for (int j { }; j < map->max_keypermod; j++)
      if (map->modifiermap[map->max_keypermod * k + j] == 
        numlock_kcode)
        mask = (1 << k);
  
  return mask;
}
///////////////////////////////////////////////////////////
some::xlib::KeySym::KeySym() {
  ::XDisplayKeycodes(dpy.ptr, &start, &end);
  syms = ::XGetKeyboardMapping(dpy.ptr, 
    start, end - start + 1, &skip);
  if (syms == nullptr)
    throw std::runtime_error(
        "Failed to get keyboard mapping");
}

some::xlib::KeySym::~KeySym() {
  ::XFree(syms);  
}

std::vector<unsigned>
some::xlib::KeySym::get_ksyms()
const noexcept {
  std::vector<unsigned> syms;
  for (int k { start }; k <= end; k++)
    syms.emplace_back(this->syms[(k - start) * skip]);

  return syms;
}
///////////////////////////////////////////////////////////
some::xlib::WinAttr::WinAttr(::Window const win) {
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
///////////////////////////////////////////////////////////
some::xlib::QueryTree::QueryTree(::Window const win) {
  ::Window root;
  ::Window parw;
  ::XQueryTree(dpy.ptr, win, &root, &parw, &wins, &n);
}

some::xlib::QueryTree::~QueryTree() {
  ::XFree(wins);
}

std::vector<::Window>
some::xlib::QueryTree::get() 
const noexcept {
  std::vector<::Window> WINS;
  for (unsigned i { }; i < n; i++)
    WINS.emplace_back(wins[i]);

  return WINS;
}
///////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////
void
some::xlib::Font::init(char const FONT[]) {
  if ((fn = ::XLoadQueryFont(dpy.ptr, FONT)) == nullptr)
    throw std::runtime_error("No font found");

  scent = fn->ascent + fn->descent;
}

void
some::xlib::Font::deinit() {
  if (fn)
    ::XFreeFont(dpy.ptr, fn);
}

int
some::xlib::Font::get_scent()
noexcept {
  return scent;
}

int
some::xlib::Font::get_ascent() 
noexcept {
  return fn->ascent;
}

int 
some::xlib::Font::get_descent() 
noexcept {
  return fn->descent;
}

int
some::xlib::Font::text_width(char const S[], 
std::size_t const len)
noexcept {
  return ::XTextWidth(fn, S, len);
}

int
some::xlib::Font::text_width16(wchar_t const S[],
std::size_t const len)
noexcept {
  ::XChar2b const s { 
    static_cast<unsigned char>(S[0]), 
    static_cast<unsigned char>(S[1])
  };

  return ::XTextWidth16(fn, &s, len);
}
///////////////////////////////////////////////////////////
some::xlib::Gc::Gc(::Window const win)
noexcept :
  gc { ::XCreateGC(dpy.ptr, win, 0, nullptr) } {
  ::XSetLineAttributes(dpy.ptr, gc, 1, LineSolid, CapButt,
    JoinMiter);
}

some::xlib::Gc::Gc(Gc&& gc)
noexcept :
  gc { std::exchange(gc.gc, nullptr) }
{ }

some::xlib::Gc&
some::xlib::Gc::operator=(Gc&& gc)
noexcept {
  this->gc = std::exchange(gc.gc, nullptr);
  return *this;
}

some::xlib::Gc::~Gc() {
  if (gc)
    ::XFreeGC(dpy.ptr, gc);
}

::GC
some::xlib::Gc::get()
const noexcept {
  return gc;
}

void
some::xlib::Gc::set_fg(std::size_t const col)
const noexcept {
  ::XSetForeground(dpy.ptr, gc, col);
}

void
some::xlib::Gc::set_bg(std::size_t const col)
const noexcept {
  ::XSetBackground(dpy.ptr, gc, col);
}
///////////////////////////////////////////////////////////
some::xlib::Draw::Draw(::Window const win, 
int const w, int const h, int const d) 
noexcept :
  win { win },
  drawable { ::XCreatePixmap(dpy.ptr, win, w, h, d) }
{ }

some::xlib::Draw::Draw(Draw&& draw)
noexcept :
  win { draw.win },
  drawable { std::exchange(draw.drawable, 0) }
{ }

some::xlib::Draw&
some::xlib::Draw::operator=(Draw&& draw)
noexcept {
  win = draw.win;
  drawable = std::exchange(draw.drawable, 0);
  return *this;
}

some::xlib::Draw::~Draw() {
  if (drawable) {
    ::XFreePixmap(dpy.ptr, drawable);
  }
}

void
some::xlib::Draw::fill(::GC const gc, int const x, 
int const y, int const w, int const h)
const noexcept {
  // set color and other props on the gc first
  ::XFillRectangle(dpy.ptr, win, gc, x, y, w, h);
}

void
some::xlib::Draw::string(char const S[], 
std::size_t const len, ::GC const gc, int const x, 
int const y)
const noexcept {
  // set color and other props on the gc first
  ::XDrawString(dpy.ptr, win, gc, x, y, S, len);
}

void
some::xlib::Draw::stipple(::GC const gc, 
unsigned const d, unsigned const dia, int const w, 
int const h) const noexcept {
  for (int i { }; i < w; i += d)
    for (int j { }; j < h; j += d) {
      ::XFillArc(dpy.ptr, drawable, gc, 
        i, j, dia, dia, 0, 64 * 360);
    }
      
  //::XCopyArea(dpy.ptr, drawable, win, gc, 0, 0, w, h, 
    //0, 0);
}

void
some::xlib::Draw::set_clip(::GC const gc, 
int const x, int const y)
const noexcept {
  ::XSetClipMask(dpy.ptr, gc, drawable);
  ::XSetClipOrigin(dpy.ptr, gc, x, y);
}

void
some::xlib::Draw::copy_plane(::GC const gc, 
::Window const win, int const x0, int const y0, 
int const w, int const h, int const x1, int const y1)
const noexcept {
  ::XCopyPlane(dpy.ptr, drawable, win, gc, x0, y0, w, h,
    x1, y1, 1);

}
///////////////////////////////////////////////////////////
some::xlib::Pixmap::Pixmap(::Window const win, 
char const BITS[], unsigned const w, unsigned const h) 
noexcept :
  win { win },
  pixmap { 
    ::XCreateBitmapFromData(dpy.ptr, win, BITS, w, h) }
{ }

some::xlib::Pixmap::~Pixmap() {
  ::XFreePixmap(dpy.ptr, pixmap);
}

void
some::xlib::Pixmap::copy_plane(::GC const gc, 
int const x0, int const y0, int const w, int const h, 
int const x1 , int const y1)
const noexcept {
  ::XCopyPlane(dpy.ptr, pixmap, win, gc, x0, y0, w, h, x1,
    y1, 1);
}
///////////////////////////////////////////////////////////
void
some::xlib::Cursor::init()
noexcept {
  ptr = ::XCreateFontCursor(dpy.ptr, PTR_SYM);
  move = ::XCreateFontCursor(dpy.ptr, MOVE_SYM);
  resize = ::XCreateFontCursor(dpy.ptr, RESIZE_SYM);
  h = ::XCreateFontCursor(dpy.ptr, HDBL_SYM);
  v = ::XCreateFontCursor(dpy.ptr, VDBL_SYM);
}

void
some::xlib::Cursor::deinit() {
  ::XFreeCursor(dpy.ptr, v);
  ::XFreeCursor(dpy.ptr, h);
  ::XFreeCursor(dpy.ptr, resize);
  ::XFreeCursor(dpy.ptr, move);
  ::XFreeCursor(dpy.ptr, ptr);
}
///////////////////////////////////////////////////////////
void
some::xlib::Prop::init() 
noexcept {
  data = new unsigned char { DATASIZE };
  wm_protocols =
    ::XInternAtom(dpy.ptr, "WM_PROTOCOLS", false);
  wm_name =::XInternAtom(dpy.ptr, "WM_NAME", false);
  wm_delete_window =
    ::XInternAtom(dpy.ptr, "WM_DELETE_WINDOW", false);
  wm_state = ::XInternAtom(dpy.ptr, "WM_STATE", false);
  wm_take_focus =
    ::XInternAtom(dpy.ptr, "WM_TAKE_FOCUS", false);
  wm_icon_name =
    ::XInternAtom(dpy.ptr, "WM_ICON_NAME", false);
  net_supported =
    ::XInternAtom(dpy.ptr, "_NET_SUPPORTED", false);
  net_wm_state =
    ::XInternAtom(dpy.ptr, "_NET_WM_STATE", false);
  net_wm_name =
    ::XInternAtom(dpy.ptr, "_NET_WM_NAME", false);
  net_wm_window_opacity =
    ::XInternAtom(dpy.ptr, "_NET_WM_WINDOW_OPACITY", 
      false);
  net_active_window =
    ::XInternAtom(dpy.ptr, "_NET_ACTIVE_WINDOW", false);
  net_wm_state_fullscreen =
    ::XInternAtom(dpy.ptr, "_NET_WM_STATE_FULLSCREEN", 
      false);
  net_wm_window_type =
    ::XInternAtom(dpy.ptr, "_NET_WM_WINDOW_TYPE", false);
  net_wm_window_type_dialog =
    ::XInternAtom(dpy.ptr, "_NET_WM_WINDOW_TYPE_DIALOG", 
      false);
  net_client_list =
    ::XInternAtom(dpy.ptr, "_NET_CLIENT_LIST", false);
  net_number_of_desktops =
    ::XInternAtom(dpy.ptr, "_NET_NUMBER_OF_DESKTOPS",
      false);
  net_wm_desktop =
    ::XInternAtom(dpy.ptr, "_NET_WM_DESKTOP", false);
  net_current_desktop =
    ::XInternAtom(dpy.ptr, "_NET_CURRENT_DESKTOP", 
      false);
  net_showing_desktop =
    ::XInternAtom(dpy.ptr, "_NET_SHOWING_DESKTOP", 
      false);
  net_wm_icon = 
    ::XInternAtom(dpy.ptr, "_NET_WM_ICON", false);
  net_wm_icon_name =
      ::XInternAtom(dpy.ptr, "_NET_WM_ICON_NAME", false);
}

void
some::xlib::Prop::deinit()
noexcept {
  delete data;
}

void
some::xlib::Prop::change_state(::Window const win) 
noexcept {
  ::XChangeProperty(dpy.ptr, win, wm_state, XA_WINDOW, 32,
    PropModeReplace, (unsigned char*) &win, 1);
}

std::optional<std::string_view>
some::xlib::Prop::get_name(::Window const win)
noexcept {
  if (!(::XGetWindowProperty(dpy.ptr, win, net_wm_name, 0, 
      DATASIZE, false, AnyPropertyType, &actual_type, 
      &actual_format, &nitems, &bytes_after, &data) == 
      Success && data))
    if (!(::XGetWindowProperty(dpy.ptr, win, wm_name, 0, 
      DATASIZE, false, AnyPropertyType, &actual_type, 
      &actual_format, &nitems, &bytes_after, &data) == 
      Success && data))
      return std::nullopt;

  return reinterpret_cast<char*>(data);
}

std::optional<std::string_view>
some::xlib::Prop::get_icon(::Window const win)
noexcept {
  if (!(::XGetWindowProperty(dpy.ptr, win, 
      net_wm_icon_name, 0, DATASIZE, false, 
      AnyPropertyType, &actual_type, 
      &actual_format, &nitems, &bytes_after, 
      &data) == Success && data))
    if (!(::XGetWindowProperty(dpy.ptr, win, wm_icon_name,
      0, DATASIZE, false, AnyPropertyType, &actual_type, 
      &actual_format, &nitems, &bytes_after, &data) == 
      Success && data))
      return std::nullopt;

  return reinterpret_cast<char*>(data);
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
