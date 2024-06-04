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
    Client() = delete;
    Client(xlib::Win const, xlib::Win const, 
    Dim<int, int> const&, Dim<int, int> const&, 
    int const) noexcept;
    Client(Client const&) = delete;
    Client(Client&&) noexcept;
    Client& operator=(Client&&) noexcept;
    xlib::Win win;
    xlib::Win parw;
    xlib::draw::Gc gc;
    xlib::draw::Draw draw;
    Dim<int, int> pos;
    Dim<int, int> size;
    unsigned mode;
    // Remove selection from Client
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
    void unfocus(Client const&) const noexcept;
    void focus(Client const&) const noexcept;
    void prev_client() noexcept;
    void next_client() noexcept;
    void kill_client();
    void seltoggle() noexcept;
    void selclear() noexcept;
    void refresh_root() const noexcept;
    void refresh_panel() const noexcept;
    void change_root_state() const noexcept;
    protected:
    enum colors { BG, SEL, FG };
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
    static auto constexpr ROOTBG { Gray70 };
    xlib::Prop const atom;
    xlib::draw::Draw const root;
    xlib::draw::Gc rootgc;
    std::vector<std::reference_wrapper<Client>> SELC;
  };
  
  struct Recv : private Wm {
    void key(Data const&) noexcept;
    void button(Data const&) const noexcept;
    void motion(Data const&) const noexcept;
    void crossing(Data const&) const noexcept;
    void expose(Data const&) const noexcept;
    void unmap(Data const&) noexcept;
    void map(Data const&) const noexcept;
    void maprequest(Data const&) noexcept;
    void configure(Data const&) const noexcept;
    void configurerequest(Data const&) const noexcept;
    void property(Data const&) const noexcept;
    void clientmessage(Data const&) const noexcept;
  };
}
