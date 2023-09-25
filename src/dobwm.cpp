#include <iostream>
//#include <print>
#include <algorithm>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <sstream>
#include <functional>
#include <X11/Xatom.h>
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

/*
dobwm::Box::Box(void) {
  for (const auto &D : x.MONS()) {
    std::vector<Tag> T { Nt };
    Mon m { T, D };
    M.emplace_back(std::move(m));
  }
  
  for (const auto W : x.query_tree()) {
    map_request(W);
    ::DBGMSG("Init w/ client", W);
  }
  
  for (const auto &CMDS_ : { CMDS, CMDS_ASYNC })
    for (const auto &CMD : CMDS_) {
      const Kb &KB { std::get<0>(CMD) };
      x.grab_key(std::get<0>(KB), static_cast<unsigned long>(std::get<1>(KB)));
    }
}

auto dobwm::Box::focus(Hnd &H) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) {
        x.client(H.win, BDR_WIDTH, static_cast<unsigned long>(INACTBDR_COLOR)); });

  if (x.focus(H.win) &&
      x.client(H.win, BDR_WIDTH, static_cast<unsigned long>(ACTBDR_COLOR))) {
    curr = HndRef { std::ref(H) };
    ::DBGMSG("Focus.", curr->get().win);
    if (curr->get().res.has_value())
      print_hint(curr->get().res.value());
  }
}

auto dobwm::Box::sw_focus(void) {
  for (auto &m : M)
    for (auto &t : m.T)
      for (auto h { t.H.begin() }; h < t.H.end(); h++)
        if (*h == curr->get()) {
          if (h < t.H.end() - 1) {
            focus(*(h + 1));
            curr = HndRef { std::ref(*(h + 1)) };
            return;
          } else if (h == t.H.end() - 1) {
            focus(*t.H.begin());
            curr = HndRef { std::ref(*t.H.begin()) };
            return;
          }
        }
}
  
	if (XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
		for (i = 0; i < num; i++) {
			if (!XGetWindowAttributes(dpy, wins[i], &wa)
			|| wa.override_redirect || XGetTransientForHint(dpy, wins[i], &d1))
				continue;
			if (wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
				manage(wins[i], &wa);
		}
		for (i = 0; i < num; i++) { // now the transients
			if (!XGetWindowAttributes(dpy, wins[i], &wa))
				continue;
			if (XGetTransientForHint(dpy, wins[i], &d1)
			&& (wa.map_state == IsViewable || getstate(wins[i]) == IconicState))
				manage(wins[i], &wa);
		}
		if (wins)
			XFree(wins);
	}

	if (!XGetWindowAttributes(dpy, ev->window, &wa))
		return;
	if (wa.override_redirect)
		return;
	if (!wintoclient(ev->window))
		manage(ev->window, &wa);

auto dobwm::Box::map_request(const ::Window W) -> void {
  ::Window tra { };
  if (const auto D { x.client_dim(W) }; D.has_value() && x.map_window(W)) {
    M.back().T.back().H.emplace_back(Hnd { 
      W, D.value(), true, { }, x.client_hint(W) });
    focus(M.back().T.back().H.back());
    ////
    ::DBGMSG("Mapped.", W);
    if (M.back().T.back().H.back().res.has_value())
      print_hint(M.back().T.back().H.back().res.value());


  } else if (const auto D { x.trans_dim(W) }; D.has_value() &&
      x.client_trans(W, tra) && x.map_window(W) && x.map_window(tra)) {
    M.back().T.back().H.emplace_back(Hnd { W, D.value(), true });
    M.back().T.back().H.emplace_back(Hnd { tra, D.value(), false });
    focus(M.back().T.back().H.back());
  }
}

auto dobwm::Box::map_all(void) const {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) {
        x.map_window(H.win); });
}

auto dobwm::Box::unmap_all(void) const {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) {
        x.unmap_window(H.win); });
}

auto dobwm::Box::del_hnd(const Hnd &H) {
  for (auto &m : M)
    for (auto &t : m.T)
      if (const auto H_ { std::ranges::find(t.H, H) }; H_ < t.H.end()) {
          t.H.erase(H_);
          return;
      }
}

auto dobwm::Box::key(void) {
  const Kb KB { x.key_state(), static_cast<Key>(x.key_press(x.key_code())) };
  for (const auto &CMD : CMDS)
    if (KB == std::get<0>(CMD)) {
      if (std::get<1>(CMD) == "QUIT") {
          ::DBGMSG("WM exit");
          quit = true;
      } else if (std::get<1>(CMD) == "KILLCLI" && curr.has_value()) {
          ::DBGMSG("Kill Curr");
          const auto W { curr->get().win };
          // Take reverse focus instead
          sw_focus();
          if (x.kill_client(W))
            del_hnd(*curr);
      } else if (std::get<1>(CMD) == "UNMAPALL" && curr.has_value()) {
          ::DBGMSG("Unmap all");
          unmap_all();
      } else if (std::get<1>(CMD) == "REMAPALL" && curr.has_value()) {
          ::DBGMSG("Remap all");
          map_all();
      } else if (std::get<1>(CMD) == "SWFOCUS" && curr.has_value())
          sw_focus();
      else if (std::get<1>(CMD) == "MOVEUP" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x, curr->get().d.y -= MOVESTEP_PX);
      else if (std::get<1>(CMD) == "MOVEDOWN" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x, curr->get().d.y += MOVESTEP_PX);
      else if (std::get<1>(CMD) == "MOVELEFT" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x -= MOVESTEP_PX, curr->get().d.y);
      else if (std::get<1>(CMD) == "MOVERIGHT" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x += MOVESTEP_PX, curr->get().d.y);
      else {
        ::system(std::string(std::get<1>(CMD)).c_str());
        return;
      }
    }
  
  for (const auto &CMD : CMDS_ASYNC)
    if (KB == std::get<0>(CMD)) {
      // fork() on std::get<1>(CMD)
      return;
    }
}

auto dobwm::Box::configure_request(void) {
  ::DBGMSG("Ev.", "Config Req Event");
  //auto &ev { x.configure_request() };
  //x.configure_window(ev);
}

auto dobwm::Box::unmap_request(void) {
  const auto W { x.unmap_notify() };
  x.unmap_window(W);
}

auto dobwm::Box::cli_msg(void) const {
  ::DBGMSG("Ev.", "Client Msg Event");
  //x.kill_msg();
}

auto dobwm::Box::hnd(const ::Window W) -> HndRef {
  for (auto &m : M)
    for (auto &t : m.T)
      if (auto h { 
          std::ranges::find_if(t.H, [&](const Hnd &H) -> bool {
            return H.win == W; }) }; h < t.H.end())
        return *h;

  return std::nullopt;
}

auto dobwm::Box::enter_notify(void) {
  if (SLOPPY_FOCUS && curr.has_value() &&
      x.crossing_window() != curr->get().win)
    focus(*hnd(x.crossing_window()));
}

auto dobwm::Box::button(void) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) { 
        x.grab_button(H.win, std::get<0>(SELECT), 
          static_cast<unsigned>(std::get<1>(SELECT)));
      });

  const Btn B { x.button_state(), static_cast<Button>(x.button()) }; 
  if (B == SELECT) {
    const auto H { hnd(x.button_window()) };
    if (H.has_value() && H->get() != *curr)
      focus(H->get());
  } else if (B == RESIZE) {
    ::DBGMSG("Ev.", "Resize button");
  }
  
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) { 
        x.ungrab_button(H.win, std::get<0>(SELECT),
          static_cast<unsigned>(std::get<1>(SELECT)));
      });
}
*/
namespace dobwm {
  struct X {
    int modmask { };
    ::Display *dpy { ::XOpenDisplay(nullptr) };
    ::Window root { };
  };

