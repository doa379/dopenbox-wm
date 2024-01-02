//#include <print>
#include <algorithm>
#include <csignal>
#include <unistd.h>
#include <functional>
#include <X11/XKBlib.h>
#include <x.h>
#include <msg.h>
#include <../config.h>

/*
namespace sys {
  template<typename TK, typename TF>
  class Ordered_map {
  public:
    using iterator = typename decltype(O)::iterator;
    using const_iterator = typename decltype(O)::const_iterator;
    constexpr auto size() const { return M.size(); }
    constexpr auto& operator[](const TK& K) { 
      if (!M.contains(K)) O.push_back(K); return M[K]; }
    constexpr std::pair<TK, TF>& operator[](const TK& K, const TF& F) const {
      return { K, F }; }
    constexpr auto& at(const TK& K) const { return M.at(K); }
    constexpr auto& at(const_iterator K) const { return at(*K); }
    constexpr auto find(const TK& K) const { return std::ranges::find(O, K); }
    constexpr auto begin() { return M.begin(); }
    constexpr auto end() { return M.end(); }
    constexpr auto cbegin() const { return O.begin(); }
    constexpr auto cend() const { return O.end(); }
    constexpr auto cprev(const_iterator K) const { 
      const auto O { std::prev(K) };
      return O < this->O.cbegin() ? this->O.cend() - 1 : O; }
    constexpr auto cnext(const_iterator K) const {
      const auto O { std::next(K) };
      return O == this->O.cend() ? this->O.cbegin() : O; }
    constexpr auto erase(const TK& K) { M.erase(K); O.erase(find(K)); }
    constexpr auto erase(const_iterator K) { M.erase(*K); O.erase(K); }
  private:
    std::vector<TK> O;
    std::unordered_map<TK, TF> M;
  };
}
*/

namespace dobwm {
  struct Client {
    ::Window w;
    ::GC gc;
    std::pair<std::size_t, size_t> pos, size;
    bool sel { };
  };

  class Panel {
  public:
    Panel(const Root&) noexcept;
    ~Panel() noexcept;
    ::GC new_gc() noexcept;
    void del_gc(const ::GC GC) noexcept;
    void draw_wks() noexcept;
    void draw_title(std::string_view) noexcept;
    void draw_title(const ::GC, std::string_view) noexcept;
  private:
    const Root& root;
    ::Drawable drawable;
    ::GC root_gc;
  };

  class Action {
  public:
    explicit Action(const Root&) noexcept;
    ~Action() noexcept { };
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
  private:
    const Root& root;
    Panel p { root };
    const Msg msg;
    std::unordered_map<Calls, std::function<void()>> F;
    //sys::Ordered_map<::Window, std::vector<::Window>::const_iterator> T;
    std::vector<Client> T;
    decltype(T)::const_iterator prev, curr;
  };
}

namespace {
  static constexpr std::string_view WMNAME { "Dopenbox WM" };
  static constexpr std::string_view VER { "-0.0" };
  volatile std::sig_atomic_t sig_status;
  static void sig_handler(int sig) { sig_status = sig; }
}

dobwm::Panel::Panel(const Root& root) noexcept : root { root },
  drawable { ::XCreatePixmap(root.dpy, root.w, 
    std::get<0>(root.size), std::get<1>(root.size), 
      DefaultDepth(root.dpy, root.scrn)) },
  root_gc { new_gc() } {
  draw_title(WMNAME);
}

dobwm::Panel::~Panel() noexcept {
  ::XFreeGC(root.dpy, root_gc);
  ::XFreePixmap(root.dpy, drawable); 
}

::GC dobwm::Panel::new_gc() noexcept {
  ::GC gc { ::XCreateGC(root.dpy, root.w, 0, nullptr) };
  ::XSetLineAttributes(root.dpy, gc, 1, LineSolid, CapButt, JoinMiter);
  return gc;
}

