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

namespace dobwm {
  using Dim = std::pair<std::size_t, std::size_t>;
  struct X {  // Base class
    int modmask { };  // This mask needs repeated updates
    static inline ::Display *dpy { ::XOpenDisplay(nullptr) };
    static inline std::map<std::string_view, ::Atom> ATOM;
    ::Window root { };
    Dim scr;
  };

  struct Client {
    std::string title;
    Dim size, pos;
    bool trans { }, mut { }, sel { };
  };
  
  class Panel {
    ::Drawable drawable;
    ::GC gc;
public:
    Panel(const int) noexcept;
    ~Panel(void) noexcept;
    void draw(std::string_view);
  };

  class Action {
    std::reference_wrapper<Panel> p;
    std::map<Calls, std::function<void(void)>> F;
    std::unordered_map<::Window, Client>::const_iterator prev, curr;
  public:
    explicit Action(auto &) noexcept;
    ~Action(void) noexcept { };
    ///////////////////////////////////
    void unmapnotify(const ::Window);
    void clientmessage(const ::Window);
    void maprequest(const ::Window);
    void motionnotify(const ::Window);
    void keypress(const auto, const auto) noexcept;
    void buttonpress(void);
    void enternotify(const ::Window);
    void propertynotify(const ::Window);
    ///////////////////////////////////
    void call(const Calls C) const { F.at(C)(); }
    void shcmd(std::string_view CMD) const;
    void focus(::Window);
    void kill(void) const;
    void swfocus(void);
    void client(const char);
    void workspace(const unsigned char);
  };
}

namespace {
  static dobwm::X x;
  static std::unordered_map<::Window, dobwm::Client> T;
  volatile std::sig_atomic_t sig_status;
  static bool xerror;
  auto sig_handler(int sig) {
    sig_status = sig;
  }

  auto XError(::Display *, ::XErrorEvent *xev) {
    xerror = xev->error_code == BadAccess;
    return 0;
  }
}

dobwm::Panel::Panel(const int SCRN) noexcept :
  drawable { ::XCreatePixmap(x.dpy, x.root, std::get<0>(x.scr), std::get<1>(x.scr), DefaultDepth(x.dpy, SCRN)) },
  gc { ::XCreateGC(x.dpy, x.root, 0, nullptr) } {
  ::XSetLineAttributes(x.dpy, gc, 1, LineSolid, CapButt, JoinMiter);
}

dobwm::Panel::~Panel(void) noexcept { 
  ::XFreeGC(x.dpy, gc);
  ::XFreePixmap(x.dpy, drawable); 
}

void dobwm::Panel::draw(std::string_view S) {
  ::XSetForeground(x.dpy, gc, 0xF9F9F9);
  ::XFillRectangle(x.dpy, x.root, gc, 0, 0, std::get<0>(x.scr), BARH);
  ::XSetForeground(x.dpy, gc, 0x000000);
  ::XDrawString(x.dpy, x.root, gc, 0, BARH - 2, S.data(), S.size());
}

dobwm::Action::Action(auto &P) noexcept : p { P } {
  F[Calls::QUIT] = [] { std::raise(SIGINT); };
  F[Calls::UNMAPALL] = [] { };
  F[Calls::REMAPALL] = [] { };
  F[Calls::KILL] = [&] { kill(); };
  F[Calls::SWFOCUS] = [&] { swfocus(); };
  F[Calls::PREVCLI] = [&] { client(-1); };
  F[Calls::NEXTCLI] = [&] { client(+1); };
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
}


void dobwm::Action::unmapnotify(const ::Window W) {
  msg::DBG("Event UnmapNotify");
  if (T.find(W) == T.cend()) return;
  ::XDeleteProperty(x.dpy, W, x.ATOM["_NET_ACTIVE_WINDOW"]);
  client(-1);
  T.erase(W);
}

void dobwm::Action::clientmessage(const ::Window W) {
  msg::DBG("Event ClientMessage ");
  
  // Handle messages

  msg::DBG("Clientmessage on Window ", W);
}