  struct Client {
    ::Window w { };
    std::pair<unsigned, unsigned> size;
    std::pair<int, int> pos;
    bool mut { }, sel { };
  };
  
  class Action {
    ::Display *dpy;
    std::array<std::function<void(void)>, static_cast<std::size_t>(Calls::Z)> F;
  public:
    Action(const auto) noexcept;
    void call(const Calls C) const { ::DBGMSG(static_cast<std::size_t>(C)); 
      F[static_cast<std::size_t>(C)](); }
    void shcmd(std::string_view CMD) const { if (fork() == 0) {
      ::close(ConnectionNumber(dpy));
      ::system(CMD.data()); } }
  };

  class Manage {
    struct Atom_ {
      enum class Wm : std::size_t { PROTO, DELWIN, STATE, FOCUS, Z };
      enum class Net : std::size_t { 
        /*SUPP, NAME,*/ STATE, ACT, FSCRN, /*CLIS,*/ WTYPE, WDIALOG, Z };
      std::array<::Atom, static_cast<std::size_t>(Wm::Z)> WM;
      //std::array<::Atom, std::to_underlying(Wm::Z)> WM;
      std::array<::Atom, static_cast<std::size_t>(Net::Z)> NET;
      //std::array<::Atom, std::to_underlying(Net::CNT)> NET;
    };