void dobwm::Panel::del_gc(const ::GC GC) noexcept {
  ::XFreeGC(root.dpy, GC);
}

void dobwm::Panel::draw_wks() noexcept {

}

void dobwm::Panel::draw_title(std::string_view S) noexcept {
  ::XSetForeground(root.dpy, root_gc, static_cast<std::size_t>(WKSBG));
  ::XFillRectangle(root.dpy, root.w, root_gc, 0, 0, 48, BARH);
  ::XSetForeground(root.dpy, root_gc, static_cast<std::size_t>(WKSFG));
  ::XDrawString(root.dpy, root.w, root_gc, 0, BARH - 2, WMNAME.data(), 
    WMNAME.length());
}

void dobwm::Panel::draw_title(const ::GC GC, std::string_view S) noexcept {
  ::XSetForeground(root.dpy, GC, static_cast<std::size_t>(TITLEBG));
  ::XFillRectangle(root.dpy, root.w, GC, 50, 0, std::get<0>(root.size), BARH);
  ::XSetForeground(root.dpy, GC, static_cast<std::size_t>(TITLEFG));
  ::XDrawString(root.dpy, root.w, GC, 50, BARH - 2, S.data(), S.size());
}

dobwm::Action::Action(const Root& root) noexcept : root { root } {
  F[Calls::QUIT] = [] { std::raise(SIGINT); };
  F[Calls::UNMAPALL] = [] { };
  F[Calls::REMAPALL] = [] { };
  F[Calls::KILL] = [&] { if (curr < T.cend()) kill(curr->w); };
  F[Calls::SWFOCUS] = [&] { focus(prev->w); };
  F[Calls::PREVCLI] = [&] { 
    const auto O { std::prev(curr) };
    focus(O < T.cbegin() ? T.back().w : O->w); };
  F[Calls::NEXTCLI] = [&] { 
    const auto O { std::next(curr) };
    focus(O == T.cend() ? T.front().w : O->w); };
  F[Calls::SELTOGGLE] = [] { };
  F[Calls::SELCLEAR] = [] { };
  F[Calls::MOVEUP] = [] { dobwm::DBG("Move Up"); };
  F[Calls::MOVEDOWN] = [] { };
  F[Calls::MOVELEFT] = [] { };
  F[Calls::MOVERIGHT] = [] { };
  F[Calls::RESIZEVINC] = [] { dobwm::DBG("Resize V+"); };
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
  msg.send(WMNAME, "Welcome msg", dobwm::Urg::NORMAL, 1000);
}

void dobwm::Action::unmapnotify(const ::Window W) noexcept {
  dobwm::DBG("Event UnmapNotify");
  if (const auto O { 
      std::ranges::find_if(T, [W](const auto& T) { return T.w == W; }) }; 
        O < T.end()) {
    curr = O - 1;
    p.del_gc(O->gc);
    T.erase(O);
    set_act(curr->w);
    ::XDeleteProperty(root.dpy, root.w, root.atom.CLIENT_LIST);
    for (const auto& C : T)
      ::XChangeProperty(root.dpy, root.w, root.atom.CLIENT_LIST, XA_WINDOW, 
        32, PropModeAppend, reinterpret_cast<const unsigned char*>(&C.w), 1);
  }
}

void dobwm::Action::clientmessage(const ::Window W) noexcept {
  dobwm::DBG("Event ClientMessage ");
  
  // Handle messages

  dobwm::DBG("ClientMessage on Window ", W);
}

