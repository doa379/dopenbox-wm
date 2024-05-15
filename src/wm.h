#pragma once

#include <vector>
#include <tuple>
#include <Xlib.h>

namespace some {
  template<typename S, typename T>
  struct Dim : private std::pair<S, T> {
    Dim() : std::pair<S, T> { } { }
    Dim(S const& s, T const& t) : std::pair<S, T> { s, t } { }
    Dim(std::pair<S, T> const& p) : std::pair<S, T> { p } { }
    S& x() noexcept { return this->first; }
    S x() const noexcept { return this->first; }
    S& w() noexcept { return this->first; }
    S w() const noexcept { return this->first; }
    T& y() noexcept { return this->second; }
    T y() const noexcept { return this->second; }
    T& h() noexcept { return this->second; }
    T h() const noexcept { return this->second; }
  };

  struct Client {
    xlib::Win win;
    xlib::Win parw;
    //::GC gc;
    Dim<int, int> pos;
    Dim<int, int> size;
    unsigned mode;
    int sel;
    int ft;
  };

  // Impl. un/loading wks at runtime
  struct Wk {
    std::vector<Client> C;
    std::size_t prevc { };
    std::size_t currc { };
  };

  struct Mon {
    Dim<int, int> pos;
    Dim<int, int> size;
  };

  class Draw {
    public:
    Draw();
    ~Draw();
    private:
    xlib::Draw const draw;
  };

  class Wm {
    public:
    Wm();
    ~Wm();
    void unfocus(xlib::Win const) const;
    void focus(xlib::Win const) const;
    void prev_client();
    void next_client();
    void kill_client();
    protected:
    xlib::Xlib const xlib;
    xlib::Win const rootw;
    xlib::Input const input;
    std::unordered_map<int, int> KCODE_KSYM;
    std::array<std::function<void()>, 128> CALL;
    std::vector<Wk> WK;
    std::size_t prevwk { };
    std::size_t currwk { };
    std::vector<Mon> MON;
    std::size_t mon { };
    //::GC wkgc;
    //::GC statusgc;
    static auto constexpr VO { 14 };
    private:
    enum colors { BG, SEL, FG };
    Draw draw;
  };
  
  struct Recv : private Wm {
    void key(data::L const&);
    void button(data::L const&) const;
    void motion(data::L const&) const;
    void crossing(data::L const&) const;
    void expose(data::L const&) const;
    void unmap(data::L const&);
    void map(data::L const&) const;
    void maprequest(data::L const&);
    void configure(data::L const&) const;
    void configurerequest(data::L const&) const;
    void property(data::L const&) const;
    void clientmessage(data::Msg const&) const;
  };
}
