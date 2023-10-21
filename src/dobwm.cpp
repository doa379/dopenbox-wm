#include <iostream>
//#include <print>
#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <sstream>
#include <functional>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <dobwm.h>
#include <msg.h>
#include <panel.h>
#include <../config.h>

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
    else
      arg << std::to_string(t) << " ";
    msg(args...);
  }
  
  auto msg(void) {
    std::cout << arg.ss.str() << "\n";
    arg.ss.clear();
  }
};

static dobwm::Msg msg;

template<typename ...Args>
auto DBGMSG(const Args &...args) {
  Dbg dbg;
  dbg.msg(args...);
}
  
auto MSG(std::string_view MSG, const dobwm::Urg URG, const unsigned TO) {
  msg.send("Dopenbox WM", MSG, URG, TO);
}

namespace dobwm {
  struct X {  // Base class
    int modmask { };  // This mask needs repeated updates
    static inline ::Display *dpy { ::XOpenDisplay(nullptr) };
    static inline std::unordered_map<std::string_view, ::Atom> ATOM;
    static constexpr auto REGATOM { [] (std::string_view MSG) {
        ATOM[MSG] = ::XInternAtom(dpy, MSG.data(), false);
      }
    };
    
    ::Window root { };
  };

  using Dim = std::pair<std::size_t, std::size_t>;
  struct Client {
    std::string title;
    Dim size, pos;
    bool trans { }, mut { }, sel { };
  };

  class Action {
    std::array<std::function<void(void)>, static_cast<std::size_t>(Calls::Z)> F;
    std::unordered_map<::Window, Client>::const_iterator prev, curr;
  public:
    Action(void) noexcept;
    ~Action(void) noexcept;
    void call(const Calls C) const { ::DBGMSG(static_cast<std::size_t>(C)); 
      F[static_cast<std::size_t>(C)](); }
    void shcmd(std::string_view CMD) const;
    void focus(::Window);
    void kill(void) const;
    void swfocus(void);
    void client(const char);
  };
  
  class Manage {
    static constexpr auto Y { BAR_HEIGHT + BDR_WIDTH / 2};
    ::Display *dpy;
    std::reference_wrapper<Panel> p;
    Action a;
  public:
    Manage(auto &) noexcept;
    ~Manage(void) noexcept;
    void mapnotify(void);
    void unmapnotify(const auto &);
    void clientmessage(const auto &);
    void configurenotify(const auto &);
    void maprequest(const auto &);
    void configurerequest(const auto &);
    void motionnotify(const auto &);
    void keypress(const auto &) noexcept;
    void buttonpress(const auto &);
    void enternotify(const auto &);
    void propertynotify(const auto &);
  };
}

static dobwm::X x;
static std::unordered_map<::Window, dobwm::Client> T;

dobwm::Manage::Manage(auto &P) noexcept : dpy { x.dpy }, p { P } {

}

dobwm::Manage::~Manage(void) noexcept {

}

void dobwm::Manage::mapnotify(void) {
  ::DBGMSG("Event MapNotify");
}

void dobwm::Manage::unmapnotify(const auto &UMAP) {
  ::DBGMSG("Event UnmapNotify");
  const auto W { UMAP.window };
  if (T.find(W) != T.end()) {
    a.client(-1);
    T.erase(W);
  }
}

void dobwm::Manage::clientmessage(const auto &CMSG) {
  ::DBGMSG("Event ClientMessage ");
  const auto W { CMSG.window };
  
  // Handle messages
}

void dobwm::Manage::configurenotify(const auto &CONF) {
  ::DBGMSG("Event ConfigureNotify");
}