void dobwm::Action::maprequest(const ::Window W) noexcept {
  dobwm::DBG("Event MapRequest");
  static ::XWindowAttributes wa;
  dobwm::DBG("Window ", W);
  if (!::XGetWindowAttributes(root.dpy, W, &wa) || wa.override_redirect)
    return;
  
  static constexpr auto WMASK {
    EnterWindowMask |
    FocusChangeMask |
    PropertyChangeMask |
    StructureNotifyMask };
  ::XSelectInput(root.dpy, W, WMASK);
  ::XChangeProperty(root.dpy, root.w, root.atom.CLIENT_LIST, XA_WINDOW, 32, 
      PropModeAppend, reinterpret_cast<const unsigned char*>(&W), 1);
  if (T.size()) {
    const auto POS { arrange(wa.width, wa.height) };
    wa.x = std::get<0>(POS);
    wa.y = std::get<1>(POS);
  }

  wa.y = wa.y < BARH ? BARH : wa.y;
  ::XMoveWindow(root.dpy, W, wa.x, wa.y);
  ::XMapWindow(root.dpy, W);
  
  prev = curr;
  if (T.size())
    set_inact(curr->w);
  T.emplace_back(Client { 
    W, p.new_gc(), { wa.x, wa.y }, { wa.width, wa.height } });
  curr = T.cend() - 1;
  set_act(curr->w);
  dobwm::DBG("End MapRequest");
}

void dobwm::Action::motionnotify(const ::Window W) noexcept {
  //dobwm::DBG("Event MotionNotify Window ", W);
}

void dobwm::Action::keypress(const auto STATE, const auto CODE) noexcept {
  dobwm::DBG("Event KeyPress ");
  const auto KMOD { STATE & root.modmask };
  const auto KSYM { ::XkbKeycodeToKeysym(root.dpy, CODE, 0, 0) };
  const auto& V { KEYS.at(KMOD).at(KSYM) };
  if (V.index() == 0) call(std::get<0>(V));
  else shcmd(std::get<1>(V));
}

void dobwm::Action::buttonpress(const ::Window W, const auto STATE,
const auto BTN) noexcept {
  dobwm::DBG("Event ButtonPress Window ", W);
  ::XUngrabPointer(root.dpy, CurrentTime);
}

void dobwm::Action::propertynotify(const ::Window W) noexcept {
  dobwm::DBG("Event PropertyNotify ");
  if (W == root.w) {
    p.draw_title(WMNAME);
  } else if (::XTextProperty tp;
      ::XGetTextProperty(root.dpy, W, &tp, root.atom.NAME) && tp.nitems) {
        const auto C {
          std::ranges::find_if(T, [W](const auto& T) { return T.w == W; }) };
        p.draw_title(C->gc, reinterpret_cast<const char*>(tp.value));
  }

  dobwm::DBG("End PropertyNotify");
}

void dobwm::Action::shcmd(std::string_view CMD) const noexcept { 
  if (fork() == 0) {
    ::close(ConnectionNumber(root.dpy));
    ::setsid();
    ::system(CMD.data());
  }
}

void dobwm::Action::set_act(const ::Window W) const noexcept {
  ::XSetWindowBorder(root.dpy, W, static_cast<std::size_t>(ACTBDR));
  ::XSetWindowBorderWidth(root.dpy, W, BDRW);
  ::XSetInputFocus(root.dpy, W, RevertToPointerRoot, CurrentTime);
  ::XChangeProperty(root.dpy, root.w, root.atom.WM_STATE, XA_WINDOW, 32, 
    PropModeReplace, reinterpret_cast<const unsigned char*>(&W), 1);
  ::XChangeProperty(root.dpy, W, root.atom.ACTIVE_WINDOW, XA_WINDOW, 32, 
    PropModeReplace, reinterpret_cast<const unsigned char*>(&W), 1);
  ::XRaiseWindow(root.dpy, W);
////////////////////////////////////////////////////////////////////////////  
  static constexpr auto BUTTONMASK { ButtonPressMask | ButtonReleaseMask };
  for (const auto& MOD : BTNS)
    for (const auto& BTN : std::get<1>(MOD)) {
      ::XUngrabButton(root.dpy, std::get<0>(BTN), 
        std::get<0>(MOD) & root.modmask, W);
      ::XGrabButton(root.dpy, std::get<0>(BTN), 
        std::get<0>(MOD) & root.modmask, W, false, BUTTONMASK, GrabModeSync, 
          GrabModeSync, None, None);
    }
}