void dobwm::Action::maprequest(const ::Window W) {
  msg::DBG("Event MapRequest");
  static ::XWindowAttributes wa;
  msg::DBG("Window ", W);
  if (!::XGetWindowAttributes(x.dpy, W, &wa) || wa.override_redirect)
    return;
  
  static constexpr auto Y_ { BARH + BDRW / 2}; // Y offset
  ::Window t { };
  T[W] = Client {
    .size = Dim { wa.width, wa.height }, 
    .pos = Dim { wa.x, wa.y < Y_ ? wa.y += Y_ : wa.y },
    .trans = static_cast<bool>(::XGetTransientForHint(x.dpy, W, &t))
  };

  static constexpr auto WMASK {
    EnterWindowMask |
    FocusChangeMask |
    PropertyChangeMask |
    StructureNotifyMask };

  ::XSelectInput(x.dpy, W, WMASK);
  ::XUngrabButton(x.dpy, AnyButton, AnyModifier, W);
  static constexpr auto BUTTONMASK { ButtonPressMask | ButtonReleaseMask };
  ::XGrabButton(x.dpy, AnyButton, AnyModifier, W, false, BUTTONMASK, GrabModeSync, GrabModeSync, None, None);

  if (curr != T.cend()) {
    const auto &PREV { curr->second };
    if (std::get<0>(PREV.pos) + std::get<0>(PREV.size) + wa.width <
        std::get<0>(x.scr)) {
      wa.x = std::get<0>(PREV.pos) + std::get<0>(PREV.size);
      wa.y = std::get<1>(PREV.pos);
    } else if (std::get<1>(PREV.pos) + std::get<1>(PREV.size) + wa.height <
        std::get<1>(x.scr)) {
      wa.x = 0;
      // 'min/maximize' std::get<1>(PREV.pos) + std::get<1>(PREV.size)
      wa.y = std::get<1>(PREV.pos) + std::get<1>(PREV.size);
    }
  }
  
  ::XMoveWindow(x.dpy, W, wa.x, wa.y);
  ::XMapWindow(x.dpy, W);
  focus(W);
  msg::DBG("End MapRequest");
}

void dobwm::Action::motionnotify(const ::Window W) {
  //msg::DBG("Event MotionNotify ");
  if (W != x.root) return;
}

void dobwm::Action::keypress(const auto STATE, const auto CODE) noexcept {
  msg::DBG("Event KeyPress ");
  const auto KMOD { STATE & x.modmask };
  const auto KSYM { ::XkbKeycodeToKeysym(x.dpy, CODE, 0, 0) };
  const auto &V { KEYS.at(KMOD).at(KSYM) };
  if (V.index() == 0) call(std::get<0>(V));
  else shcmd(std::get<1>(V));
}

void dobwm::Action::buttonpress(void) {
  msg::DBG("Event ButtonPress");
}

void dobwm::Action::enternotify(const ::Window W) {
  msg::DBG("Event EnterNotify ");
  /*
  if ((XING.mode != NotifyNormal || XING.detail == NotifyInferior) && 
      XING.window != x.root) return;
  */
  focus(W);
}

void dobwm::Action::propertynotify(const ::Window W) {
  msg::DBG("Event PropertyNotify ");
  if (W == x.root /*&& PROP.atom == XA_WM_NAME*/) {
    msg::DBG("Root property");
    for (auto &[w, c] : T) {
      if (::XTextProperty tp;
        ::XGetTextProperty(x.dpy, w, &tp, x.ATOM["WM_NAME"]) && tp.nitems) {
        c.title = reinterpret_cast<const char *>(tp.value);
      }
    }

    p.get().draw(msg::WMNAME);
  } else {
      p.get().draw(T[W].title);
  }

  /*
  if (const auto C { std::ranges::find_if(T, 
      [W](const auto &C) { return C.w == W; }) }; C < T.end()) {
    if (PROP.atom == XA_WM_NAME || 
          PROP.atom == atom.NET[static_cast<std::size_t>(Net::NAME)]) {
      msg::DBG("Prop detected");
      if (::XTextProperty tp;
        ::XGetTextProperty(x.dpy, W, &tp, atom.NET[static_cast<std::size_t>(Net::NAME)]) && tp.nitems) {
        int n;
        if (tp.encoding == XA_STRING)
          C->title = std::string { (char *) tp.value };
        else if (char **list { };
          ::XmbTextPropertyToTextList(x.dpy, &tp, &list, &n) >= Success && n > 0 && *list) {
          C->title = std::string { (char *) *list };
          ::XFreeStringList(list);
        }

        ::XFree(tp.value);
        msg::DBG(C->title);
      }
    }
  }
  */
  
  msg::DBG("End PropertyNotify");
}

void dobwm::Action::shcmd(std::string_view CMD) const { 
  if (fork() == 0) {
    ::close(ConnectionNumber(x.dpy));
    ::setsid();
    ::system(CMD.data());
  }
}