void dobwm::Manage::maprequest(const auto &MREQ) {
  ::DBGMSG("Event MapRequest");
  static ::XWindowAttributes wa;
  const ::Window W { MREQ.window };
  ::DBGMSG("Window ", W);
  if (!::XGetWindowAttributes(dpy, W, &wa) || wa.override_redirect)
    return;
  
  ::Window t { };
  T[W] = Client {
    .size = Dim { wa.width, wa.height }, 
    .pos = Dim { wa.x, wa.y < Y ? wa.y += Y : wa.y },
    .trans = static_cast<bool>(::XGetTransientForHint(x.dpy, W, &t))
  };

  static constexpr auto WMASK {
    EnterWindowMask |
    FocusChangeMask |
    PropertyChangeMask |
    StructureNotifyMask };

  ::XSelectInput(dpy, W, WMASK);
  ::XUngrabButton(dpy, AnyButton, AnyModifier, W);
  static constexpr auto BUTTONMASK { ButtonPressMask | ButtonReleaseMask };
  ::XGrabButton(dpy, AnyButton, AnyModifier, W, false, BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
  ::XMoveWindow(x.dpy, W, 0, wa.y);
  ::XMapWindow(dpy, W);
  a.focus(W);

  ::DBGMSG("End MapRequest");
}

void dobwm::Manage::configurerequest(const auto &CREQ) {
  ::DBGMSG("Event ConfigureRequest");
  ::XWindowChanges wc {
      CREQ.x, CREQ.y, CREQ.width, CREQ.height,
      CREQ.border_width, CREQ.above, CREQ.detail };
  ::XConfigureWindow(dpy, CREQ.window, CREQ.value_mask, &wc);
}

void dobwm::Manage::motionnotify(const auto &MOTN) {
  ::DBGMSG("Event MotionNotify ");
  if (MOTN.window != x.root) return;
}

void dobwm::Manage::keypress(const auto &KEY) noexcept {
  ::DBGMSG("Event KeyPress ");
  const auto KMOD { KEY.state & x.modmask };
  const auto KSYM { ::XkbKeycodeToKeysym(dpy, KEY.keycode, 0, 0) };
  const auto &V { KEYS.at(KMOD).at(KSYM) };
  if (V.index() == 0) a.call(std::get<0>(V));
  else a.shcmd(std::get<1>(V));
}

void dobwm::Manage::buttonpress(const auto &BTNP) {
  ::DBGMSG("Event ButtonPress");
}

void dobwm::Manage::enternotify(const auto &XING) {
  ::DBGMSG("Event EnterNotify ");
  if ((XING.mode != NotifyNormal || XING.detail == NotifyInferior) && 
      XING.window != x.root) return;
  
  a.focus(XING.window);
}

void dobwm::Manage::propertynotify(const auto &PROP) {
  ::DBGMSG("Event PropertyNotify ");
  const auto W { PROP.window };
  if (W == x.root /*&& PROP.atom == XA_WM_NAME*/) {
    ::DBGMSG("Root property");
    for (auto &[w, c] : T) {
      if (::XTextProperty tp;
        ::XGetTextProperty(x.dpy, w, &tp, x.ATOM["WM_NAME"]) && tp.nitems) {
        c.title = reinterpret_cast<const char *>(tp.value);
      }
    }

    p.get().draw("Root Property");
  } else {
      p.get().draw(T[W].title);
  }

  /*
  if (const auto C { std::ranges::find_if(T, 
      [W](const auto &C) { return C.w == W; }) }; C < T.end()) {
    if (PROP.atom == XA_WM_NAME || 
          PROP.atom == atom.NET[static_cast<std::size_t>(Net::NAME)]) {
      ::DBGMSG("Prop detected");
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
        ::DBGMSG(C->title);
      }
    }
  }
  */
  
  ::DBGMSG("End PropertyNotify");
}

dobwm::Action::Action(void) noexcept {
  for (auto &f : F) f = [] { };
  F[static_cast<std::size_t>(Calls::QUIT)] = [] { 
    std::raise(SIGINT); 
  };
  F[static_cast<std::size_t>(Calls::UNMAPALL)] = [] { };
  F[static_cast<std::size_t>(Calls::REMAPALL)] = [] { };
  F[static_cast<std::size_t>(Calls::KILL)] = [&] { kill(); };
  F[static_cast<std::size_t>(Calls::SWFOCUS)] = [&] { swfocus(); };
  F[static_cast<std::size_t>(Calls::PREVCLI)] = [&] { client(-1); };
  F[static_cast<std::size_t>(Calls::NEXTCLI)] = [&] { client(+1); };
  F[static_cast<std::size_t>(Calls::SELTOGGLE)] = [] { };
  F[static_cast<std::size_t>(Calls::SELCLEAR)] = [] { };
  F[static_cast<std::size_t>(Calls::MOVEUP)] = [] { 
    ::DBGMSG("Move Up"); };
  F[static_cast<std::size_t>(Calls::MOVEDOWN)] = [] { };
  F[static_cast<std::size_t>(Calls::MOVELEFT)] = [] { };
  F[static_cast<std::size_t>(Calls::MOVERIGHT)] = [] { };
  F[static_cast<std::size_t>(Calls::RESIZEVINC)] = [] {
    ::DBGMSG("Resize V+"); };
  F[static_cast<std::size_t>(Calls::RESIZEVDEC)] = [] { };
  F[static_cast<std::size_t>(Calls::RESIZEHDEC)] = [] { };
  F[static_cast<std::size_t>(Calls::RESIZEHINC)] = [] { };
  F[static_cast<std::size_t>(Calls::SELECT)] = [] { };
  F[static_cast<std::size_t>(Calls::RESIZE)] = [] { };
}

dobwm::Action::~Action(void) noexcept {

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
    ::XDeleteProperty(x.dpy, x.root, x.ATOM["_NET_ACTIVE_WINDOW"]);
    prev = curr = T.cend();
    return;
  } else if (curr != T.cend()) {
      ::XSetWindowBorder(x.dpy, curr->first, static_cast<std::size_t>(INACTBDR_COLOR));
      prev = curr;
  }

  ::XSetWindowBorder(x.dpy, w, static_cast<std::size_t>(ACTBDR_COLOR));
  ::XSetWindowBorderWidth(x.dpy, w, BDR_WIDTH);
  ::XSetInputFocus(x.dpy, w, RevertToPointerRoot, CurrentTime);
  ::XChangeProperty(x.dpy, x.root, x.ATOM["_NET_WM_STATE"], XA_WINDOW, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&w), 1);
  ::XChangeProperty(x.dpy, w, x.ATOM["_NET_ACTIVE_WINDOW"], XA_WINDOW, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&w), 1);
  ::XRaiseWindow(x.dpy, w);
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
  ev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(x.dpy, W, false, NoEventMask, &ev);
}