void dobwm::Action::set_inact(const ::Window W) const noexcept {
  for (const auto& MOD : BTNS)
    for (const auto& BTN : std::get<1>(MOD))
      ::XUngrabButton(root.dpy, std::get<0>(BTN), 
        std::get<0>(MOD) & root.modmask, root.w);
  
  ::XDeleteProperty(root.dpy, W, root.atom.ACTIVE_WINDOW);
  ::XSetWindowBorder(root.dpy, W, static_cast<std::size_t>(INACTBDR));
}

void dobwm::Action::kill(const ::Window W) const noexcept {
  ::XEvent xev { ClientMessage };
  xev.xclient.window = W;
  xev.xclient.message_type = root.atom.PROTO;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = root.atom.DELETE_WINDOW;
  xev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(root.dpy, W, false, NoEventMask, &xev);
}

void dobwm::Action::focus(const ::Window W) noexcept {
  if (W == curr->w)
    return;
  set_inact(curr->w);
  prev = curr;
  curr = std::ranges::find_if(T, [W](const auto& T) { return T.w == W; });
  set_act(curr->w);
}

std::pair<std::size_t, std::size_t> 
dobwm::Action::arrange(const std::size_t W, const std::size_t H) noexcept {
  ::XWindowAttributes wa;
  ::XGetWindowAttributes(root.dpy, T.back().w, &wa);
  if (wa.x + BARH + W < std::get<0>(root.size) &&
      wa.y + BARH + H < std::get<1>(root.size))
    return { wa.x + BARH, wa.y + BARH };
  
  return { };
}

void dobwm::Action::workspace(const unsigned char N) const noexcept {
  /*
  ::XChangeProperty(root.dpy, root.w, root.atom.CURRENT_DESKTOP,
    XA_CARDINAL, 32, PropModeReplace,
      &N, 1);
  ::XChangeProperty(root.dpy, root.w, root.atom.WM_DESKTOP,
    XA_CARDINAL, 32, PropModeReplace,
      &N, 1);

  ::XChangeProperty(root.dpy, root.w, root.atom.SHOWING_DESKTOP,
    XA_CARDINAL, 32, PropModeReplace,
      &N, 1);

  return;
  */
  ::XEvent xev { ClientMessage };
  xev.xclient.serial = 0;
  xev.xclient.send_event = True;
  xev.xclient.window = root.w;
  xev.xclient.message_type = root.atom.CURRENT_DESKTOP;
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = N;
  xev.xclient.data.l[1] = 0;
  xev.xclient.data.l[2] = 0;
  xev.xclient.data.l[3] = 0;
  xev.xclient.data.l[4] = 0;
  long mask { SubstructureRedirectMask | SubstructureNotifyMask };
  ::XSendEvent(root.dpy, root.w, false, mask, &xev);
  ::XSync(root.dpy, false);

  {
  ::XEvent xev { ClientMessage };
  xev.xclient.serial = 0;
  xev.xclient.send_event = True;
  xev.xclient.window = root.w;
  xev.xclient.message_type = root.atom.SHOWING_DESKTOP;
  xev.xclient.format = 32;
  unsigned long state { };
  xev.xclient.data.l[0] = state;
  xev.xclient.data.l[1] = 0;
  xev.xclient.data.l[2] = 0;
  xev.xclient.data.l[3] = 0;
  xev.xclient.data.l[4] = 0;
  long mask { SubstructureRedirectMask | SubstructureNotifyMask };
  ::XSendEvent(root.dpy, root.w, false, mask, &xev);
  }
}

