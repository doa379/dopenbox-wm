#pragma once

#include <vector>
#include <tuple>

#include "Xlib.h"
#include "palette.h"

namespace some {
  template<typename S, typename T>
  struct Dim : private std::pair<S, T> {
    Dim() : std::pair<S, T> { } { }
    Dim(S const& s, T const& t) : 
      std::pair<S, T> { s, t } { }
    Dim(std::pair<S, T> const& p) :
      std::pair<S, T> { p } { }
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
    Dim<int, int> pos;
    Dim<int, int> size;
    unsigned mode;
    bool sel { };
  };

  struct Wk {
    std::vector<Client> C;
    std::size_t prevc { };
    std::size_t currc { };
  };

  struct Mon {
    Dim<int, int> pos;
    Dim<int, int> size;
  };

  class Wm {
    public:
    Wm();
    ~Wm();
    void sw_wk(unsigned const) noexcept;
    void sw_mon(unsigned const) const noexcept;
    void unmap_all() const noexcept;
    void map_all() const noexcept;
    void load_wk() noexcept;
    void unload_wk() noexcept;
    void unfocus(xlib::Win const) const noexcept;
    void focus(xlib::Win const) const noexcept;
    void prev_client() noexcept;
    void next_client() noexcept;
    void kill_client();
    void seltoggle() noexcept;
    void selclear() noexcept;
    void refresh_panel() const noexcept;
    void change_root_state() const noexcept;
    protected:
    xlib::Xlib const xlib;
    xlib::Win const rootwin;
    xlib::Input const input;
    xlib::draw::Font const font;
    std::unordered_map<int, int> KCODE_KSYM;
    std::array<std::function<void()>, 128> CALL;
    std::vector<Wk> WK;
    std::size_t prevwk { };
    std::size_t currwk { };
    std::vector<Mon> MON;
    std::size_t mon { };
    private:
    enum colors { BG, SEL, FG };
    static auto constexpr ROOTBG { Gray70 };
    xlib::Prop const atom;
    xlib::draw::Pixmap const rootpix;
  };
  
  struct Recv : private Wm {
    void key(data::L const&) noexcept;
    void button(data::L const&) const noexcept;
    void motion(data::L const&) const noexcept;
    void crossing(data::L const&) const noexcept;
    void expose(data::L const&) const noexcept;
    void unmap(data::L const&) noexcept;
    void map(data::L const&) const noexcept;
    void maprequest(data::L const&) noexcept;
    void configure(data::L const&) const noexcept;
    void configurerequest(data::L const&) const noexcept;
    void property(data::L const&) const noexcept;
    void clientmessage(data::Msg const&) const noexcept;
  };
}
