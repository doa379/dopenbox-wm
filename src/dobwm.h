#pragma once

#include <vector>
#include <string>
#include <X11/Xutil.h>

namespace dobwm {
  enum class Mode { DEF, TRA, MON };

  struct Client {
    ::Window u, v;  // (parent, actual)
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
  };

  class X : private Event {
    static const auto ROOTMASK { SubstructureRedirectMask | SubstructureNotifyMask };
    static const auto BUTTONMASK { ButtonPressMask | ButtonReleaseMask | ButtonMotionMask };
    static const auto NOTIFMASK { PropertyChangeMask };
    static bool error;
    ::Display *dpy { };
    ::XEvent ev { };
    ::Window root { };
  public:
    X(void);
    ~X(void);
    static int init_XError(::Display *, ::XErrorEvent *);
    static int XError(::Display *, ::XErrorEvent *);
    int next_event(void) { return ::XNextEvent(dpy, &ev); }
    int &event(void) { return ev.type; }
    void create_notify(void) const;
    void destroy_notify(void) const;
    void reparent_notify(void) const;
    void map_notify(void) const;
    ::Window unmap_notify(void) const;
    void configure_notify(void) const;
    ::Window map_request(void) const;
    void window(::Window, const unsigned, const unsigned, const unsigned);
    void unmap_request(::Window, ::Window) const;
    ::XConfigureRequestEvent &configure_request(void);
    void configure_window(::XConfigureRequestEvent &, ::Window) const;
    void query_tree(const unsigned, const unsigned, const unsigned);
    void button_press(void);
    void button_release(void);
    void grab_button(void);
    void grab_button(::Window, const std::vector<int> &);
    void motion_notify(void);
    void key_press(void);
    void key_release(void);
    void grab_key(void);
  };

  class Xops {
  public:
  /*
    WAttr map_request(::Window &) const;
    ::Window manage(::Window, WAttr &, const unsigned, const unsigned, const unsigned) const;
    void unmanage(::Window) const;
    ::XConfigureRequestEvent &configure_request(void);
    void configure_window(::XConfigureRequestEvent &, ::Window) const;
    */
  };
}
