#pragma once

#include <X11/Xlib.h>
#include <vector>
#include <tuple>
#include <Xlib.h>

namespace some {
  class Root {
    public:
    Root(::Display*);
    ~Root();
    Xlib::Xlib xlib;
    Xlib::Input input;
    ::Window w;
    private:
    static bool xerror;
    static int handler(::Display*, ::XErrorEvent* xev) {
      xerror = xev->error_code == BadAccess;
      return 0;
    }
  };
  
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
    Wm(::Display*);
    ~Wm();
    void mapnotify();
    void unmapnotify();
    void clientmessage();
    void configurenotify();
    void maprequest();
    void configurerequest();
    void motionnotify();
    void keypress();
    void btnpress();
    void enternotify();
    void propertynotify();
    void expose();
    private:
    ::Display* dpy;
    Root root;
    std::array<std::function<void()>, 128> CALL;
    std::vector<Wk> WK;
    std::vector<Wk>::iterator wk[2]; // Prev, Curr
    std::vector<Mon> MON;
    std::vector<Mon>::iterator mon;
    ::Window panel;
    ::GC wkgc;
    ::GC statusgc;
  };
}
