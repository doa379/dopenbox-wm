#include <iostream>
#include <algorithm>

#include "wm.h"
#include "Xlib.h"
#include "sig.h"
#include "../config.h"

int some::Ui::clen = conf::ui::CLEN;
int some::Ui::icow = conf::ui::ICOW;
int some::Ui::icostrlen = conf::ui::ICOSTRLEN;
int some::Ui::winstr = conf::ui::WINSTR;
int some::Ui::cascoset = conf::ui::CASCOSET;
int some::Ui::bdrw = conf::ui::BDRW;
std::array<std::size_t, 4> some::Ui::COLORS {
  conf::ui::COLORS[Ui::BG],
  conf::ui::COLORS[Ui::ACTSEL],
  conf::ui::COLORS[Ui::SEL],
  conf::ui::COLORS[Ui::FG]
};

int
main(int const argc, char const* ARGV[]) {
  try {
    static some::Display const dpy;
    dpy.init();
    
    {
      some::xlib::DefaultXError const error; 
      ::XSelectInput(dpy.ptr, dpy.root, 
        SubstructureRedirectMask);
      ::XSync(dpy.ptr, false);
      if (error.get())
        throw std::runtime_error(
          "Initialization error (another wm running?)");
    }

    static some::xlib::Font font;
    font.init(conf::FONT);
    static some::xlib::Cursor cursor;
    cursor.init();
    static some::xlib::Prop prop;
    prop.init();

    static some::Root root { conf::WMNAME };
    std::unordered_map<conf::Calls,
      std::function<void()>> calls {
        { conf::Calls::WK0, [] { } },
        { conf::Calls::WK1, [] { root.sw_wk(1); } },
        { conf::Calls::WK2, [] { root.sw_wk(2); } },
        { conf::Calls::WK3, [] { root.sw_wk(3); } },
        { conf::Calls::WK4, [] { root.sw_wk(4); } },
        { conf::Calls::WK5, [] { root.sw_wk(5); } },
        { conf::Calls::WK6, [] { root.sw_wk(6); } },
        { conf::Calls::WK7, [] { root.sw_wk(7); } },
        { conf::Calls::WK8, [] { root.sw_wk(8); } },
        { conf::Calls::WK9, [] { root.sw_wk(9); } },
        { conf::Calls::LOADWK, [] { root.load_wk(); } },
        { conf::Calls::UNLOADWK, 
          [] { root.unload_wk(); } },
        { conf::Calls::MON0, [] { root.sw_mon(0); } },
        { conf::Calls::MON1, [] { root.sw_mon(1); } },
        { conf::Calls::MON2, [] { root.sw_mon(2); } },
        { conf::Calls::MON3, [] { root.sw_mon(3); } },
        { conf::Calls::MON4, [] { root.sw_mon(4); } },
        { conf::Calls::MON5, [] { root.sw_mon(5); } },
        { conf::Calls::MON6, [] { root.sw_mon(6); } },
        { conf::Calls::MON7, [] { root.sw_mon(7); } },
        { conf::Calls::MON8, [] { root.sw_mon(8); } },
        { conf::Calls::MON9, [] { root.sw_mon(9); } },
        { conf::Calls::SWFOCUS, [] { } },
        { conf::Calls::TOGGLEMODE, [] { } },
        { conf::Calls::NEXT, 
          [] { root.next(); } },
        { conf::Calls::PREV, 
          [] { root.prev(); } },
        { conf::Calls::ROTATE_NEXT, 
          [] { root.rotate_next(); } },
        { conf::Calls::ROTATE_PREV, 
          [] { root.rotate_prev(); } },
        { conf::Calls::KILL, 
          [] { root.kill_client(); } },
        { conf::Calls::SELTOGGLE, 
          [] { root.toggle_sel(); } },
        { conf::Calls::SELCLEAR, 
          [] { root.clear_sel(); } },
        { conf::Calls::QUIT, [] { std::raise(SIGINT); } },
        { conf::Calls::MOVE, 
          [] { root.move_client(); } },
        { conf::Calls::RESIZE, 
          [] { root.resize_client(); } },
        { conf::Calls::STATE, [] { } }
    };
    
    static some::Kbd kbd;
    for (auto const& k : conf::KBD) {
      if (std::holds_alternative<char const*>(k.var))
        kbd.init(k.mod, k.sym, 
          std::get<char const*>(k.var));
      else if (std::holds_alternative<conf::Calls>(k.var)) {
        auto const c { std::get<conf::Calls>(k.var) };
        kbd.init(k.mod, k.sym, calls[c]);
      }
    }
    
    static some::Btn btn;
    for (auto const& b : conf::BTN) {
      auto const c { std::get<conf::Calls>(b.var) };
      btn.init(b.mod, b.sym, calls[c]);
    }

    kbd.grab_keys();
    root.init_wks(conf::NWKS);
    root.init_tree();
    root.init_mons();
    root.init_wgs();
    root.change_state();
    std::cout << conf::WMNAME << " initialized\n";
    using FnType = std::function<void(::XEvent const&)>;
    std::array<FnType, LASTEvent> F;
    std::ranges::for_each(F, [](auto& f) {
      f = [](::XEvent const&) { }; });
    
    F[KeyPress] = [](::XEvent const& xev) { 
      root.key(xev.xkey); };
    F[ButtonPress] = [](::XEvent const& xev) { 
      root.button_press(xev.xbutton); };
    F[ButtonRelease] = [](::XEvent const& xev) { 
      root.button_release(xev.xbutton); };
    F[MotionNotify] = [](::XEvent const& xev) { 
      root.motion(xev.xmotion); };
    F[EnterNotify] = [](::XEvent const& xev) { 
      root.crossing(xev.xcrossing); };
    F[FocusIn] = [](::XEvent const& xev) { 
      root.focus_change(xev.xfocus); };
    F[Expose] = [](::XEvent const& xev) { 
      root.expose(xev.xexpose); };
    F[UnmapNotify] = [](::XEvent const& xev) { 
      root.unmap(xev.xunmap); };
    F[MapNotify] = [](::XEvent const& xev) { 
      root.map(xev.xmap); };
    F[MapRequest] = [](::XEvent const& xev) { 
      root.map_request(xev.xmaprequest); };
    F[DestroyNotify] = [](::XEvent const& xev) { 
      root.destroy(xev.xdestroywindow); };
    F[ConfigureNotify] = [](::XEvent const& xev) { 
      root.configure(xev.xconfigure); };
    F[ConfigureRequest] = [](::XEvent const& xev) { 
      root.configure_request(xev.xconfigurerequest); };
    F[PropertyNotify] = [](::XEvent const& xev) { 
      root.property(xev.xproperty); };
    F[ClientMessage] = [](::XEvent const& xev) { 
      root.client_message(xev.xclient); };
    F[MappingNotify] = [](::XEvent const& xev) { 
      root.mapping(xev.xmapping); };
    
    some::Ev ev;
    some::Sig const sig;
    for (auto gen { ev.seq() }; sig.none();) {
      auto const xev { gen.next() };
      F[xev.type](xev);
      ::XSync(dpy.ptr, false);
    }

    root.deinit();
    prop.deinit();
    cursor.deinit();
    font.deinit();
    dpy.deinit();
    std::cout << conf::WMNAME << " exit\n";
  } catch (std::exception const& ex) {
    std::cerr << ex.what() << "\n";
  }

  return 0;
}
