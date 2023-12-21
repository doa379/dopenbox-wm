#include <iostream>
//#include <print>
#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <sstream>
#include <functional>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <msg.h>
#include <../config.h>

namespace msg {
  static constexpr std::string_view WMNAME { "Dopenbox WM" };
  static constexpr std::string_view VER { "-0.0" };

  template<typename T>
  concept Stringular = requires(T &t) {
    { t } -> std::convertible_to<std::basic_string<char>>;
  };

  template<typename T>
  concept Strangular = requires(T &t) {
    { t } -> std::convertible_to<std::basic_string_view<char>>;
  };

  class Dbg {
    struct Arg {
      std::stringstream ss;
      template<typename T>
      auto operator <<(const T &t) -> Arg & {
        ss << t;
        ss.flush();
        return *this;
      }
    };
    
    Arg arg;
    public:
    template<typename T, typename ...Args>
    //requires (Stringular<T> || Strangular<T>)
    auto msg(const T &t, const Args &...args) {
      if constexpr (std::is_convertible_v<T, std::string> ||
                      std::is_convertible_v<T, std::string_view>)
        arg << t << " ";
      else arg << std::to_string(t) << " ";
      msg(args...);
    }
    
    auto msg(void) {
      std::cout << arg.ss.str() << "\n";
      arg.ss.clear();
    }
  };

  static dobwm::Msg msg;

  template<typename ...Args>
  auto DBG(const Args &...args) {
    Dbg dbg;
    dbg.msg(args...);
  }
    
  auto send(std::string_view MSG, const dobwm::Urg URG, const unsigned TO) {
    msg.send(msg::WMNAME, MSG, URG, TO);
  }
}

namespace sys {
  template<typename TK, typename TF>
  class Ordered_map {
    std::vector<TK> O;
    std::unordered_map<TK, TF> M;
  public:
    using iterator = typename decltype(O)::iterator;
    using const_iterator = typename decltype(O)::const_iterator;
    constexpr auto size(void) const { return M.size(); }
    constexpr auto &operator[](const TK &K) { 
      if (!M.contains(K)) O.push_back(K); return M[K]; }
    constexpr std::pair<TK, TF> &operator[](const TK &K, const TF &F) const {
      return { K, F }; }
    constexpr auto &at(const TK &K) const { return M.at(K); }
    constexpr auto &at(const_iterator K) const { return at(*K); }
    constexpr auto find(const TK &K) const { return std::ranges::find(O, K); }
    constexpr auto begin(void) { return M.begin(); }
    constexpr auto end(void) { return M.end(); }
    constexpr auto cbegin(void) const { return O.begin(); }
    constexpr auto cend(void) const { return O.end(); }
    constexpr auto cprev(const_iterator K) const { 
      const auto O { std::prev(K) };
      return O < this->O.cbegin() ? this->O.cend() - 1 : O; }
    constexpr auto cnext(const_iterator K) const {
      const auto O { std::next(K) };
      return O == this->O.cend() ? this->O.cbegin() : O; }
    constexpr auto erase(const TK &K) { M.erase(K); O.erase(find(K)); }
    constexpr auto erase(const_iterator K) { M.erase(*K); O.erase(K); }
  };
}

