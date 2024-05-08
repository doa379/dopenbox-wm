#pragma once

#include <vector>
#include <tuple>
#include <Xlib.h>

namespace some {
  struct Client {
    Xlib::Win w;
    //::GC gc;
    std::pair<int, int> pos;
    std::pair<int, int> size;
    unsigned mode;
    int sel;
    int ft;
  };

  struct Wk {
    std::vector<Client> C;
    // Prev, Curr
    std::vector<Client>::const_iterator client[2] { C.cbegin(), C.cbegin() }; 
    int n;
  };

  struct Mon {
    std::pair<int, int> pos;
    std::pair<int, int> size;
  };
  
  class Wm {
    public:
    Wm();
    ~Wm();
    void focus();
    void spawn();
    void prev_client();
    void next_client();
    void kill_client();
    protected:
    Display dpy;
    Xlib::Xlib xlib;
    Xlib::Input input;
    Xlib::Draw draw;
    std::array<std::function<void()>, 128> CALL;
    std::vector<Wk> WK;
    // Prev, Curr
    std::vector<Wk>::iterator wk[2] { WK.begin(), WK.begin() }; 
    std::vector<Mon> MON;
    std::vector<Mon>::const_iterator mon;
    Xlib::Win rootw;
    //::GC wkgc;
    //::GC statusgc;
    private:
  };
  
  template<typename T>
  struct Recv : private Wm {
    void key(const T&);
    void button(const T&);
    void motion(const T&);
    void crossing(const T&);
    void expose(const T&);
    void unmap(const T&);
    void map(const T&);
    void maprequest(const T&);
    void configure(const T&);
    void configurerequest(const T&);
    void property(const T&);
    void clientmessage(const T&);
  };
}