void dobwm::Action::swfocus(void) {
  if (T.size() < 2) return;
  focus(prev->first);
}

void dobwm::Action::client(const char O) {
  if (T.size() < 2) return;
  auto t { curr };
  if (O > 0) {
    if (std::next(t) == T.cend()) focus(T.cbegin()->first);
    else focus(std::next(t)->first);
  } else if (O < 0) {
      if (t == T.begin())
        focus(std::next(T.begin(), T.size() - 1)->first);
      else
        focus(std::next(T.cbegin(), std::distance(T.cbegin(), t) - 1)->first);
  }
}

volatile std::sig_atomic_t sig_status;

auto sig_handler(int sig) {
  sig_status = sig;
}

static bool error;

auto XError(::Display *, ::XErrorEvent *ev) {
  error = ev->error_code == BadAccess;
  return 0;
}

int main(const int ARGC, const char *ARGV[]) {
  try {
    //std::println("...");
    ::DBGMSG("Dopenbox Window Manager ver.", VER);
    if (!x.dpy) throw std::runtime_error("Unable to open display");

    static const auto SCR { DefaultScreen(x.dpy) };
    x.REGATOM("WM_PROTOCOLS");
    x.REGATOM("WM_NAME");
    x.REGATOM("WM_DELETE_WINDOW");
    x.REGATOM("WM_STATE");
    x.REGATOM("WM_TAKE_FOCUS");
    x.REGATOM("_NET_SUPPORTED");
    x.REGATOM("_NET_WM_STATE");
    x.REGATOM("_NET_WM_NAME");
    x.REGATOM("_NET_ACTIVE_WINDOW");
    x.REGATOM("_NET_WM_STATE_FULLSCREEN");
    x.REGATOM("_NET_WM_WINDOW_TYPE");
    x.REGATOM("_NET_WM_WINDOW_TYPE_DIALOG");
    // Root window
    x.root = RootWindow(x.dpy, SCR);
    ::DBGMSG("Root Window ", x.root);
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
    if (error) {
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
      for (const auto &KEY : std::get<1>(MOD)) { }

    ::XSync(x.dpy, false);
    dobwm::Panel p { x.dpy, x.root, SCR, dobwm::BAR_HEIGHT };
    std::signal(SIGINT, sig_handler);
    static dobwm::Manage m { p };

    static ::XEvent ev;
    std::array<std::function<void(void)>, LASTEvent> F;  // Literal defn.
    for (auto &f : F) f = [] { };
    F[MapNotify] = [] { m.mapnotify(); };
    F[UnmapNotify] = [] { m.unmapnotify(ev.xunmap); };
    F[ClientMessage] = [] { m.clientmessage(ev.xclient); };
    F[ConfigureNotify] = [] { m.configurenotify(ev.xconfigure); };
    F[MapRequest] = [] { m.maprequest(ev.xmaprequest); };
    F[ConfigureRequest] = [] { m.configurerequest(ev.xconfigurerequest); };
    //F[MotionNotify] = [] { m.motionnotify(ev.xmotion); };
    F[KeyPress] = [] { m.keypress(ev.xkey); };
    F[ButtonPress] = [] { m.buttonpress(ev.xbutton); };
    F[EnterNotify] = [] { m.enternotify(ev.xcrossing); };
    F[PropertyNotify] = [] { m.propertynotify(ev.xproperty); };

    unsigned n;
    ::Window root, parent, *w { };
    if (::XQueryTree(x.dpy, x.root, &root, &parent, &w, &n)) {
      for (unsigned i { }; i < n; i++) {
        ::XWindowAttributes wa;
        if (::XGetWindowAttributes(x.dpy, w[i], &wa) && wa.map_state == IsViewable) {
          // Send as native event
          ::XEvent ev { MapRequest };
          ev.xmaprequest.send_event = true,
          ev.xmaprequest.parent = x.root;
          ev.xmaprequest.window = w[i];
          ::XSendEvent(x.dpy, x.root, true, ROOTMASK, &ev);
        }
      }

      if (w) ::XFree(w);
    }
    
    ::XSync(x.dpy, false);
    ::DBGMSG("WM initialized");
    ::MSG("Welcome msg", dobwm::Urg::NORMAL, 1000);
    // Ev loop
    while (!sig_status)
      if (::XNextEvent(x.dpy, &ev) == 0) {
        F[ev.type]();
        ::XSync(x.dpy, false);
      }
    
    // Deinit
    ::XUngrabKey(x.dpy, AnyKey, AnyModifier, x.root);
    ::XUngrabButton(x.dpy, AnyButton, AnyModifier, x.root);
    ::XSetInputFocus(x.dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    ::XCloseDisplay(x.dpy);
    ::DBGMSG("WM exit");
  } catch (const std::exception &E) {
      ::DBGMSG("Ex.", E.what());
      //std::println("Ex. { }", E.what());
      return -1;
  }
  
  return 0;
}
