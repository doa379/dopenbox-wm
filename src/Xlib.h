#pragma once

#include <list>
#include <string>
#include <X11/Xutil.h>

namespace Xlib {
  struct Client {
    ::Window win;
    std::string name;
    unsigned x { }, y { }, w { }, h { };
  };

  struct Tag {
    std::list<Client> C;
  };

  struct Mon {
    std::list<Tag> T;
    unsigned w { }, h { };
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

  class Box {
    static const auto ROOTMASK { SubstructureRedirectMask | SubstructureNotifyMask };
    //static const auto ROOTMASK {SubstructureRedirectMask | ButtonPressMask | SubstructureNotifyMask | PropertyChangeMask };
    static bool error;
    ::Display *dpy { nullptr };
    ::XEvent ev { };
    ::Window root { };
    std::list<Xlib::Mon> M;
  public:
    Box(const unsigned char, const unsigned char);
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
    void map_request(void);
    void configure_request(void);
    void button_press(void);
    void button_release(void);
    void motion_notify(void);
    void key_press(void);
    void key_release(void);
  };
}
