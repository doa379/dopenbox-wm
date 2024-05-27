#pragma once

#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <array>
#include <functional>
#include <variant>

namespace some {
  struct Display {
    Display() = default;
    Display(Display const&) = delete;
    Display& operator=(Display const&) = delete;
    Display(Display&&) = delete;
    ~Display() = default;
    static ::Display* ptr;
    static void init();
    static void deinit() noexcept;
    int connection() const noexcept;
  };
  
  namespace data {
    struct L {
      long D[12];
      long& operator[](std::size_t const I) noexcept {
        return D[I]; }
      long operator[](std::size_t const I) const noexcept {
        return D[I]; }
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
    using S = std::function<void(data::T const&)>;
    public:
    Ev();
    ~Ev() = default;
    bool next() noexcept;
    void sync() const noexcept;
    S call(data::T&) const noexcept;
    void init_key(S const&) noexcept;
    S key(S const&, data::L&) const noexcept;
    void init_button(S const&) noexcept;
    S button(S const&, data::L&) const noexcept;
    void init_motion(S const&) noexcept;
    S motion(S const&, data::L&) const noexcept;
    void init_crossing(S const&) noexcept;
    S crossing(S const&, data::L&) const noexcept;
    void init_focuschange(S const&) noexcept;
    S focuschange(S const&, data::L&) const noexcept;
    void init_expose(S const&) noexcept;
    S expose(S const&, data::L&) const noexcept;
    void init_graphicsexpose(S const&) noexcept;
    S graphicsexpose(S const&, data::L&) const noexcept;
    void init_noexpose(S const&) noexcept;
    S noexpose(S const&, data::L&) const noexcept;
    void init_visibility(S const&) noexcept;
    S visibility(S const&, data::L&) const noexcept;
    void init_createwindow(S const&) noexcept;
    S createwindow(S const&, data::L&) const noexcept;
    void init_destroywindow(S const&) noexcept;
    S destroywindow(S const&, data::L&) const noexcept;
    void init_unmap(S const&) noexcept;
    S unmap(S const&, data::L&) const noexcept;
    void init_map(S const&) noexcept;
    S map(S const&, data::L&) const noexcept;
    void init_maprequest(S const&) noexcept;
    S maprequest(S const&, data::L&) const noexcept;
    void init_reparent(S const&) noexcept;
    S reparent(S const&, data::L&) const noexcept;
    void init_configure(S const&) noexcept;
    S configure(S const&, data::L&) const noexcept;
    void init_gravity(S const&) noexcept;
    S gravity(S const&, data::L&) const noexcept;
    void init_resizerequest(S const&) noexcept;
    S resizerequest(S const&, data::L&) const noexcept;
    void init_configurerequest(S const&) noexcept;
    S configurerequest(S const&, data::L&) const noexcept;
    void init_circulate(S const&) noexcept;
    S circulate(S const&, data::L&) const noexcept;
    void init_circulaterequest(S const&) noexcept;
    S circulaterequest(S const&, data::L&) const noexcept;
    void init_property(S const&) noexcept;
    S property(S const&, data::L&) const noexcept;
    void init_selectionclear(S const&) noexcept;
    S selectionclear(S const&, data::L&) const noexcept;
    void init_selectionrequest(S const&) noexcept;
    S selectionrequest(S const&, data::L&) const noexcept;
    void init_selection(S const&) noexcept;
    S selection(S const&, data::L&) const noexcept;
    void init_colormap(S const&) noexcept;
    S colormap(S const&, data::L&) const noexcept;
    void init_clientmessage(S const&) noexcept;
    S clientmessage(S const&, data::Msg&) const noexcept;
    void init_mapping(S const&) noexcept;
    S mapping(S const&, data::L&) const noexcept;
    void init_keymap(S const&) noexcept;
    S keymap(S const&, data::Keymap&) const noexcept;
    private:
    static const Display dpy;
    ::XEvent xev;
    std::array<std::function<S(data::T&)>, LASTEvent> F;
  };

  class Sys {
    public:
    void spawn(char const []) const noexcept;
    private:
    static Display const dpy;
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
      SubstructureRedirectMask |
      SubstructureNotifyMask
    };
    
    class DefaultXError {
      public:
      DefaultXError() noexcept;
      ~DefaultXError() = default;
      bool get() const noexcept;
      private:
      static bool xerror;
      static int handler(::Display*, ::XErrorEvent*) 
      noexcept;
    };