void dobwm::Action::focus(::Window w) {
  if (!T.size()) {
    prev = curr = T.cend();
    p.get().draw(msg::WMNAME);
    return;
  } else if (curr != T.cend())
      ::XSetWindowBorder(x.dpy, curr->first, static_cast<std::size_t>(INACTBDR_COLOR));

  ::XSetWindowBorder(x.dpy, w, static_cast<std::size_t>(ACTBDR_COLOR));
  ::XSetWindowBorderWidth(x.dpy, w, BDRW);
  ::XSetInputFocus(x.dpy, w, RevertToPointerRoot, CurrentTime);
  ::XChangeProperty(x.dpy, x.root, x.ATOM["_NET_WM_STATE"], XA_WINDOW, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&w), 1);
  ::XChangeProperty(x.dpy, w, x.ATOM["_NET_ACTIVE_WINDOW"], XA_WINDOW, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&w), 1);
  ::XRaiseWindow(x.dpy, w);
  prev = curr;
  curr = T.find(w);
}

void dobwm::Action::kill(void) const {
  if (curr == T.cend()) return;
  const ::Window W { curr->first };
  ::XEvent ev { ClientMessage };
  ev.xclient.window = W;
  ev.xclient.message_type = x.ATOM["WM_PROTOCOLS"];
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = x.ATOM["WM_DELETE_WINDOW"];
  //ev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(x.dpy, W, false, NoEventMask, &ev);
}

void dobwm::Action::swfocus(void) {
  if (T.size() < 2) return;
  focus(prev->first);
}

void dobwm::Action::client(const char O) {
  auto t { curr };
  if (O > 0) {
    if (std::next(t) == T.cend()) focus(T.cbegin()->first);
    else focus(std::next(t)->first);
  } else if (O < 0) {
      if (t == T.cbegin())
        focus(std::next(T.cbegin(), T.size() - 1)->first);
      else
        focus(std::next(T.cbegin(), std::distance(T.cbegin(), t) - 1)->first);
  }
}

void dobwm::Action::workspace(const unsigned char N) {
  unsigned long *wks;
/*
  ::Atom xa_ret_type;
  int ret_format;
  unsigned long ret_nitems;
  unsigned long ret_bytes_after;
  unsigned char *ret_prop;
  const auto COUNT { 
    ::XGetWindowProperty(x.dpy, curr->first, x.ATOM["_WIN_WORKSPACE"], 0, 1024, false, XA_CARDINAL, &xa_ret_type, &ret_format, &ret_nitems, &ret_bytes_after, &ret_prop) == Success };
  if (COUNT) {
    msg::DBG("DTP nitems ", ret_nitems);
    //msg::DBG("DTP ", ret_prop[0]);
    ::XFree(ret_prop);
  }
*/
  ::XEvent ev { ClientMessage };
  ev.xclient.window = x.root;
  //ev.xclient.message_type = x.ATOM["_NET_CURRENT_DESKTOP"];
  ev.xclient.message_type = x.ATOM["_WIN_WORKSPACE"];
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = 0;
  ev.xclient.data.l[1] = 0;
  ::XSendEvent(x.dpy, x.root, false, NoEventMask, &ev);
/*
  unsigned char data[1];
  data[0] = N;
  ::XChangeProperty(x.dpy, x.root, x.ATOM["_NET_WM_DESKTOP"], XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 1);
*/
  /*
  ::Atom xa_ret_type;
  int ret_format;
  unsigned long ret_nitems;
  unsigned long ret_bytes_after;
  unsigned char *ret_prop;
  if (::XGetWindowProperty(x.dpy, x.root, x.ATOM["_WIN_WORKSPACE"], 0, 1024, false, XA_CARDINAL, &xa_ret_type, &ret_format, &ret_nitems, &ret_bytes_after, &ret_prop) == Success) {
    msg::DBG("_WIN_WORKSPACE ", ret_prop[0]);
    ::XFree(ret_prop);
  }
  */
}

