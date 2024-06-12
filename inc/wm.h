#pragma once

#include <vector>
#include <tuple>

#include "Xlib.h"
#include "palette.h"

namespace some {
  class Arrange {

  };

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

  struct Root {
    Root() = delete;
    Root(xlib::Win const, int const, int const, int const);
    Root(Root&) = delete;
    Root& operator=(Root&) = delete;
    Root(Root const&) = delete;
    Root& operator=(Root const&) = delete;
    Root(Root&&) = delete;
    Root& operator=(Root&&) = delete;
    static auto constexpr ROOTBG { Gray70 };
    xlib::Win win;
    ////////////////////
    xlib::draw::Gc gc;
    xlib::draw::Draw const draw;
    Dim<int, int> size;
    ////////////////////
  };

  struct Client {
    Client() = delete;
    Client(xlib::Win const, xlib::Win const, 
    Dim<int, int> const&, Dim<int, int> const&, 
    int const, int const) noexcept;
    Client(Client const&) = delete;
    Client(Client&&) noexcept;
    Client& operator=(Client&&) noexcept;
    xlib::Win win;
    xlib::Win parw;
    ////////////////////
    xlib::draw::Gc gc;
    xlib::draw::Draw draw;
    Dim<int, int> pos;
    Dim<int, int> size;
    int th;
    ////////////////////
    unsigned mode;
    bool sel { };
  };

  class Ui {
    // Event driven class that manages ui/mouse input
    // Draws ui elements
    // Manages ui elements namely scaling

    // Define key elements of ui wrt size and position
    // Takes any given Root/Client as parameter

    public:
    void draw_wks(Root&);
    void draw_icons(Root&);
    void draw_status(Root&);
    void draw_titlebar(Client&);
    void draw_min(Client&);
    void max(Client&);
    void draw_minmax(Client&);
    void draw_kill(Client&);
    // test if cursor/focus is at any given element
    unsigned wks(Root&) { return 0; }
    unsigned icon(Root&) { return 0; }
    bool at_status(Root&) { return false; }
    bool at_title(Client&) { return false; }
    bool at_min(Client&) { return false; }
    bool at_max(Client&) { return false; }
    bool at_minmax(Client&) { return false; }
    bool at_kill(Client&) { return false; }
    private:
    float scaling { 1.0 };
    ////////////////////
    xlib::draw::Gc gc;
    xlib::draw::Draw draw;
    Dim<int, int> pos;
    Dim<int, int> size;
    int th;
    ////////////////////
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
    void exit() const noexcept;
    void sw_wk(unsigned const) noexcept;
    void sw_mon(unsigned const) const noexcept;
    void unmap_all() const noexcept;
    void map_all() noexcept;
    void load_wk() noexcept;
    void unload_wk() noexcept;
    void unfocus(Client const&) const noexcept;
    void focus(Client const&) noexcept;
    void prev_client() noexcept;
    void next_client() noexcept;
    void kill_client();
    void toggle_sel() noexcept;
    void clear_sel() noexcept;
    void refresh_root() const noexcept;
    void refresh_panel() const noexcept;
    void change_root_state() const noexcept;
    void change_wins_state() noexcept;
    protected:
    enum colors { BG, SEL, FG };
    static auto constexpr CASC_OSET { 14 };
    Root const root;
    xlib::Xlib const xlib;
    xlib::Input const input;
    xlib::draw::Font const font;
    xlib::draw::Cursor const cursor;
    std::unordered_map<int, int> KCODE_KSYM;
    std::array<std::function<void()>, 128> CALL;
    std::vector<Wk> WK;
    std::size_t prevwk { };
    std::size_t currwk { };
    std::vector<Mon> MON;
    std::size_t mon { };
    bool btn1_ispressed { };
    Dim<int, int> move_origin;
    private:
    static auto constexpr ROOTBG { Gray70 };
    xlib::Prop prop;
    std::vector<std::reference_wrapper<Client>> SELC;
  };
  
  struct Recv : private Wm {
    void key(Data const&) noexcept;
    void button_press(Data const&) noexcept;
    void button_release(Data const&) noexcept;
    void motion(Data const&) const noexcept;
    void crossing(Data const&) noexcept;
    void expose(Data const&) const noexcept;
    void unmap(Data const&) noexcept;
    void map(Data const&) const noexcept;
    void maprequest(Data const&) noexcept;
    void configure(Data const&) const noexcept;
    void configurerequest(Data const&) const noexcept;
    void property(Data const&) noexcept;
    void clientmessage(Data const&) const noexcept;
    void exit() const noexcept;
  };
}