    using Win = ::Window;
    class Xlib {
      public:
      Win root() const noexcept;
      int dpy_width() const noexcept;
      int dpy_height() const noexcept;
      int depth() const noexcept;
      Win create_win(Win const, int const, int const)
      const noexcept;
      void destroy_win(Win const) const noexcept;
      void map_win(Win const) const noexcept;
      void unmap_win(Win const) const noexcept;
      void set_winbg(Win const, std::size_t const)
      const noexcept;
      void set_bdrcolor(Win const, std::size_t const) 
      const noexcept;
      void set_bdrwidth(Win const, int const)
      const noexcept;
      void move_win(Win const, int const, int const)
      const noexcept;
      void repar_win(Win const, Win const, int const, int const) 
      const noexcept;
      ::GC default_gc() const noexcept;
      private:
      static Display const dpy;
    };

    class Input {
      public:
      void select(Win const, long const) const noexcept;
      void set_focus(Win const) const noexcept;
      unsigned modmask() const noexcept;
      ::KeyCode keysym_keycode(::KeySym const)
      const noexcept;
      ::KeySym keycode_keysym(::KeyCode const)
      const noexcept;
      void grab_key(Win const, int const, int const)
      const noexcept;
      void ungrab_key(Win const, int const, int const)
      const noexcept;
      void ungrab_allkey(Win const) const noexcept;
      void grab_btn(Win const, int const, int const)
      const noexcept;
      void ungrab_btn(Win const, int const, int const)
      const noexcept;
      void ungrab_pointer() const noexcept;
      void warp_pointer(Win const, int const, int const)
      const noexcept;
      private:
      static Display const dpy;
    };
    
    class WinAttr {
      public:
      WinAttr() = delete;
      WinAttr(Win const);
      bool override_redirect() const noexcept;
      std::pair<int, int> size() const noexcept;
      std::pair<int, int> pos() const noexcept;
      private:
      static Display const dpy;
      ::XWindowAttributes wa;
    };

    class QueryTree {
      public:
      QueryTree(Win const);
      ~QueryTree();
      std::vector<Win> get() const noexcept;
      private:
      static Display const dpy;
      Win* wins { };
      unsigned n { };
    };

    class Xinerama {
      public:
      Xinerama();
      ~Xinerama();
      int number() const noexcept;
      // { pos, size }
      std::pair<std::pair<int, int>, std::pair<int, int>>
      query(std::size_t const) const noexcept;
      private:
      static Display const dpy;
      ::XineramaScreenInfo* screeninfo;
      int n;
    };
    
    class Prop {
      public:
      Prop() noexcept;
      void change_state(Win const) const noexcept;
      private:
      static Display const dpy;
      ::Atom wm_protocols;
      ::Atom wm_name;
      ::Atom wm_delete_window;
      ::Atom wm_state;
      ::Atom wm_take_focus;
      ::Atom wm_icon_name;
      ::Atom net_supported;
      ::Atom net_wm_state;
      ::Atom net_wm_name;
      ::Atom net_wm_window_opacity;
      ::Atom net_active_window;
      ::Atom net_wm_state_fullscreen;
      ::Atom net_wm_window_type;
      ::Atom net_wm_window_type_dialog;
      ::Atom net_client_list;
      ::Atom net_number_of_desktops;
      ::Atom net_wm_desktop;
      ::Atom net_current_desktop;
      ::Atom net_showing_desktop;
      ::Atom net_wm_icon;
      ::Atom net_wm_icon_name;
    };

    namespace draw {
      class Font {
        public:
        Font() = delete;
        Font(char const []);
        ~Font();
        int get_scent() const noexcept;
        int get_ascent() const noexcept;
        int get_descent() const noexcept;
        int text_width(char const []) const noexcept;
        int text_width16(wchar_t const []) const noexcept;
        private:
        static Display const dpy;
        ::XFontStruct* fn;
        int scent;

      };

      class Gc {
        public:
        Gc() = delete;
        Gc(Win const) noexcept;
        ~Gc();
        ::GC get() const noexcept;
        private:
        static Display const dpy;
        ::GC gc;
      };

      class Pixmap {
        public:
        Pixmap() = delete;
        Pixmap(Win const, int const, int const, int const)
        noexcept;
        ~Pixmap();
        void fill(Win const, ::GC const, std::size_t, 
        int const, int const, int const, int const) 
        const noexcept;
        void draw_string(char const [], 
        Win const, ::GC const, std::size_t const,
        int const, int const) const noexcept;
        private:
        static Display const dpy;
        ::Drawable drawable;
      };
    }
  }
}