int main(const int ARGC, const char *ARGV[]) {
  try {
    //std::println("...");
    msg::DBG(msg::WMNAME, "ver. ", msg::VER);
    if (!x.dpy) throw std::runtime_error("Unable to open display");
    static const auto SCRN { DefaultScreen(x.dpy) };
    x.scr = dobwm::Dim { DisplayWidth(x.dpy, SCRN), DisplayHeight(x.dpy, SCRN) };
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
    ::XUngrabButton(x.dpy, AnyButton, AnyModifier, x.root);
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
    /*
    for (const auto &KEY : dobwm::KEYS)
      ::XGrabKey(x.dpy, ::XKeysymToKeycode(x.dpy, std::get<1>(KEY)), std::get<0>(KEY) & x.modmask, x.root, true, GrabModeAsync, GrabModeAsync);
    
    for (const auto &BTN : dobwm::BTNS) { }
    */
    for (const auto &MOD : dobwm::KEYS)
      for (const auto &KEY : std::get<1>(MOD))
        ::XGrabKey(x.dpy, ::XKeysymToKeycode(x.dpy, std::get<0>(KEY)), std::get<0>(MOD) & x.modmask, x.root, true, GrabModeAsync, GrabModeAsync);
    
    for (const auto &MOD : dobwm::BTNS)
      for (const auto &BTN : std::get<1>(MOD)) {
        (void) BTN;
      }

    ::XSync(x.dpy, false);
    // Init. Atoms
    static constexpr auto REGATOM { [](std::string_view MSG) {
        x.ATOM[MSG] = ::XInternAtom(x.dpy, MSG.data(), false);
      }
    };

    REGATOM("WM_PROTOCOLS");
    REGATOM("WM_NAME");
    REGATOM("WM_DELETE_WINDOW");
    REGATOM("WM_STATE");
    REGATOM("WM_TAKE_FOCUS");
    REGATOM("_NET_SUPPORTED");
    REGATOM("_NET_WM_STATE");
    REGATOM("_NET_WM_NAME");
    REGATOM("_NET_ACTIVE_WINDOW");
    REGATOM("_NET_WM_STATE_FULLSCREEN");
    REGATOM("_NET_WM_WINDOW_TYPE");
    REGATOM("_NET_WM_WINDOW_TYPE_DIALOG");
    REGATOM("_WIN_WORKSPACE_COUNT");
    REGATOM("_WIN_WORKSPACE");
    REGATOM("_NET_NUMBER_OF_DESKTOPS");
    REGATOM("_NET_WM_DESKTOP");
    REGATOM("_NET_CURRENT_DESKTOP");
////////////////////////////////////////////////////////////////////////
  unsigned char data[1];
  data[0] = dobwm::NWKS;
  ::XChangeProperty(x.dpy, x.root, x.ATOM["_WIN_WORKSPACE_COUNT"], XA_CARDINAL, 32, PropModeReplace, (unsigned char *) data, 1);

    ::Atom xa_ret_type;
    int ret_format;
    unsigned long ret_nitems;
    unsigned long ret_bytes_after;
    unsigned char *ret_prop;
    if (::XGetWindowProperty(x.dpy, x.root, x.ATOM["_WIN_WORKSPACE_COUNT"], 0, 1024, false, XA_CARDINAL, &xa_ret_type, &ret_format, &ret_nitems, &ret_bytes_after, &ret_prop) == Success) {
      msg::DBG("WIN_WORKSPACE_COUNT ", ret_prop[0]);
      //::XFree(ret_prop);
    }
    /*
    if (::XGetWindowProperty(x.dpy, x.root, x.ATOM["_NET_NUMBER_OF_DESKTOPS"], 0, 1024, false, XA_CARDINAL, &xa_ret_type, &ret_format, &ret_nitems, &ret_bytes_after, &ret_prop) == Success) {
      msg::DBG("_NET_NUMBER_OF_DESKTOPS ", ret_prop[0]);
      ::XFree(ret_prop);
    }
    */
////////////////////////////////////////////////////////////////////////
    std::signal(SIGINT, sig_handler);
    static dobwm::Panel p { SCRN };
    static dobwm::Action a { p };
    static ::XEvent xev;
    std::array<std::function<void(void)>, LASTEvent> F;  // Literal defn.
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
      (void) xev.xbutton;
      a.buttonpress();
    };
    F[EnterNotify] = [] { 
      if (xev.xcrossing.mode == NotifyNormal && 
          xev.xcrossing.detail != NotifyInferior)
        a.enternotify(xev.xcrossing.window);
    };
    F[PropertyNotify] = [] { a.propertynotify(xev.xproperty.window); };

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
    msg::DBG("WM initialized");
    msg::send("Welcome msg", dobwm::Urg::NORMAL, 1000);
    p.draw(msg::WMNAME);
    // Ev loop
    while (sig_status == 0)
      if (::XNextEvent(x.dpy, &xev) == 0) {
        F[xev.type]();
        ::XSync(x.dpy, false);
      }
    
    // Deinit
    ::XUngrabKey(x.dpy, AnyKey, AnyModifier, x.root);
    ::XUngrabButton(x.dpy, AnyButton, AnyModifier, x.root);
    ::XSetInputFocus(x.dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    ::XCloseDisplay(x.dpy);
    msg::DBG("WM exit");
  } catch (const std::exception &E) {
      msg::DBG("Ex.", E.what());
      //std::println("Ex. { }", E.what());
      return -1;
  }
  
  return 0;
}
