#pragma once

#include <X11/Xlib.h>
#include <array>
#include <functional>
#include <variant>

namespace some {
  struct Display {
    Display() = default;
    Display(const Display&) = delete;
    Display& operator=(const Display&) = delete;
    Display(Display&&) = delete;
    ~Display() = default;
    static ::Display* ptr;
    static void init();
    static void deinit();
  };
  
  namespace data {
    struct L {
      long D[12];
      long& operator[](const std::size_t I) noexcept { return D[I]; }
      long operator[](const std::size_t I) const noexcept { return D[I]; }
    };

    struct Msg : public L {
      union {
        char B[20];
        short S[10];
        long L[5];
      };
    };
    
    struct Keymap : public L {
      char KEYMAP_VECTOR[32];
    };

    using T = std::variant<L, Msg, Keymap>;
  }

  class Ev {
    using S = std::function<void(const data::T&)>;
    public:
    Ev();
    ~Ev() = default;
    bool next() noexcept;
    void sync() const noexcept;
    S call(data::T&) const noexcept;
    void init_key(const S&) noexcept;
    S key(const S&, data::L&) const noexcept;
    void init_button(const S&) noexcept;
    S button(const S&, data::L&) const noexcept;
    void init_motion(const S&) noexcept;
    S motion(const S&, data::L&) const noexcept;
    void init_crossing(const S&) noexcept;
    S crossing(const S&, data::L&) const noexcept;
    void init_focuschange(const S&) noexcept;
    S focuschange(const S&, data::L&) const noexcept;
    void init_expose(const S&) noexcept;
    S expose(const S&, data::L&) const noexcept;
    void init_graphicsexpose(const S&) noexcept;
    S graphicsexpose(const S&, data::L&) const noexcept;
    void init_noexpose(const S&) noexcept;
    S noexpose(const S&, data::L&) const noexcept;
    void init_visibility(const S&) noexcept;
    S visibility(const S&, data::L&) const noexcept;
    void init_createwindow(const S&) noexcept;
    S createwindow(const S&, data::L&) const noexcept;
    void init_destroywindow(const S&) noexcept;
    S destroywindow(const S&, data::L&) const noexcept;
    void init_unmap(const S&) noexcept;
    S unmap(const S&, data::L&) const noexcept;
    void init_map(const S&) noexcept;
    S map(const S&, data::L&) const noexcept;
    void init_maprequest(const S&) noexcept;
    S maprequest(const S&, data::L&) const noexcept;
    void init_reparent(const S&) noexcept;
    S reparent(const S&, data::L&) const noexcept;
    void init_configure(const S&) noexcept;
    S configure(const S&, data::L&) const noexcept;
    void init_gravity(const S&) noexcept;
    S gravity(const S&, data::L&) const noexcept;
    void init_resizerequest(const S&) noexcept;
    S resizerequest(const S&, data::L&) const noexcept;
    void init_configurerequest(const S&) noexcept;
    S configurerequest(const S&, data::L&) const noexcept;
    void init_circulate(const S&) noexcept;
    S circulate(const S&, data::L&) const noexcept;
    void init_circulaterequest(const S&) noexcept;
    S circulaterequest(const S&, data::L&) const noexcept;
    void init_property(const S&) noexcept;
    S property(const S&, data::L&) const noexcept;
    void init_selectionclear(const S&) noexcept;
    S selectionclear(const S&, data::L&) const noexcept;
    void init_selectionrequest(const S&) noexcept;
    S selectionrequest(const S&, data::L&) const noexcept;
    void init_selection(const S&) noexcept;
    S selection(const S&, data::L&) const noexcept;
    void init_colormap(const S&) noexcept;
    S colormap(const S&, data::L&) const noexcept;
    void init_clientmessage(const S&) noexcept;
    S clientmessage(const S&, data::Msg&) const noexcept;
    void init_mapping(const S&) noexcept;
    S mapping(const S&, data::L&) const noexcept;
    void init_keymap(const S&) noexcept;
    S keymap(const S&, data::Keymap&) const noexcept;
    private:
    Display dpy;
    ::XEvent xev;
    std::array<std::function<S(data::T&)>, LASTEvent> F;
  };
  
