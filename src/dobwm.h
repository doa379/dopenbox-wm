#pragma once

#include <vector>
#include <string>
#include <X11/Xutil.h>

namespace dobwm {
  enum class Mode { DEF, TRA, MON };

  struct Client {
    ::Window win;
    std::string name;
    unsigned x { }, y { }, w { }, h { };
    Mode mode { dobwm::Mode::DEF };
    bool sel { };
  };

  struct Tag {
    std::vector<Client> C;
  };

  struct Mon {
    std::vector<Tag> T;
    unsigned w { }, h { };
  };

  class Box {
    std::vector<Mon> M;
  public:
    Box(void);
    void map_request(void);
    void unmap_request(void);
    void configure_request(void);
    void init_windows(void);
    void grab_button(void);
    void grab_key(void);
  };
/*
  enum class Event {
    CreateNotify,
    DestroyNotify,
    ReparentNotify,
    MapNotify,
    UnmapNotify,
    ConfigureNotify,
    MapRequest,
    ConfigureRequest,
    ButtonPress,
    ButtonRelease,
    MotionNotify,
    KeyPress,
    KeyRelease
  };
*/
  class Atom {

  };

  class Event {
  protected:
    ::XEvent ev { };
  public:
    void create_notify(void) const { (void) ev.xcreatewindow; }
    void destroy_notify(void) const { (void) ev.xdestroywindow; }
    void reparent_notify(void) const { (void) ev.xreparent; }
    void map_notify(void) const { (void) ev.xmap; }
    ::Window unmap_notify(void) const { return ev.xunmap.window; }
    void configure_notify(void) const { (void) ev.xconfigure; }
    ::Window map_request(void) const { return ev.xmaprequest.window; }
    ::XConfigureRequestEvent &configure_request(void) { return ev.xconfigurerequest; }
    void button_press(void);
    void button_release(void);
    void motion_notify(void);
    void key_press(void);
    void key_release(void);
  };

  class X : public Event {
    static const auto ROOTMASK { SubstructureRedirectMask | SubstructureNotifyMask };
    static const auto BUTTONMASK { ButtonPressMask | ButtonReleaseMask | ButtonMotionMask };
    static const auto NOTIFMASK { PropertyChangeMask };
    static bool error;
    ::Display *dpy { };
    ::Window root { };
  public:
    X(void);
    ~X(void);
    static int init_XError(::Display *, ::XErrorEvent *);
    static int XError(::Display *, ::XErrorEvent *);
    int next_event(void) { return ::XNextEvent(dpy, &ev); }
    int &event(void) { return ev.type; }
    void window(::Window, const unsigned, const unsigned);
    void unmap_request(::Window) const;
    void configure_window(::XConfigureRequestEvent &, ::Window) const;
    void query_tree(const unsigned, const unsigned);
    void grab_button(void);
    void grab_button(::Window, const std::vector<int> &);
    void grab_key(void);
  };
}
