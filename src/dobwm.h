#pragma once

#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <functional>
#include <optional>
#include <utility>
#include <X11/Xutil.h>
#include <palette.h>

namespace dobwm {
  static constexpr std::string_view VER { "-0.0" };
  enum class Mode { DEF, TRA, MON };

  struct Dim {
    int x { }, y { }, w { }, h { };
  };

  struct Wattr {
    Dim d;
    Mode mode { };
    ::Window tra;
  };

  class Box {
    struct Client {
      ::Window win;
      Wattr wa;
      std::string name;
      bool sel { };
      bool operator ==(const Client &C) const { return C.win == win; }
      bool operator !=(const Client &C) const { return !(C == *this); }
    };

    struct Tag {
      std::vector<Client> C;
    };

    struct Mon {
      std::vector<Tag> T;
      Dim d;
    };
    
    class Input {
    public:
    };

    class Arrange {
    public:
    };

    using HndRef = std::optional<std::reference_wrapper<Client>>;
    std::vector<Mon> M;
    HndRef curr;
  public:
    Box(const std::vector<Dim> &);
    ~Box(void);
    void key(void);
    void init(void);
    void configure_request(void);
    void map_request(const ::Window);
    void unmap_request(void);
    void map_all(void) const;
    void unmap_all(void) const;
    void cli_msg(void) const;
    void focus(Client &);
    void sw_focus(void);
    decltype(curr) client(const ::Window);
    void del_client(const Client &);
    void enter_notify(void);
    void button(void);
  };

  enum class XEvent {
    //Create = CreateNotify,
    //Destroy = DestroyNotify,
    //Reparent = ReparentNotify,
    Map = MapNotify,
    Unmap = UnmapNotify,
    CliMsg = ClientMessage,
    Config = ConfigureNotify,
    MapReq = MapRequest,
    ConfigReq = ConfigureRequest,
    Motion = MotionNotify,
    Key = KeyPress,
    Button = ButtonPress,
    Enter = EnterNotify
  };
  
  class Event {
  protected:
    ::XEvent ev { };
  public:
    dobwm::XEvent event(void) const {
      return static_cast<dobwm::XEvent>(ev.type); }
    //void create_notify(void) const { (void) ev.xcreatewindow; }
    //void destroy_notify(void) const { (void) ev.xdestroywindow; }
    //void reparent_notify(void) const { (void) ev.xreparent; }
    void map_notify(void) const { (void) ev.xmap; }
    ::Window unmap_notify(void) const { return ev.xunmap.window; }
    void configure_notify(void) const { (void) ev.xconfigure; }
    ::Window map_request(void) const { return ev.xmaprequest.window; }
    ::XConfigureRequestEvent &configure_request(void) { 
      return ev.xconfigurerequest; }
    void motion_notify(void) const { 
      const ::XMotionEvent &ev { this->ev.xmotion }; };
    int key_state(void) const { return ev.xkey.state; }
    ::KeyCode key_code(void) const { return ev.xkey.keycode; }
    //::Window client(void) const { return ev.xclient.window; }
    //::Atom msg_type(void) const { return ev.xclient.message_type; }
    int button(void) const { return ev.xbutton.button; }
    int button_state(void) const { return ev.xbutton.state; }
    ::Window button_window(void) const { return ev.xbutton.window; }
    ::Window crossing_window(void) const { return ev.xcrossing.window; }
  };

  enum class Wm : int { PROTO, DELWIN, CNT };
  enum class Net : int { SUPP, STATE, ACT, FSCRN, CNT };

  class X : public Event {
    class Xinerama {
      std::vector<Dim> D;
    public:
      Xinerama(::Display *);
      ~Xinerama(void);
      //std::size_t Nm(void) { return D.size(); }
      std::vector<Dim> M(void) const { return D; }
    };

    static constexpr auto ROOTMASK {
      SubstructureRedirectMask | SubstructureNotifyMask };
    static constexpr auto BUTTONMASK {
      ButtonPressMask | ButtonReleaseMask | ButtonMotionMask };
    static constexpr auto NOTIFMASK { PropertyChangeMask };
    static bool error;
    int modmask { };
    ::Display *dpy { ::XOpenDisplay(nullptr) };
    ::Window root { };
    std::array<::Atom, static_cast<int>(Wm::CNT)> WM;
    //std::array<::Atom, std::to_underlying(Wm::CNT)> WM;
    std::array<::Atom, static_cast<int>(Net::CNT)> NET;
    //std::array<::Atom, std::to_underlying(Net::CNT)> NET;
  public:
    X(void);
    ~X(void);
    static int XError(::Display *, ::XErrorEvent *);
    std::vector<Dim> MONS(void) const;
    int next_event(void) { return ::XNextEvent(dpy, &ev); }
    void client(::Window, const int, const Palette) const;
    void focus(::Window) const;
    bool isactive(const ::Window) const;
    void map_window(const ::Window) const;
    void unmap_window(const ::Window) const;
    void configure_window(::XConfigureRequestEvent &) const;
    std::vector<::Window> query_tree(void);
    std::optional<Wattr> client_attr(const ::Window);
    void grab_key(const int, const int) const;
    void grab_button(const int, const int);
    void grab_button(const ::Window, const int, const int);
    void ungrab_button(const ::Window, const int, const int);
    ::KeySym key_press(::KeyCode);
    void kill_msg(const ::Window) const;
    void kill_msg(void) const;
    bool kill_client(const ::Window) const;
    bool move(const ::Window, const int, const int) const;
  };
}