  namespace xlib {
    // Look to select masks
    static constexpr auto ROOTMASK { 
      SubstructureRedirectMask | 
      SubstructureNotifyMask | 
      ButtonPressMask |
      PointerMotionMask |
      EnterWindowMask |
      LeaveWindowMask |
      StructureNotifyMask |
      PropertyChangeMask |
      ExposureMask
    };
    
    static constexpr auto PARMASK {
      SubstructureRedirectMask | SubstructureNotifyMask
    };
    
    struct Atom {
      ::Atom WM_PROTOCOLS;
      ::Atom WM_NAME;
      ::Atom WM_DELETE_WINDOW;
      ::Atom WM_STATE;
      ::Atom WM_TAKE_FOCUS;
      ::Atom WM_ICON_NAME;
      ::Atom NET_SUPPORTED;
      ::Atom NET_WM_STATE;
      ::Atom NET_WM_NAME;
      ::Atom NET_WM_WINDOW_OPACITY;
      ::Atom NET_ACTIVE_WINDOW;
      ::Atom NET_WM_STATE_FULLSCREEN;
      ::Atom NET_WM_WINDOW_TYPE;
      ::Atom NET_WM_WINDOW_TYPE_DIALOG;
      ::Atom NET_CLIENT_LIST;
      ::Atom NET_NUMBER_OF_DESKTOPS;
      ::Atom NET_WM_DESKTOP;
      ::Atom NET_CURRENT_DESKTOP;
      ::Atom NET_SHOWING_DESKTOP;
      ::Atom NET_WM_ICON;
      ::Atom NET_WM_ICON_NAME;
    };
      
    class DefaultXError {
      public:
      DefaultXError() { ::XSetErrorHandler(handler); }
      ~DefaultXError() = default;
      bool get() { return xerror; }
      private:
      static bool xerror;
      static int handler(::Display*, ::XErrorEvent* xev) {
        xerror = (xev->error_code == BadAccess ||
          xev->error_code == BadWindow);
        return 0;
      };
    };

    using Win = ::Window;
    class Xlib {
      public:
      Win root() const noexcept;
      Win create_window(const Win, const int, const int) const noexcept;
      void destroy_window(const Win) const noexcept;
      void mapwindow(const Win) const noexcept;
      void unmapwindow(const Win) const noexcept;
      void set_bdrcolor(const Win, const std::size_t) const noexcept;
      void set_bdrwidth(const Win, const int) const noexcept;
      void movewindow(const Win, const int, const int) const noexcept;
      void reparent(const Win, const Win, const int, const int) const noexcept;
      private:
      Display dpy;
    };

    class Input {
      public:
      void select(const Win, const long) const noexcept;
      void set_focus(const Win) const noexcept;
      unsigned modmask() const noexcept;
      void grab_key(const Win, const int, const int) const noexcept;
      void ungrab_key(const Win, const int, const int) const noexcept;
      void ungrab_allkey(const Win) const noexcept;
      void grab_btn(const Win, const int, const int) const noexcept;
      void ungrab_btn(const Win, const int, const int) const noexcept;
      void ungrab_pointer() const noexcept;
      void warp_pointer(const Win, const int, const int) const noexcept;
      private:
      Display dpy;
    };
    
    class WinAttr {
      public:
      WinAttr() = delete;
      WinAttr(const Win WIN) : 
        status { ::XGetWindowAttributes(dpy.ptr, WIN, &wa) } { }
      bool isvalid() const noexcept { 
        return !(status == BadDrawable || status == BadWindow); }
      bool override_redirect() const noexcept { return wa.override_redirect; }
      std::pair<int, int> size() const noexcept { 
        return { wa.width, wa.height }; }
      std::pair<int, int> pos() const noexcept { return { wa.x, wa.y }; }
      private:
      Display dpy;
      ::XWindowAttributes wa;
      Status status;
    };

    class QueryTree {
      public:
      QueryTree(const Win);
      ~QueryTree();
      std::vector<Win> get() const noexcept;
      private:
      Display dpy;
      Win* wins { };
      unsigned n { };
    };

    class Xinerama {
      public:
      private:
      Display dpy;
    };

    class Draw {
      public:
      private:
      Display dpy;
    };
  }
}