    ::Display *dpy;
    Atom_ atom;
    static constexpr auto WMASK {
      EnterWindowMask |
      FocusChangeMask |
      PropertyChangeMask |
      StructureNotifyMask };
  public:
    Manage(const auto) noexcept;
    void client(const ::Window, const auto &);
    void configure(const Client &);
    ::Atom atomprop(const ::Window, ::Atom);
  };

  class Ev {
    std::reference_wrapper<X> x;
    std::array<std::function<void(void)>, LASTEvent> F;  // Literal defn.
    Action a { x.get().dpy };
    Manage m { x.get().dpy };
    ::XEvent ev;
  public:
    Ev(void) = delete;
    explicit Ev(X &);
    auto next(void) { return ::XNextEvent(x.get().dpy, &ev); }
    void mapnotify(void);
    void unmapnotify(void);
    void clientmessage(void);
    void configurenotify(void);
    void maprequest(void);
    void configurerequest(void);
    void motionnotify(void);
    void keypress(void);
    void buttonpress(void);
    void enternotify(void);
    void call(void) { F[ev.type](); }
  };
}

//static std::array<std::vector<Client>, NT> T;
static std::vector<dobwm::Client> C;

//dobwm::Action::Action(X &x) : x { x } {
dobwm::Action::Action(const auto DPY) noexcept : dpy { DPY } {
  for (auto &f : F)
    f = [] { };

  F[static_cast<std::size_t>(Calls::QUIT)] = [] { 
    std::raise(SIGINT); };
  F[static_cast<std::size_t>(Calls::UNMAPALL)] = [] { };
  F[static_cast<std::size_t>(Calls::REMAPALL)] = [] { };
  F[static_cast<std::size_t>(Calls::KILL)] = [] { };
  F[static_cast<std::size_t>(Calls::SWFOCUS)] = [] { };
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

dobwm::Manage::Manage(const auto DPY) noexcept : dpy { DPY } {
  // Init. Atoms
  using Wm = Atom_::Wm;
  atom.WM[static_cast<std::size_t>(Wm::PROTO)] =
    ::XInternAtom(dpy, "WM_PROTOCOLS", false);
  atom.WM[static_cast<std::size_t>(Wm::DELWIN)] =
    ::XInternAtom(dpy, "WM_DELETE_WINDOW", false);
  atom.WM[static_cast<std::size_t>(Wm::STATE)] =
    ::XInternAtom(dpy, "WM_STATE", false);
  atom.WM[static_cast<std::size_t>(Wm::FOCUS)] =
    ::XInternAtom(dpy, "WM_TAKE_FOCUS", false);
  using Net = Atom_::Net;
  /*
  atom.NET[static_cast<std::size_t>(Net::SUPP)] =
    ::XInternAtom(dpy, "_NET_SUPPORTED", false);
  atom.NET[static_cast<std::size_t>(Net::NAME)] =
    ::XInternAtom(dpy, "_NET_WM_NAME", false);
  */
  atom.NET[static_cast<std::size_t>(Net::STATE)] =
    ::XInternAtom(dpy, "_NET_WM_STATE", false);
  atom.NET[static_cast<std::size_t>(Net::ACT)] =
    ::XInternAtom(dpy, "_NET_ACTIVE_WINDOW", false);
  atom.NET[static_cast<std::size_t>(Net::FSCRN)] =
    ::XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", false);
  /*
  atom.NET[static_cast<std::size_t>(Net::CLIS)] =
    ::XInternAtom(dpy, "_NET_CLIENT_LIST", false);
  */
  atom.NET[static_cast<std::size_t>(Net::WTYPE)] =
    ::XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", false);
  atom.NET[static_cast<std::size_t>(Net::WDIALOG)] =
    ::XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", false);
}

void dobwm::Manage::client(const ::Window W, const auto &WA) {
  C.emplace_back(Client { W, { WA.width, WA.height }, { WA.x, WA.y } });
  ::XWindowChanges wc;
  wc.border_width = BDR_WIDTH;
  ::XConfigureWindow(dpy, W, CWBorderWidth, &wc);
  ::XSetWindowBorder(dpy, W, static_cast<std::size_t>(INACTBDR_COLOR));
  /*
  configure(C.back());
  ::Atom state { atomprop(W, atom.NET[static_cast<std::size_t>(Atom_::Net::STATE)]) };
  if (state == atom.NET[static_cast<std::size_t>(Atom_::Net::FSCRN)])
    ;// set FS
  */
  // Detect WM Hints
  ::XSelectInput(dpy, W, WMASK);
  // Grab Buttons for W
  ::XRaiseWindow(dpy, W);
  ::XMapWindow(dpy, W);
}

void dobwm::Manage::configure(const Client &C) {
  ::XConfigureEvent ce;
  ce.type = ConfigureNotify;
  ce.display = dpy;
  ce.event = C.w;
  ce.window = C.w;
  ce.x = std::get<0>(C.pos);
  ce.y = std::get<1>(C.pos);
  ce.width = std::get<0>(C.size);
  ce.height = std::get<1>(C.size);
  ce.border_width = BDR_WIDTH;
  ce.above = None;
  ce.override_redirect = False;
  ::XSendEvent(dpy, C.w, False, StructureNotifyMask, reinterpret_cast<::XEvent *>(&ce));
}

::Atom dobwm::Manage::atomprop(const ::Window W, ::Atom prop) {
  int di;
  unsigned long dl;
  unsigned char *p { };
  ::Atom da, atom { None };

  if (::XGetWindowProperty(dpy, W, prop, 0L, sizeof atom, False, XA_ATOM,
      &da, &di, &dl, &dl, &p) == Success && p) {
    atom = *(::Atom *) p;
    ::XFree(p);
  }

  return atom;
}

dobwm::Ev::Ev(X &x) : x { x } {
  for (auto &f : F)
    f = [] { };

  F[MapNotify] = [&] { mapnotify(); };
  F[UnmapNotify] = [&] { unmapnotify(); };
  F[ClientMessage] = [&] { clientmessage(); };
  F[ConfigureNotify] = [&] { configurenotify(); };
  F[MapRequest] = [&] { maprequest(); };
  F[ConfigureRequest] = [&] { configurerequest(); };
  F[MotionNotify] = [&] { motionnotify(); };
  F[KeyPress] = [&] { keypress(); };
  F[ButtonPress] = [&] { buttonpress(); };
  F[EnterNotify] = [&] { enternotify(); };
}

void dobwm::Ev::mapnotify(void) {
  ::DBGMSG("Event MapNotify ", ev.type);
}

void dobwm::Ev::unmapnotify(void) {
  ::DBGMSG("Event UnmapNotify ", ev.type);
  ::XUnmapEvent &umev { ev.xunmap };
}

void dobwm::Ev::clientmessage(void) {
  ::DBGMSG("Event ClientMessage ", ev.type);
  ::XClientMessageEvent &cmev { ev.xclient };
  const auto C_ { std::ranges::find_if(C, 
    [w = cmev.window](const auto &C) { return C.w == w; }) };
  if (C_ == C.end()) return;
  // Handle messages
}

void dobwm::Ev::configurenotify(void) {
  ::DBGMSG("Event ConfigureNotify ", ev.type);
  ::XConfigureEvent &cev { ev.xconfigure };
}

void dobwm::Ev::maprequest(void) {
  ::DBGMSG("Event MapRequest ", ev.type);
  static ::XWindowAttributes wa;
  ::XMapRequestEvent &mrev { ev.xmaprequest };
  const auto W { mrev.window };
  if (!::XGetWindowAttributes(x.get().dpy, W, &wa) ||
    wa.override_redirect) return;
  if (std::ranges::find_if(C, [W](const auto &C) { return C.w == W; }) == C.end())
    m.client(W, wa);
}

void dobwm::Ev::configurerequest(void) {
  ::DBGMSG("Event ConfigureRequest ", ev.type);
  ::XConfigureRequestEvent &crev { ev.xconfigurerequest };
  ::XWindowChanges wc;
}

void dobwm::Ev::motionnotify(void) {
  ::DBGMSG("Event MotionNotify ", ev.type);
  ::XMotionEvent &mev { ev.xmotion };
  if (mev.window != x.get().root) return;
}

void dobwm::Ev::keypress(void) {
  ::DBGMSG("Event KeyPress ", ev.type);
  ::XKeyEvent &kev { ev.xkey };
  ::KeySym keysym { ::XKeycodeToKeysym(x.get().dpy, kev.keycode, 0) };
 /*
  for (const auto &C : CMDS)
    if (std::get<0>(C) == kev.state && std::get<1>(C) == keysym) {
      a.call(std::get<Calls>(std::get<2>(C)));
      return;
    }
  
  for (const auto &C : USERCMDS)
    if (std::get<0>(C) == kev.state && std::get<1>(C) == keysym) {
      a.shcmd(std::get<std::string_view>(std::get<2>(C)));
      return;
    }
  */
  const std::size_t KEY { kev.state | keysym };
  try {
    const auto &V { CMDS.at(KEY) };
    if (V.index() == 0) {
      a.call(std::get<0>(V));
      return;
    }
    
    a.shcmd(std::get<1>(V));
  } catch (...) { }
}

void dobwm::Ev::buttonpress(void) {
  ::DBGMSG("Event ButtonPress ", ev.type);
  ::XButtonPressedEvent &bpev { ev.xbutton };
}

void dobwm::Ev::enternotify(void) {
  ::DBGMSG("Event EnterNotify ", ev.type);
  ::XCrossingEvent &cev { ev.xcrossing };
  if ((cev.mode != NotifyNormal || cev.detail == NotifyInferior) && 
      cev.window != x.get().root) return;
  const auto C_ { std::ranges::find_if(C, 
    [w = cev.window](const auto &C) { return C.w == w; }) };
  // focus C_
}

volatile std::sig_atomic_t sig_status;

auto sig_handler(int sig) {
  sig_status = sig;
  //::XEvent ev { 0 };
  //::XSendEvent(x.dpy, x.root, 0, 0, &ev);
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
    dobwm::X x;
    if (!x.dpy)
      throw std::runtime_error("Unable to open display");

    const auto SCR { DefaultScreen(x.dpy) };
    x.root = RootWindow(x.dpy, SCR);
    ::XSetErrorHandler(XError);
  /*
    const auto SUPP { ::XInternAtom(x.dpy, ")NET_SUPPORTED", false) };
    ::XChangeProperty(x.dpy, x.root, SUPP, XA_ATOM, 32, PropModeReplace,
          reinterpret_cast<unsigned char *>(atom.NET.data()),
            static_cast<std::size_t>(Net::Z));
*/
    const auto ROOTMASK {
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
    for (const auto &C : dobwm::CMDS) {
      const auto KC { ::XKeysymToKeycode(x.dpy, std::get<1>(C)) };
      ::XGrabKey(x.dpy, KC, std::get<0>(C) & x.modmask, x.root, true, GrabModeAsync, GrabModeAsync);
    }

    for (const auto &C : dobwm::USERCMDS) {
      const auto KC { ::XKeysymToKeycode(x.dpy, std::get<1>(C)) };
      ::XGrabKey(x.dpy, KC, std::get<0>(C) & x.modmask, x.root, true, GrabModeAsync, GrabModeAsync);
    }
    */
    ::XGrabKey(x.dpy, AnyKey, dobwm::MODKEY & x.modmask, x.root, true, GrabModeAsync, GrabModeAsync);

    ::XSync(x.dpy, false);
    /*
    for (const auto &D : x.MONS()) {
      std::vector<Tag> T { Nt };
      Mon m { T, D };
      M.emplace_back(std::move(m));
    }
    
    for (const auto W : x.query_tree()) {
      map_request(W);
      ::DBGMSG("Init w/ client", W);
    }
    
    for (const auto &CMDS_ : { CMDS, CMDS_ASYNC })
      for (const auto &CMD : CMDS_) {
        const Kb &KB { std::get<0>(CMD) };
        x.grab_key(std::get<0>(KB), static_cast<unsigned long>(std::get<1>(KB)));
      }
    */
    dobwm::Panel p { x.dpy, x.root, SCR };
    p.draw("...");
    std::signal(SIGINT, sig_handler);
    dobwm::Ev ev { x };
    ::DBGMSG("WM initialized");
    ::MSG("Welcome msg", dobwm::Urg::NORMAL, 1000);
    while (!sig_status)
      if (ev.next() == 0) {
        ev.call();
        p.draw("...");
      }
    
    ::XCloseDisplay(x.dpy);
    ::DBGMSG("\nWM exit");
  } catch (const std::exception &E) {
      ::DBGMSG("Ex.", E.what());
      //std::println("Ex. { }", E.what());
      return -1;
  }
  
  return 0;
}
