#pragma once

#include <vector>
#include <tuple>
#include <map>

#include "Xlib.h"

namespace some {
  struct Ui {
    static int clen;
    static int icow;
    static int icostrlen;
    static int winstr;
    static int cascoset;
    static int bdrw;
    static std::array<std::size_t, 4> COLORS;
    enum colors { BG, ACTSEL, SEL, FG };
  };

  class Arrange {
    public:
    Arrange() = default;
    ~Arrange() = default;
    std::pair<int, int> cascade(::Window const, 
      int const, int const) 
    const noexcept;
    void center(::Window const, int const, int const, 
      int const, int const) const noexcept;
    private:
    static Ui const ui;
  };

  class Kbd {
    friend class Root;
  public:
    Kbd() noexcept;
    ~Kbd();
    static void init(unsigned const, unsigned const,
      std::function<void()> const&) noexcept;
    static void init(unsigned const, unsigned const,
      char const*) noexcept;
    static void grab_keys();
    static unsigned kmod(unsigned const);
  private:
    static Display const dpy;
    static std::unordered_map<unsigned, unsigned> 
      kcode_ksym;
    static std::vector<std::pair<unsigned, unsigned>> 
      KMOD_KSYM;
    static std::map<std::pair<unsigned, unsigned>, 
      std::function<void()>> call;
    static std::map<std::pair<unsigned, unsigned>, 
      std::string_view> shell;
    static unsigned numlockmask;
  };

  class Btn {
    friend class Root;
    friend class Client;
  public:
    static void init(unsigned const, unsigned const,
      std::function<void()> const&) noexcept;
  private:
    static std::vector<std::pair<unsigned, unsigned>> 
      KMOD_BTN;
    static std::map<std::pair<unsigned, unsigned>, 
      std::function<void()>> btn;
  };

  struct Wg {
    Wg() = default;
    Wg(::Window const, int const, int const, int const) 
      noexcept;
    Wg(Wg const&) = delete;
    Wg& operator=(Wg const&) = delete;
    Wg(Wg&&) noexcept;
    Wg& operator=(Wg&&) noexcept;
    ~Wg();
    static Display const dpy;
    ::Window win { };
    some::xlib::Gc gc;
    //some::xlib::Gc bg;
    //some::xlib::Gc fg;
    //some::xlib::Gc sel;
    some::xlib::Draw draw;
    int w { };
    int h { };
  };

  class Client {
    //friend struct Wm;
    friend class Root;
  public:
    Client() = delete;
    Client(::Window const, int const, int const, 
    int const, int const) noexcept;
    Client(Client const&) = delete;
    Client& operator=(Client const&) = delete;
    Client(Client&&) noexcept;
    Client& operator=(Client&&) noexcept;
    ~Client();
    void map() const noexcept;
    void unmap() const noexcept;
    void repar();
    void set_bg(std::size_t const) const noexcept;
    void set_bdrcolor(std::size_t const) const noexcept;
    void grab_btns() const noexcept;
    void grab_any_btn() const noexcept;
    void set_focus() const noexcept;
    void toggle_sel(std::size_t const, std::size_t const)
    noexcept;
    void clear_sel(std::size_t const) noexcept;
    void refresh() const noexcept;
    bool move(int const, int const) noexcept;
    void move(int const, int const, int const, int const) noexcept;
    bool resize(int const, int const) noexcept;
    void resize(int const, int const, int const, int const)
    noexcept;
  private:
    static Display const dpy;
    static xlib::Prop prop;
    static xlib::Font font;
    static xlib::Cursor cursor;
    static Btn const btn;
    static Ui const ui;
    ::Window win { };
    Wg par;
    Wg btn0;
    Wg btn1;
    Wg btn2;
    Wg icon;
    int x { };
    int y { };
    int w { };
    int h { };
    int ptr_x { };
    int ptr_y { };
    int ptr_x_root { };
    int ptr_y_root { };
    unsigned mode { };
    bool sel { };
  };