namespace dobwm {
  struct X {  // Base class
    int modmask { };  // This mask needs repeated updates
    static inline ::Display *dpy { ::XOpenDisplay(nullptr) };
    static inline ::Window root { };
    struct {
      const std::size_t PROTOCOLS { 
          ::XInternAtom(dpy, "WM_PROTOCOLS", false) },
        NAME { ::XInternAtom(dpy, "WM_NAME", false) },
        DELETE_WINDOW { ::XInternAtom(dpy, "WM_DELETE_WINDOW", false) },
        STATE { ::XInternAtom(dpy, "WM_STATE", false) },
        TAKE_FOCUS { ::XInternAtom(dpy, "WM_TAKE_FOCUS", false) },
        SUPPORTED { ::XInternAtom(dpy, "_NET_SUPPORTED", false) },
        WM_STATE { ::XInternAtom(dpy, "_NET_WM_STATE", false) },
        WM_NAME { ::XInternAtom(dpy, "_NET_WM_NAME", false) },
        ACTIVE_WINDOW { ::XInternAtom(dpy, "_NET_ACTIVE_WINDOW", false) },
        WM_STATE_FULLSCREEN { 
          ::XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", false) },
        WM_WINDOW_TYPE { ::XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", false) },
        WM_WINDOW_TYPE_DIALOG { 
          ::XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", false) },
        CLIENT_LIST { ::XInternAtom(dpy, "_NET_CLIENT_LIST", false) },
        NUMBER_OF_DESKTOPS {
          ::XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", false) },
        WM_DESKTOP { ::XInternAtom(dpy, "_NET_WM_DESKTOP", false) },
        CURRENT_DESKTOP { ::XInternAtom(dpy, "_NET_CURRENT_DESKTOP", false) };
    } atom;
    
    std::pair<std::size_t, std::size_t> scr;
  };

  /*
  class Client {
    ::Window w;
    ::GC gc;
    std::pair<std::size_t, size_t> pos, size;
  };
  */

  class Panel {
    ::Drawable drawable;
    std::vector<::GC> G;
  public:
    Panel(const int) noexcept;
    ~Panel(void) noexcept;
    void new_gc(void) noexcept;
    void del_gc(::GC g) noexcept;
    void draw_wks(void) noexcept;
    void draw_title(std::string_view) noexcept;
  };

  class Action {
    std::reference_wrapper<Panel> p;
    std::unordered_map<Calls, std::function<void(void)>> F;
    sys::Ordered_map<::Window, std::vector<::Window>::const_iterator> T0;
    std::vector<::Window> T, S; // All, Features
    decltype(T)::const_iterator prev, curr;
  public:
    explicit Action(auto &) noexcept;
    ~Action(void) noexcept { };
    ///////////////////////////////////
    void unmapnotify(const ::Window) noexcept;
    void clientmessage(const ::Window) noexcept;
    void maprequest(const ::Window) noexcept;
    void motionnotify(const ::Window) noexcept;
    void keypress(const auto, const auto) noexcept;
    void buttonpress(const ::Window, const auto, const auto) noexcept;
    void propertynotify(const ::Window) noexcept;
    ///////////////////////////////////
    void call(const Calls C) const noexcept { F.at(C)(); }
    void shcmd(std::string_view CMD) const noexcept;
    void set_act(const ::Window) const noexcept;
    void set_inact(const ::Window) const noexcept;
    void kill(const ::Window) const noexcept;
    void focus(const ::Window) noexcept;
    std::pair<std::size_t, std::size_t> arrange(const std::size_t, 
      const std::size_t) noexcept;
    void workspace(const unsigned char) const noexcept;
  };
}

namespace {
  static dobwm::X x;
  volatile std::sig_atomic_t sig_status;
  static bool xerror;
  auto sig_handler(int sig) { sig_status = sig; }
  auto XError(::Display *, ::XErrorEvent *xev) {
    xerror = xev->error_code == BadAccess;
    return 0;
  }
}

dobwm::Panel::Panel(const int SCRN) noexcept :
  drawable { ::XCreatePixmap(x.dpy, x.root, 
    std::get<0>(x.scr), std::get<1>(x.scr), DefaultDepth(x.dpy, SCRN)) } {
  G.emplace_back(::GC { ::XCreateGC(x.dpy, x.root, 0, nullptr) });
  ::XSetLineAttributes(x.dpy, G[1], 1, LineSolid, CapButt, JoinMiter);
}

dobwm::Panel::~Panel(void) noexcept {
  for (auto &g : G) ::XFreeGC(x.dpy, g);
  ::XFreePixmap(x.dpy, drawable); 
}

void dobwm::Panel::new_gc(void) noexcept {
  G.emplace_back(::GC { ::XCreateGC(x.dpy, x.root, 0, nullptr) });
  ::XSetLineAttributes(x.dpy, G[0], 1, LineSolid, CapButt, JoinMiter);
}

void dobwm::Panel::del_gc(::GC g) noexcept {
  const auto G { 
    std::ranges::find_if(this->G, [&](const auto &G) { return G == g; })
  };

  this->G.erase(G);
  ::XFreeGC(x.dpy, g);
}

void dobwm::Panel::draw_wks(void) noexcept {
  ::XSetForeground(x.dpy, G[0], static_cast<std::size_t>(WKSBG));
  ::XFillRectangle(x.dpy, x.root, G[0], 0, 0, 48, BARH);
  ::XSetForeground(x.dpy, G[0], static_cast<std::size_t>(WKSFG));
  ::XDrawString(x.dpy, x.root, G[0], 0, BARH - 2, "GC0", 3);
}

void dobwm::Panel::draw_title(std::string_view S) noexcept {
  ::XSetForeground(x.dpy, G[1], static_cast<std::size_t>(TITLEBG));
  ::XFillRectangle(x.dpy, x.root, G[1], 50, 0, std::get<0>(x.scr), BARH);
  ::XSetForeground(x.dpy, G[1], static_cast<std::size_t>(TITLEFG));
  ::XDrawString(x.dpy, x.root, G[1], 50, BARH - 2, S.data(), S.size());
}

dobwm::Action::Action(auto &P) noexcept : p { P } {
  F[Calls::QUIT] = [] { std::raise(SIGINT); };
  F[Calls::UNMAPALL] = [] { };
  F[Calls::REMAPALL] = [] { };
  F[Calls::KILL] = [&] { if (curr < T.cend()) kill(*curr); };
  F[Calls::SWFOCUS] = [&] { focus(*prev); };
  F[Calls::PREVCLI] = [&] { 
    const auto O { std::prev(curr) };
    focus(O < T.cbegin() ? T.back() : *O); };
  F[Calls::NEXTCLI] = [&] { 
    const auto O { std::next(curr) };
    focus(O == T.cend() ? T.front() : *O); };
  F[Calls::SELTOGGLE] = [] { };
  F[Calls::SELCLEAR] = [] { };
  F[Calls::MOVEUP] = [] { msg::DBG("Move Up"); };
  F[Calls::MOVEDOWN] = [] { };
  F[Calls::MOVELEFT] = [] { };
  F[Calls::MOVERIGHT] = [] { };
  F[Calls::RESIZEVINC] = [] { msg::DBG("Resize V+"); };
  F[Calls::RESIZEVDEC] = [] { };
  F[Calls::RESIZEHDEC] = [] { };
  F[Calls::RESIZEHINC] = [] { };
  F[Calls::SELECT] = [] { };
  F[Calls::RESIZE] = [] { };
  F[Calls::WKS0] = [&] { workspace(0); };
  F[Calls::WKS1] = [&] { workspace(1); };
  F[Calls::WKS2] = [&] { workspace(2); };
  F[Calls::WKS3] = [&] { workspace(3); };
  F[Calls::WKS4] = [&] { workspace(4); };
  F[Calls::WKS5] = [&] { workspace(5); };
  F[Calls::WKS6] = [&] { workspace(6); };
  F[Calls::WKS7] = [&] { workspace(7); };
  F[Calls::WKS8] = [&] { workspace(8); };
  F[Calls::WKS9] = [&] { workspace(9); };
  F[Calls::STATE] = [&] { };
}

void dobwm::Action::unmapnotify(const ::Window W) noexcept {
  msg::DBG("Event UnmapNotify");
  if (const auto O { std::ranges::find(T, W) }; O < T.end()) {
    curr = O - 1;
    T.erase(O);
    set_act(*curr);
    ::XDeleteProperty(x.dpy, x.root, x.atom.CLIENT_LIST);
    for (const auto W : T)
      ::XChangeProperty(x.dpy, x.root, x.atom.CLIENT_LIST, XA_WINDOW, 32, 
          PropModeAppend, reinterpret_cast<const unsigned char *>(&W), 1);
  }
}

void dobwm::Action::clientmessage(const ::Window W) noexcept {
  msg::DBG("Event ClientMessage ");
  
  // Handle messages

  msg::DBG("Clientmessage on Window ", W);
}

void dobwm::Action::maprequest(const ::Window W) noexcept {
  msg::DBG("Event MapRequest");
  static ::XWindowAttributes wa;
  msg::DBG("Window ", W);
  if (!::XGetWindowAttributes(x.dpy, W, &wa) || wa.override_redirect)
    return;
  
  static constexpr auto WMASK {
    EnterWindowMask |
    FocusChangeMask |
    PropertyChangeMask |
    StructureNotifyMask };
  ::XSelectInput(x.dpy, W, WMASK);
  ::XChangeProperty(x.dpy, x.root, x.atom.CLIENT_LIST, XA_WINDOW, 32, 
      PropModeAppend, reinterpret_cast<const unsigned char *>(&W), 1);
  if (T.size()) {
    const auto POS { arrange(wa.width, wa.height) };
    wa.x = std::get<0>(POS);
    wa.y = std::get<1>(POS);
  }

  ::XMoveWindow(x.dpy, W, wa.x, wa.y < BARH ? BARH : wa.y);
  ::XMapWindow(x.dpy, W);
  
  prev = curr;
  if (T.size()) set_inact(*curr);
  T.emplace_back(W);
  curr = T.cend() - 1;
  set_act(*curr);
  p.get().new_gc();
  msg::DBG("End MapRequest");
}

void dobwm::Action::motionnotify(const ::Window W) noexcept {
  //msg::DBG("Event MotionNotify Window ", W);
}

void dobwm::Action::keypress(const auto STATE, const auto CODE) noexcept {
  msg::DBG("Event KeyPress ");
  const auto KMOD { STATE & x.modmask };
  const auto KSYM { ::XkbKeycodeToKeysym(x.dpy, CODE, 0, 0) };
  const auto &V { KEYS.at(KMOD).at(KSYM) };
  if (V.index() == 0) call(std::get<0>(V));
  else shcmd(std::get<1>(V));
}

void dobwm::Action::buttonpress(const ::Window W, const auto STATE, const auto BTN) noexcept {
  msg::DBG("Event ButtonPress Window ", W);
  ::XUngrabPointer(x.dpy, CurrentTime);
}

void dobwm::Action::propertynotify(const ::Window W) noexcept {
  msg::DBG("Event PropertyNotify ");
  if (W == x.root) {
    p.get().draw(msg::WMNAME);
  } else if (::XTextProperty tp;
      ::XGetTextProperty(x.dpy, W, &tp, x.atom.NAME) && tp.nitems) {
        p.get().draw_title(reinterpret_cast<const char *>(tp.value));
  }

  msg::DBG("End PropertyNotify");
}

void dobwm::Action::shcmd(std::string_view CMD) const noexcept { 
  if (fork() == 0) {
    ::close(ConnectionNumber(x.dpy));
    ::setsid();
    ::system(CMD.data());
  }
}

void dobwm::Action::set_act(const ::Window W) const noexcept {
  ::XSetWindowBorder(x.dpy, W, static_cast<std::size_t>(ACTBDR));
  ::XSetWindowBorderWidth(x.dpy, W, BDRW);
  ::XSetInputFocus(x.dpy, W, RevertToPointerRoot, CurrentTime);
  ::XChangeProperty(x.dpy, x.root, x.atom.WM_STATE, XA_WINDOW, 32, 
    PropModeReplace, reinterpret_cast<const unsigned char *>(&W), 1);
  ::XChangeProperty(x.dpy, W, x.atom.ACTIVE_WINDOW, XA_WINDOW, 32, 
    PropModeReplace, reinterpret_cast<const unsigned char *>(&W), 1);
  ::XRaiseWindow(x.dpy, W);
////////////////////////////////////////////////////////////////////////////  
  static constexpr auto BUTTONMASK { ButtonPressMask | ButtonReleaseMask };
  for (const auto &MOD : BTNS)
    for (const auto &BTN : std::get<1>(MOD)) {
      ::XUngrabButton(x.dpy, std::get<0>(BTN), std::get<0>(MOD) & x.modmask, W);
      ::XGrabButton(x.dpy, std::get<0>(BTN), std::get<0>(MOD) & x.modmask, 
        W, false, BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
    }
}

void dobwm::Action::set_inact(const ::Window W) const noexcept {
  for (const auto &MOD : BTNS)
    for (const auto &BTN : std::get<1>(MOD))
      ::XUngrabButton(x.dpy, std::get<0>(BTN), std::get<0>(MOD) & x.modmask, x.root);
  
  ::XDeleteProperty(x.dpy, W, x.atom.ACTIVE_WINDOW);
  ::XSetWindowBorder(x.dpy, W, static_cast<std::size_t>(INACTBDR));
}

void dobwm::Action::kill(const ::Window W) const noexcept {
  ::XEvent xev { ClientMessage };
  xev.xclient.window = W;
  xev.xclient.message_type = x.atom.PROTOCOLS;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = x.atom.DELETE_WINDOW;
  xev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(x.dpy, W, false, NoEventMask, &xev);
}

void dobwm::Action::focus(const ::Window W) noexcept {
  if (W == *curr) return;
  set_inact(*curr);
  prev = curr;
  curr = std::ranges::find(T, W);
  set_act(*curr);
}

std::pair<std::size_t, std::size_t> 
dobwm::Action::arrange(const std::size_t W, const std::size_t H) noexcept {
  ///////////// Horiz polarization dumb covering alog.
  // First determine minimal y
  // Then determine maximal x

  /*
  if (!T.size()) return { };
  // Determine maximal x
  ::XWindowAttributes ua;
  ::XGetWindowAttributes(x.dpy, T.front(), &ua);
  auto x_ { 0 };
  for (const auto U : T) {
    ::XGetWindowAttributes(x.dpy, U, &ua);
    if (ua.x + ua.width > x_)
      x_ = ua.x + ua.width;
  }

  // Determine maximal y
  ::XGetWindowAttributes(x.dpy, T.front(), &ua);
  auto y_ { 0 };
  for (const auto U : T) {
    ::XGetWindowAttributes(x.dpy, U, &ua);
    if (x_ > std::get<0>(x.scr) && ua.x + ua.height > y_)
      y_ = ua.x + ua.height;
  }

  ::XGetWindowAttributes(x.dpy, T.back(), &ua);
  if (ua.x + ua.width + W < std::get<0>(x.scr))
    return { ua.x + ua.width, y_ };
  else if (ua.x + ua.height + H < std::get<1>(x.scr))
    return { x_, ua.x + ua.height };
  */

  if (!T.size()) return { };
  ::XWindowAttributes wa;
  ::XGetWindowAttributes(x.dpy, T.back(), &wa);
  if (wa.x + 20 + W < std::get<0>(x.scr) && wa.y + 20 + H < std::get<1>(x.scr))
    return { wa.x + 20, wa.y + 20 };

/*
  // West
  if (std::get<0>(x.scr) - wa.x - wa.width > W)
    return { wa.x + wa.width, ... };
  // East
  else if (va.x > W)
    return { va.x - W, miny };
  // North
  else if (va.y > H)
    return {minx, va.y - H };
  // South
  else if (std::get<1>(x.scr) - va.y - va.height > H)
    return { minx, va.y + va.height };
*/
  return { };
}

void dobwm::Action::workspace(const unsigned char N) const noexcept {
  /*
  ::XChangeProperty(x.dpy, x.root, x::NET::CURRENT_DESKTOP, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char *>(&N), 1);
  */
  ::XEvent xev { ClientMessage };
  xev.xclient.window = x.root;
  xev.xclient.message_type = x.atom.CURRENT_DESKTOP;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = N;
  xev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(x.dpy, x.root, false, NoEventMask, &xev);
}

int main(const int ARGC, const char *ARGV[]) {
  try {
    //std::println("...");
    msg::DBG(msg::WMNAME, "ver. ", msg::VER);
    if (!x.dpy) throw std::runtime_error("Unable to open display");
    static const auto SCRN { DefaultScreen(x.dpy) };
    x.scr = std::pair<std::size_t, std::size_t> { 
      DisplayWidth(x.dpy, SCRN), DisplayHeight(x.dpy, SCRN) };
    // Root window
    x.root = RootWindow(x.dpy, SCRN);
    msg::DBG("Root Window ", x.root);
    ::XSetErrorHandler(XError);
    static constexpr auto ROOTMASK {
      SubstructureRedirectMask | 
      SubstructureNotifyMask | 
      ButtonPressMask |
      PointerMotionMask |
      EnterWindowMask |
      LeaveWindowMask |
      StructureNotifyMask |
      PropertyChangeMask
    };

    ::XSelectInput(x.dpy, x.root, ROOTMASK);
    ::XSync(x.dpy, false);
    if (xerror) {
      ::XCloseDisplay(x.dpy);
      throw std::runtime_error("Initialization error (another wm running?)");
    }

    ::XUngrabKey(x.dpy, AnyKey, AnyModifier, x.root);
    // Modifier Mask
    ::XModifierKeymap *modmap { ::XGetModifierMapping(x.dpy) };
    unsigned numlockmask { };
    for (int k { }; k < 8; k++)
      for (int j { }; j < modmap->max_keypermod; j++)
        if (modmap->modifiermap[modmap->max_keypermod * k + j] ==
            ::XKeysymToKeycode(x.dpy, XK_Num_Lock))
          numlockmask = (1 << k);
    
    ::XFreeModifiermap(modmap);
    x.modmask = ~(numlockmask | LockMask);
    for (const auto &MOD : dobwm::KEYS)
      for (const auto &KEY : std::get<1>(MOD))
        ::XGrabKey(x.dpy, ::XKeysymToKeycode(x.dpy, std::get<0>(KEY)), 
          std::get<0>(MOD) & x.modmask, x.root, true, 
            GrabModeAsync, GrabModeAsync);
    
    ::XSync(x.dpy, false);
    
    static dobwm::Panel p { SCRN };
    static dobwm::Action a { p };
    static ::XEvent xev;
    static std::array<std::function<void(void)>, LASTEvent> F; // Literal defn.
    for (auto &f : F) f = [] { };
    F[MapNotify] = [] { msg::DBG("Event MapNotify"); };
    F[UnmapNotify] = [] { a.unmapnotify(xev.xunmap.window); };
    F[ClientMessage] = [] { a.clientmessage(xev.xclient.window); };
    F[ConfigureNotify] = [] { 
      (void) xev.xconfigure;
      msg::DBG("Event ConfigureNotify");
    };
    F[MapRequest] = [] { a.maprequest(xev.xmaprequest.window); };
    F[ConfigureRequest] = [] { 
      const auto &CONF { xev.xconfigurerequest };
      ::XWindowChanges wc {
          CONF.x, CONF.y, CONF.width, CONF.height,
          CONF.border_width, CONF.above, CONF.detail };
      ::XConfigureWindow(x.dpy, CONF.window, CONF.value_mask, &wc);
    };
    F[MotionNotify] = [] { a.motionnotify(xev.xmotion.window); };
    F[KeyPress] = [] { a.keypress(xev.xkey.state, xev.xkey.keycode); };
    F[ButtonPress] = [] { 
      a.buttonpress(xev.xbutton.window, xev.xbutton.state, xev.xbutton.button);
    };
    F[EnterNotify] = [] { 
      if (xev.xcrossing.window != x.root && dobwm::SLOPPY_FOCUS)
        a.focus(xev.xcrossing.window);
    };
    F[PropertyNotify] = [] { a.propertynotify(xev.xproperty.window); };

    if (dobwm::INITWKS >= 0 && dobwm::INITWKS <= dobwm::NWKS) {
      ::XChangeProperty(x.dpy, x.root, x.atom.NUMBER_OF_DESKTOPS,
        XA_CARDINAL, 32, PropModeReplace,
          reinterpret_cast<const unsigned char *>(&dobwm::NWKS), 1);
      ::XChangeProperty(x.dpy, x.root, x.atom.CURRENT_DESKTOP,
        XA_CARDINAL, 32, PropModeReplace,
          reinterpret_cast<const unsigned char *>(&dobwm::INITWKS), 1);
    }

    unsigned n;
    ::Window root, parent, *w { };
    if (::XQueryTree(x.dpy, x.root, &root, &parent, &w, &n)) {
      for (unsigned i { }; i < n; i++) {
        ::XWindowAttributes wa;
        if (::XGetWindowAttributes(x.dpy, w[i], &wa) && 
            wa.map_state == IsViewable) {
          ::XEvent xev { MapRequest };
          xev.xmaprequest.send_event = true,
          xev.xmaprequest.parent = x.root;
          xev.xmaprequest.window = w[i];
          ::XSendEvent(x.dpy, x.root, true, ROOTMASK, &xev);
        }
      }

      if (w) ::XFree(w);
    }

    ::XSync(x.dpy, false);
    p.draw_title(msg::WMNAME);
    std::signal(SIGINT, sig_handler);
    msg::DBG("WM initialized");
    msg::send("Welcome msg", dobwm::Urg::NORMAL, 1000);
    // Ev loop
    while (sig_status == 0 && ::XNextEvent(x.dpy, &xev) == 0) {
      F[xev.type]();
      ::XSync(x.dpy, false);
    }
    // Deinit
    for (const auto &MOD : dobwm::KEYS)
      for (const auto &KEY : std::get<1>(MOD))
        ::XUngrabKey(x.dpy, ::XKeysymToKeycode(x.dpy, std::get<0>(KEY)),
          std::get<0>(MOD) & x.modmask, x.root);

    ::XSetInputFocus(x.dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    ::XCloseDisplay(x.dpy);
    msg::DBG("\nWM exit");
  } catch (const std::exception &E) {
      msg::DBG("Ex.", E.what());
      //std::println("Ex. { }", E.what());
      return -1;
  }
  
  return 0;
}
