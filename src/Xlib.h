#pragma once

#include <dobwm.h>

namespace Xlib {
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

  class Box {
    static const auto ROOTMASK { SubstructureRedirectMask | SubstructureNotifyMask };
    //static const auto ROOTMASK {SubstructureRedirectMask | ButtonPressMask | SubstructureNotifyMask | PropertyChangeMask };
    static bool error;
    ::Display *dpy { nullptr };
    ::XEvent ev { };
    ::Window root { };
  public:
    Box(void);
    ~Box(void);
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
