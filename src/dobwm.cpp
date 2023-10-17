#include <iostream>
//#include <print>
#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <sstream>
#include <functional>
#include <optional>
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
  struct X {
    int modmask { };  // This mask needs repeated updates
    ::Display *dpy { ::XOpenDisplay(nullptr) };
    ::Window root { };
  };

  using Dim = std::pair<std::size_t, std::size_t>;
  struct Client {
    std::string title;
    Dim size, pos;
    ::Window w { };
    bool trans { }, mut { }, sel { };
  };
  
  class Manage {
    struct Atom_ {
      enum class Wm : std::size_t { PROTO, NAME, DELWIN, STATE, FOCUS, Z };
      enum class Net : std::size_t { 
        SUPP, STATE, NAME, ACT, FSCRN, WTYPE, WDIALOG, Z };
      std::array<::Atom, static_cast<std::size_t>(Wm::Z)> WM;
      std::array<::Atom, static_cast<std::size_t>(Net::Z)> NET;
      //std::array<::Atom, std::to_underlying(Wm::Z)> WM;
      //std::array<::Atom, std::to_underlying(Net::CNT)> NET;
    };
    Atom_ atom;
    using Wm = Atom_::Wm;
    using Net = Atom_::Net;
    static constexpr auto Y { BAR_HEIGHT + BDR_WIDTH / 2};

    std::array<std::function<void(void)>, static_cast<std::size_t>(Calls::Z)> F;
    void call(const Calls C) const { ::DBGMSG(static_cast<std::size_t>(C)); 
      F[static_cast<std::size_t>(C)](); }
    ::Display *dpy;
    std::reference_wrapper<Panel> p;
    std::optional<::Window> prev, curr;
    //////////////////////////////////////////////////////
    void shcmd(std::string_view CMD) const;
    void focus(::Window);
    void kill(void) const;
    void swfocus(void);
    void client(const char);
    //////////////////////////////////////////////////////
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
    void keypress(const auto &);
    void buttonpress(const auto &);
    void enternotify(const auto &);
    void propertynotify(const auto &);
  };
}

static dobwm::X x;
//static std::array<std::vector<Client>, NT> T;
static std::vector<dobwm::Client> T;