  class Root {
    class Wk {
      //friend struct Wm;
      friend class Root;
    public:
      Wk() noexcept;
      Wk(Wk const&) = delete;
      Wk& operator=(Wk const&) = delete;
      Wk(Wk&&) noexcept;
      Wk& operator=(Wk&&) noexcept;
      ~Wk() = default;
      void init_wg(int const w, int const h) noexcept;
    private:
      std::vector<Client> C;
      //std::vector<Client>::iterator prevc;
      ssize_t prevc { };
      //std::vector<Client>::iterator currc;
      ssize_t currc { };
      Wg wg;
    };

    struct Mon {
      int x;
      int y;
      int w;
      int h;
    };

    public:
    Root() = delete;
    Root(char const []) noexcept;
    ~Root();
    void init_wks(unsigned const) noexcept;
    void init_tree() noexcept;
    void init_mons() noexcept;
    void init_wgs() noexcept;
    void deinit() noexcept;
    void sw_wk(unsigned const) noexcept;
    void sw_mon(unsigned const) const noexcept;
    void load_wk() noexcept;
    void unload_wk() noexcept;
    void focus() noexcept;
    void next() noexcept;
    void prev() noexcept;
    void rotate_next() noexcept;
    void rotate_prev() noexcept;
    void kill_client();
    void toggle_sel() noexcept;
    void clear_sel() noexcept;
    void refresh_root() noexcept;
    void refresh_panels() noexcept;
    void change_state() const noexcept;
    void move_client() const noexcept;
    void resize_client() const noexcept;
    // Events
    void key(::XKeyEvent const&) noexcept;
    void button_press(::XButtonEvent const&) noexcept;
    void button_release(::XButtonEvent const&) noexcept;
    void motion(::XMotionEvent const&) const noexcept;
    void crossing(::XCrossingEvent const&) noexcept;
    void focus_change(::XFocusChangeEvent const&) noexcept;
    void expose(::XExposeEvent const&) const noexcept;
    void unmap(::XUnmapEvent const&) noexcept;
    void map(::XMapEvent const&) const noexcept;
    void map_request(::XMapRequestEvent const&) noexcept;
    void destroy(::XDestroyWindowEvent const&) noexcept;
    void configure(::XConfigureEvent const&) 
    const noexcept;
    void configure_request(::XConfigureRequestEvent const&)
    const noexcept;
    void property(::XPropertyEvent const&) noexcept;
    void client_message(::XClientMessageEvent const&)
    const noexcept;
    void mapping(::XMappingEvent const&) noexcept;
    // End Events
    protected:
    Arrange const arrange;
    Sys const sys;
    // Data
    std::vector<Wk> wk;
    std::vector<Wk>::iterator prevwk;
    std::vector<Wk>::iterator currwk;
    std::vector<Mon> mon;
    std::vector<Mon>::iterator currmon;
    //
    private:
    static Display const dpy;
    static xlib::Font font;
    static xlib::Cursor cursor;
    static xlib::Prop prop;
    static Kbd kbd;
    static Btn btn;
    static Ui const ui;
    std::string_view wmname;
    Wg status;
    // Where we want to show just single icon
    Wg icon;
    // Data
    std::vector<std::reference_wrapper<Client>> SELC;
  };
 /* 
  struct Wm : private Root {
    static Display const dpy;
    Wm() noexcept;
    void key(::XKeyEvent const&) noexcept;
    void button_press(::XButtonEvent const&) noexcept;
    void button_release(::XButtonEvent const&) noexcept;
    void motion(::XMotionEvent const&) const noexcept;
    void crossing(::XCrossingEvent const&) noexcept;
    void expose(::XExposeEvent const&) const noexcept;
    void unmap(::XUnmapEvent const&) noexcept;
    void map(::XMapEvent const&) const noexcept;
    void map_request(::XMapRequestEvent const&) noexcept;
    void configure(::XConfigureEvent const&) 
    const noexcept;
    void configure_request(::XConfigureRequestEvent const&)
    const noexcept;
    void property(::XPropertyEvent const&) noexcept;
    void client_message(::XClientMessageEvent const&)
    const noexcept;
  };
  */
}