int main(const int ARGC, const char* ARGV[]) {
  try {
    //std::println("...");
    dobwm::DBG(WMNAME, "ver. ", VER);
    static dobwm::Root root;
    dobwm::DBG("Root Window ", root.w);
    for (const auto& MOD : dobwm::KEYS)
      for (const auto& KEY : std::get<1>(MOD))
        root.grab_key(std::get<0>(MOD), std::get<0>(KEY));

    static dobwm::Action a { root };
    // Events
    static ::XEvent xev;
    static std::array<std::function<void()>, LASTEvent> F; // Literal defn.
    for (auto& f : F)
      f = [] { dobwm::DBG("Unqualified event ", xev.type); };
    
    F[MapNotify] = [] { dobwm::DBG("Event MapNotify"); };
    F[UnmapNotify] = [] { a.unmapnotify(xev.xunmap.window); };
    F[ClientMessage] = [] { a.clientmessage(xev.xclient.window); };
    F[ConfigureNotify] = [] { 
      (void) xev.xconfigure;
      dobwm::DBG("Event ConfigureNotify");
    };
    F[MapRequest] = [] { a.maprequest(xev.xmaprequest.window); };
    F[ConfigureRequest] = [] { 
      const auto& CONF { xev.xconfigurerequest };
      ::XWindowChanges wc {
          CONF.x, CONF.y, CONF.width, CONF.height,
          CONF.border_width, CONF.above, CONF.detail };
      ::XConfigureWindow(root.dpy, CONF.window, CONF.value_mask, &wc);
    };
    F[MotionNotify] = [] { a.motionnotify(xev.xmotion.window); };
    F[KeyPress] = [] { a.keypress(xev.xkey.state, xev.xkey.keycode); };
    F[ButtonPress] = [] { 
      a.buttonpress(xev.xbutton.window, xev.xbutton.state, xev.xbutton.button);
    };
    F[EnterNotify] = [] { 
      if (xev.xcrossing.window != root.w && dobwm::SLOPPY_FOCUS)
        a.focus(xev.xcrossing.window);
    };
    F[PropertyNotify] = [] { a.propertynotify(xev.xproperty.window); };

    // Workspaces
    if (dobwm::INITWKS >= 0 && dobwm::INITWKS <= dobwm::NWKS) {
      ::XChangeProperty(root.dpy, root.w, root.atom.NUMBER_OF_DESKTOPS,
        XA_CARDINAL, 32, PropModeReplace,
          reinterpret_cast<const unsigned char*>(&dobwm::NWKS), 1);
      ::XChangeProperty(root.dpy, root.w, root.atom.CURRENT_DESKTOP,
        XA_CARDINAL, 32, PropModeReplace,
          reinterpret_cast<const unsigned char*>(&dobwm::INITWKS), 1);
    }
 
    { // Clients
      unsigned n;
      ::Window rootw, parw;
      ::Window* w;
      if (::XQueryTree(root.dpy, root.w, &rootw, &parw, &w, &n)) {
        for (unsigned i { }; i < n; i++) {
          ::XWindowAttributes wa;
          if (::XGetWindowAttributes(root.dpy, w[i], &wa) && 
              wa.map_state == IsViewable) {
            ::XEvent xev { MapRequest };
            xev.xmaprequest.send_event = true,
            xev.xmaprequest.parent = root.w;
            xev.xmaprequest.window = w[i];
            ::XSendEvent(root.dpy, root.w, true, root.ROOTMASK, &xev);
          }
        }

        if (w) ::XFree(w);
      }

      ::XSync(root.dpy, false);
    }

    std::signal(SIGINT, sig_handler);
    dobwm::DBG("WM initialized");
    // Ev loop
    while (sig_status == 0 && ::XNextEvent(root.dpy, &xev) == 0) {
      F[xev.type]();
      ::XSync(root.dpy, false);
    }
    // Deinit
    for (const auto& MOD : dobwm::KEYS)
      for (const auto& KEY : std::get<1>(MOD))
        root.ungrab_key(std::get<0>(MOD), std::get<0>(KEY));

    dobwm::DBG("\nWM exit");
  } catch (const std::exception& E) {
      dobwm::DBG("Ex.", E.what());
      //std::println("Ex. { }", E.what());
      return -1;
  }
  
  return 0;
}