dobwm::Manage::Manage(auto &P) noexcept : dpy { x.dpy }, p { P } {
  // Init. Atoms
  atom.WM[static_cast<std::size_t>(Wm::PROTO)] =
    ::XInternAtom(x.dpy, "WM_PROTOCOLS", false);
  atom.WM[static_cast<std::size_t>(Wm::NAME)] =
    ::XInternAtom(x.dpy, "WM_NAME", false);
  atom.WM[static_cast<std::size_t>(Wm::DELWIN)] =
    ::XInternAtom(x.dpy, "WM_DELETE_WINDOW", false);
  atom.WM[static_cast<std::size_t>(Wm::STATE)] =
    ::XInternAtom(x.dpy, "WM_STATE", false);
  atom.WM[static_cast<std::size_t>(Wm::FOCUS)] =
    ::XInternAtom(x.dpy, "WM_TAKE_FOCUS", false);
  atom.NET[static_cast<std::size_t>(Net::SUPP)] =
    ::XInternAtom(x.dpy, "_NET_SUPPORTED", false);
  atom.NET[static_cast<std::size_t>(Net::STATE)] =
    ::XInternAtom(x.dpy, "_NET_WM_STATE", false);
  atom.NET[static_cast<std::size_t>(Net::NAME)] =
    ::XInternAtom(x.dpy, "_NET_WM_NAME", false);
  atom.NET[static_cast<std::size_t>(Net::ACT)] =
    ::XInternAtom(x.dpy, "_NET_ACTIVE_WINDOW", false);
  atom.NET[static_cast<std::size_t>(Net::FSCRN)] =
    ::XInternAtom(x.dpy, "_NET_WM_STATE_FULLSCREEN", false);
  atom.NET[static_cast<std::size_t>(Net::WTYPE)] =
    ::XInternAtom(x.dpy, "_NET_WM_WINDOW_TYPE", false);
  atom.NET[static_cast<std::size_t>(Net::WDIALOG)] =
    ::XInternAtom(x.dpy, "_NET_WM_WINDOW_TYPE_DIALOG", false);

  ::XChangeProperty(x.dpy, x.root, atom.NET[static_cast<std::size_t>(Net::SUPP)], XA_ATOM, 32, PropModeReplace, reinterpret_cast<unsigned char *>(atom.NET.data()), static_cast<std::size_t>(Net::Z));
  ::XSync(x.dpy, false);

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

dobwm::Manage::~Manage(void) noexcept {
  ::XDeleteProperty(x.dpy, x.root, atom.NET[static_cast<std::size_t>(Net::ACT)]);
}

void dobwm::Manage::mapnotify(void) {
  ::DBGMSG("Event MapNotify");
}

void dobwm::Manage::unmapnotify(const auto &UMAP) {
  ::DBGMSG("Event UnmapNotify");
  const auto C { std::ranges::find_if(T, 
    [W = UMAP.window](const auto &C) { return C.w == W; }) };
  if (C < T.end()) {
    //curr.emplace(prev.value());
    //focus(curr.value());
    client(-1);
    T.erase(C);
  }
}

void dobwm::Manage::clientmessage(const auto &CMSG) {
  ::DBGMSG("Event ClientMessage ");
  const auto C { std::ranges::find_if(T, 
    [W = CMSG.window](const auto &C) { return C.w == W; }) };
  if (C < T.end()) {
    // Handle messages
    /*
    if (CMSG.message_type == atom.WM[static_cast<std::size_t>(Wm::PROTO)] &&
        CMSG.data.l[0] == atom.WM[static_cast<std::size_t>(Wm::DELWIN)]) {
    
    } else if (true) {
        // Handle other msgs
    }
    */
  }
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

  else if (std::ranges::find_if(T, [W](const auto &C) { return C.w == W; }) == T.end()) {
    ::Window t { };
    T.emplace_back(Client { 
      .size = Dim { wa.width, wa.height }, 
      .pos = Dim { wa.x, wa.y < Y ? wa.y += Y : wa.y },
      .w = W, 
      .trans = static_cast<bool>(::XGetTransientForHint(x.dpy, W, &t)) });
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
    focus(T.back().w);
  }

  //::XSync(dpy, false);
  ::DBGMSG("End MapRequest");
}

void dobwm::Manage::configurerequest(const auto &CREQ) {
  ::DBGMSG("Event ConfigureRequest");
  ::XWindowChanges wc {
      CREQ.x, CREQ.y, CREQ.width, CREQ.height,
      CREQ.border_width, CREQ.above, CREQ.detail };
  ::XConfigureWindow(dpy, CREQ.window, CREQ.value_mask, &wc);
  /*
  if (::XConfigureWindow(dpy, CREQ.window, CREQ.value_mask, &wc))
    ::XSync(dpy, false);
  */
}

void dobwm::Manage::motionnotify(const auto &MOTN) {
  ::DBGMSG("Event MotionNotify ");
  if (MOTN.window != x.root) return;
}

void dobwm::Manage::keypress(const auto &KEY) {
  ::DBGMSG("Event KeyPress ");
  const auto KMOD { KEY.state & x.modmask };
  const auto KSYM { ::XkbKeycodeToKeysym(dpy, KEY.keycode, 0, 0) };
  for (const auto &KEY : KEYS)
    if (std::get<1>(KEY) == KSYM && (std::get<0>(KEY) & x.modmask) == KMOD) {
      const auto &V { std::get<2>(KEY) };
      if (V.index() == 0) call(std::get<0>(V));
      else shcmd(std::get<1>(V));
      return;
    }
}

void dobwm::Manage::buttonpress(const auto &BTNP) {
  ::DBGMSG("Event ButtonPress");
}

void dobwm::Manage::enternotify(const auto &XING) {
  ::DBGMSG("Event EnterNotify ");
  if ((XING.mode != NotifyNormal || XING.detail == NotifyInferior) && 
      XING.window != x.root) return;
  if (const auto C { std::ranges::find_if(T, 
      [W = XING.window](const auto &C) { return C.w == W; }) }; C < T.end())
    focus(C->w);
}

void dobwm::Manage::propertynotify(const auto &PROP) {
  ::DBGMSG("Event PropertyNotify ");
  const auto W { PROP.window };
  if (W == x.root /*&& PROP.atom == XA_WM_NAME*/) {
    ::DBGMSG("Root property");
    /*
    std::string t;
    for (const auto &C : T)
      t += C.title + "\t";
    */
    p.get().draw("Root Property");
  } else {
    // Handle W
    
  }

  const auto WM_NAME { atom.NET[static_cast<std::size_t>(Wm::NAME)] };
  const auto NET_NAME { atom.NET[static_cast<std::size_t>(Net::NAME)] };
  ::DBGMSG("Property Window ", W);
  ::DBGMSG("WM_NAME ", WM_NAME);
  ::DBGMSG("_NET_WM_NAME ", NET_NAME);
  ::DBGMSG("XA_WM_NAME ", XA_WM_NAME);
  ::DBGMSG("Window Property atom ", PROP.atom);

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

//////////////////////////////////////////////////////////////////////////
// Private Members  //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void dobwm::Manage::shcmd(std::string_view CMD) const { 
  if (fork() == 0) {
    ::close(ConnectionNumber(x.dpy));
    ::setsid();
    ::system(CMD.data());
  }
}

void dobwm::Manage::focus(::Window w) {
  if (!T.size()) {
    ::XDeleteProperty(x.dpy, x.root, atom.NET[static_cast<std::size_t>(Net::ACT)]);
    prev = curr = std::nullopt;
    return;
  } else if (curr.has_value()) {
    ::XSetWindowBorder(x.dpy, curr.value(), static_cast<std::size_t>(INACTBDR_COLOR));
    prev.emplace(curr.value());
  }

  ::XSetWindowBorder(x.dpy, w, static_cast<std::size_t>(ACTBDR_COLOR));
  ::XSetWindowBorderWidth(x.dpy, w, BDR_WIDTH);
  ::XSetInputFocus(x.dpy, w, RevertToPointerRoot, CurrentTime);
  ::XChangeProperty(x.dpy, x.root, atom.NET[static_cast<std::size_t>(Net::ACT)], XA_WINDOW, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&w), 1);
  ::XRaiseWindow(dpy, w);
  curr.emplace(w);
}

