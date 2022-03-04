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
    ::Display *dpy { nullptr };
    ::XEvent ev { };
    ::Window root;
    std::list<Xlib::Mon> M;
    static bool error;
  public:
    Box(const unsigned char, const unsigned char);
    ~Box(void);
    static int XError(::Display *, ::XErrorEvent *);
    int event(void) { return ::XNextEvent(dpy, &ev); }
    int type(void) { return ev.type; }
  };
}
