#pragma once

#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <functional>
#include <optional>
#include <utility>
#include <X11/Xutil.h>
#include <msg.h>

namespace dobwm {
  static constexpr auto VER { -0.0 };
  struct Dim {
    int x { }, y { }, w { }, h { };
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
    auto event(void) const -> XEvent {
      return static_cast<dobwm::XEvent>(ev.type); }
    //void create_notify(void) const { (void) ev.xcreatewindow; }
    //void destroy_notify(void) const { (void) ev.xdestroywindow; }
    //void reparent_notify(void) const { (void) ev.xreparent; }
    auto map_notify(void) const { (void) ev.xmap; }
    auto unmap_notify(void) const -> ::Window { return ev.xunmap.window; }
    auto configure_notify(void) const { (void) ev.xconfigure; }
    auto map_request(void) const -> ::Window { return ev.xmaprequest.window; }
    auto configure_request(void) -> ::XConfigureRequestEvent & { 
      return ev.xconfigurerequest; }
    auto motion_notify(void) const { 
      const ::XMotionEvent &ev { this->ev.xmotion }; };
    auto key_state(void) const -> int { return ev.xkey.state; }
    auto key_code(void) const -> ::KeyCode{ return ev.xkey.keycode; }
    //::Window client(void) const { return ev.xclient.window; }
    //::Atom msg_type(void) const { return ev.xclient.message_type; }
    auto button(void) const -> int { return ev.xbutton.button; }
    auto button_state(void) const -> int { return ev.xbutton.state; }
    auto button_window(void) const -> ::Window { return ev.xbutton.window; }
    auto crossing_window(void) const -> ::Window { return ev.xcrossing.window; }
  };

  class X : public Event {
    class Xinerama {
      std::vector<Dim> D;
    public:
      Xinerama(::Display *);
      ~Xinerama(void);
      auto M(void) -> std::vector<Dim> & { return D; }
    };

    class Bar {
    public:
      Bar(void) { }
    };

    enum class Wm : int { PROTO, DELWIN, CNT };
    enum class Net : int { SUPP, STATE, ACT, FSCRN, CNT };
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
    static auto XError(::Display *, ::XErrorEvent *) -> int;
    auto MONS(void) const -> std::vector<Dim>;
    auto next_event(void) -> bool { return ::XNextEvent(dpy, &ev) == 0; }
    auto client(const ::Window, const unsigned, const unsigned long) const -> bool;
    auto focus(::Window) const -> bool;
    auto isactive(const ::Window) const -> bool;
    auto map_window(const ::Window) const -> bool;
    auto unmap_window(const ::Window) const -> bool;
    auto configure_window(::XConfigureRequestEvent &) const -> bool;
    auto query_tree(void) -> std::vector<::Window>;
    auto client_trans(const ::Window) -> std::optional<::Window>;
    auto client_attrib(const ::Window) -> std::optional<::XWindowAttributes>;
    auto client_dim(const ::Window) -> std::optional<Dim>;
    auto trans_dim(const ::Window) -> std::optional<Dim>;
    auto client_hint(const ::Window) -> std::optional<std::pair<std::string, std::string>>;
    auto grab_key(const unsigned, const ::KeySym) const -> void;
    auto grab_button(const unsigned, const unsigned) -> void;
    auto grab_button(const ::Window, const unsigned, const unsigned) -> void;
    auto ungrab_button(const ::Window, const unsigned, const unsigned) -> void;
    auto key_press(::KeyCode) -> ::KeySym;
    auto kill_msg(const ::Window) const;
    auto kill_msg(void) const;
    auto kill_client(const ::Window) const -> bool;
    auto move(const ::Window, const int, const int) const -> bool;
  };
  
  class Box {
    struct Hnd {
      ::Window win;
      Dim d;
      bool mut, sel { };
      std::optional<std::pair<std::string, std::string>> res;
      auto operator ==(const Hnd &H) const -> bool { return H.win == win; }
      auto operator !=(const Hnd &H) const -> bool { return !(H == *this); }
    };

    struct Tag {
      std::vector<Hnd> H;
    };

    struct Mon {
      std::vector<Tag> T;
      Dim d;
    };

    using HndRef = std::optional<std::reference_wrapper<Hnd>>;
    /*
    struct Curr {
      std::reference_wrapper<Mon> m;
      std::reference_wrapper<Tag> t;
      Hndref hnd;
    };
    */

    X x;
    Msg msg;
    std::vector<Mon> M;
    HndRef curr;
    //Curr curr;
  public:
    //Box(void);
    //~Box(void);
    //auto MSG(std::string_view, const Urg, const unsigned);
    //auto print_hint(const std::pair<std::string, std::string>) const;
    auto focus(Hnd &);
    auto sw_focus(void);
    auto map_request(const ::Window) -> void;
    auto map_all(void) const;
    auto unmap_all(void) const;
    auto del_hnd(const Hnd &);
    auto key(void);
    auto configure_request(void);
    auto unmap_request(void);
    auto cli_msg(void) const;
    auto hnd(const ::Window) -> HndRef;
    auto enter_notify(void);
    auto button(void);
    //auto ev(void);
  };
}
