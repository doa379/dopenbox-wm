#pragma once

#include <vector>
#include <string>
#include <X11/Xutil.h>

namespace dobwm {
  enum class Mode {
    TRANS,
    FLOAT,
    TILE,
    CASC,
    MONO
  };

  struct Client {
    ::Window win;
    std::string name;
    unsigned x { }, y { }, w { }, h { };
    Mode mode;
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

  class X {
    static const auto ROOTMASK { SubstructureRedirectMask | SubstructureNotifyMask };
    //static const auto ROOTMASK {SubstructureRedirectMask | ButtonPressMask | SubstructureNotifyMask | PropertyChangeMask };
    static bool error;
    ::Display *dpy { nullptr };
    ::XEvent ev { };
    ::Window root { };
  public:
    X(void);
    ~X(void);
    static int XError(::Display *, ::XErrorEvent *);
    int event(void) { return ::XNextEvent(dpy, &ev); }
    int type(void) { return ev.type; }
    void create_notify(void);
    void destroy_notify(void);
    void reparent_notify(void);
    void map_notify(void);
    void unmap_notify(void);
    void configure_notify(void);
    void map_request(dobwm::Client &);
    void configure_request(void);
    void button_press(void);
    void button_release(void);
    void motion_notify(void);
    void key_press(void);
    void key_release(void);
  };
}