void dobwm::Manage::kill(void) const {
  if (!T.size()) return;
  /*
  ::Window w;
  int revert_to;
  ::XGetInputFocus(dpy, &w, &revert_to);
  if (w == None) return;
  */
  const ::Window W { curr.value() };
  ::XEvent ev { .type = ClientMessage };
  ev.xclient.window = W;
  ev.xclient.format = 32;
  ev.xclient.message_type = atom.WM[static_cast<std::size_t>(Wm::PROTO)];
  ev.xclient.data.l[0] = atom.WM[static_cast<std::size_t>(Wm::DELWIN)];
  ev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(dpy, W, false, NoEventMask, &ev);
}

void dobwm::Manage::swfocus(void) {
  if (T.size() < 2) return;
  focus(prev.value());
}

void dobwm::Manage::client(const char O) {
  if (T.size() < 2) return;
  const auto C { std::ranges::find_if(T, 
      [&](const auto &C) { return C.w == curr.value(); }) + O };
  if (C == T.end()) focus(T.front().w);
  else if (C < T.begin()) focus(T.back().w);
  else focus(C->w);
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
    
    for (const auto &KEY : dobwm::KEYS)
      ::XGrabKey(x.dpy, ::XKeysymToKeycode(x.dpy, std::get<1>(KEY)), std::get<0>(KEY) & x.modmask, x.root, true, GrabModeAsync, GrabModeAsync);
    
    for (const auto &BTN : dobwm::BTNS) { }

    ::XSync(x.dpy, false);
    dobwm::Panel p { x.dpy, x.root, SCR, dobwm::BAR_HEIGHT };
    std::signal(SIGINT, sig_handler);
    static dobwm::Manage m { p };

    // Pickup clients
    unsigned n;
    ::Window root, parent, *w { };
    if (::XQueryTree(x.dpy, x.root, &root, &parent, &w, &n)) {
      for (unsigned i { }; i < n; i++) {
        ::XWindowAttributes wa;
        if (::XGetWindowAttributes(x.dpy, w[i], &wa) && wa.map_state == IsViewable) {
          // Send as native event
          // ...
          ::XMapRequestEvent ev { .window = w[i] };
          m.maprequest(ev);
        }
      }

      if (w) ::XFree(w);
    }
    
    ::DBGMSG("T.size() ", T.size());
    // Ev loop
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
    ::DBGMSG("WM initialized");
    ::MSG("Welcome msg", dobwm::Urg::NORMAL, 1000);
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
