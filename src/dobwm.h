#pragma once

#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <X11/Xutil.h>
#include <palette.h>

namespace dobwm {
  static constexpr std::string_view VER { "-0.0" };
  enum class Mode { DEF, TRA, MON };

  struct Client {
    ::Window win;
    std::string name;
    int x { }, y { }, w { }, h { };
    Mode mode { Mode::DEF };
    bool sel { };
  };

  struct Tag {
    std::vector<Client> C;
  };

  struct Mon {
    std::vector<Tag> T;
    int w { }, h { };
  };

  class Box {
    std::vector<Mon> M;
  public:
    Box(void);
    ~Box(void);
    void key(void);
    void init_clients(void);
    void map_request(void);
    void configure_request(void);
    void unmap_request(void);
    void unmap_all(void);
    void swfocus(void) const;
  };

  enum class XEvent {
    Create = CreateNotify,
    Destroy = DestroyNotify,
    Reparent = ReparentNotify,
    Map = MapNotify,
    Unmap = UnmapNotify,
    CliMsg = ClientMessage,
    Config = ConfigureNotify,
    MapReq = MapRequest,
    ConfigReq = ConfigureRequest,
    Motion = MotionNotify,
    Button = ButtonPress,
    Key = KeyPress
  };
  
  class Event {
  protected:
    ::XEvent ev { };
  public:
    dobwm::XEvent event(void) const {
      return static_cast<dobwm::XEvent>(ev.type); }
    void create_notify(void) const { (void) ev.xcreatewindow; }
    void destroy_notify(void) const { (void) ev.xdestroywindow; }
    void reparent_notify(void) const { (void) ev.xreparent; }
    void map_notify(void) const { (void) ev.xmap; }
    ::Window unmap_notify(void) const { return ev.xunmap.window; }
    void configure_notify(void) const { (void) ev.xconfigure; }
    ::Window map_request(void) const { return ev.xmaprequest.window; }
    ::XConfigureRequestEvent &configure_request(void) { 
      return ev.xconfigurerequest; }
    void motion_notify(void) const { 
      const ::XMotionEvent &ev { this->ev.xmotion }; };
    void button(void) const {
      const ::XButtonEvent &ev { this->ev.xbutton };}
    int key_state(void) const { return ev.xkey.state; }
    ::KeyCode key_code(void) const { return ev.xkey.keycode; }
  };

  enum class Wm { PROTO, DELWIN, CNT };
  enum class Net { SUPP, STATE, ACT, FSCRN, CNT };

  class X : public Event {
    static constexpr auto ROOTMASK {
      SubstructureRedirectMask | SubstructureNotifyMask };
    static constexpr auto BUTTONMASK {
      ButtonPressMask | ButtonReleaseMask | ButtonMotionMask };
    static constexpr auto NOTIFMASK { PropertyChangeMask };
    static bool error;
    int modmask { };
    ::Display *dpy { ::XOpenDisplay(nullptr) };
    ::Window root { };
    std::array<::Atom, static_cast<int>(dobwm::Wm::CNT)> WM;
    std::array<::Atom, static_cast<int>(dobwm::Net::CNT)> NET;
  public:
    X(void);
    ~X(void);
    static int XError(::Display *, ::XErrorEvent *);
    int next_event(void) { return ::XNextEvent(dpy, &ev); }
    void client(::Window, const int, const Palette);
    void unmap_request(const ::Window) const;
    void configure_window(::XConfigureRequestEvent &) const;
    std::vector<::Window> query_tree(void);
    void grab_button(const ::Window, const int, const int);
    void grab_buttons(void);
    void grab_key(const int, const int) const;
    ::KeySym key_press(::KeyCode);
  };
}
