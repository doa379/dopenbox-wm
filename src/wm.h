#pragma once

#include <vector>
#include <tuple>
#include <Xlib.h>

namespace some {
  struct Client {
    xlib::Win win;
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
    std::vector<Client>::const_iterator client[2]; 
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
    void unfocus(const xlib::Win);
    void focus(const xlib::Win);
    void prev_client();
    void next_client();
    void kill_client();
    protected:
    Display dpy;
    xlib::Xlib xlib;
    xlib::Input input;
    xlib::Draw draw;
    std::unordered_map<int, int> KCODE_KSYM;
    std::array<std::function<void()>, 128> CALL;
    std::vector<Wk> WK;
    // Prev, Curr
    std::vector<Wk>::iterator wk[2]; 
    std::vector<Mon> MON;
    std::vector<Mon>::const_iterator mon;
    xlib::Win rootw;
    //::GC wkgc;
    //::GC statusgc;
    private:
  };
  
  struct Recv : private Wm {
    void key(const data::L&);
    void button(const data::L&);
    void motion(const data::L&);
    void crossing(const data::L&);
    void expose(const data::L&);
    void unmap(const data::L&);
    void map(const data::L&);
    void maprequest(const data::L&);
    void configure(const data::L&);
    void configurerequest(const data::L&);
    void property(const data::L&);
    void clientmessage(const data::Msg&);
  };
}
