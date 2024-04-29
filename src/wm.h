#pragma once

#include <X11/Xlib.h>
#include <vector>
#include <tuple>
#include <Xlib.h>

namespace some {
  struct Client {
    ::Window w;
    ::Window parw;
    ::Window shadow;
    ::GC gc;
    std::pair<unsigned, unsigned> pos;
    std::pair<unsigned, unsigned> size;
    unsigned mode;
    int sel;
    int ft;
    int pad[2];
  };

  struct Wk {
    std::vector<Client> C;
    std::vector<Client>::iterator client[2]; // Prev, Curr
    unsigned n;
  };

  struct Mon {
    std::pair<unsigned, unsigned> pos;
    std::pair<unsigned, unsigned> size;
  };
  
  class Wm {
    public:
    Wm();
    ~Wm();
    protected:
    Display dpy;
    Xlib::Xlib xlib;
    Xlib::Input input;
    Xlib::Draw draw;
    static bool xerror;
    static int handler(::Display*, ::XErrorEvent* xev) {
      xerror = xev->error_code == BadAccess;
      return 0;
    }

    std::array<std::function<void()>, 128> CALL;
    std::vector<Wk> WK;
    std::vector<Wk>::iterator wk[2]; // Prev, Curr
    std::vector<Mon> MON;
    std::vector<Mon>::iterator mon;
    ::Window rootw;
    ::Window panel;
    ::GC wkgc;
    ::GC statusgc;
  };
  
  template<typename T>
  struct Recv : private Wm {
    void mapnotify(const T&);
    void unmapnotify(const T&);
    void clientmessage(const T&);
    void configurenotify(const T&);
    void maprequest(const T&);
    void configurerequest(const T&);
    void motionnotify(const T&);
    void keypress(const T&);
    void btnpress(const T&);
    void enternotify(const T&);
    void propertynotify(const T&);
    void expose(const T&);
  };
}
