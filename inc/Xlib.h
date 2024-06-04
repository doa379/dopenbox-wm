#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xinerama.h>
#include <array>
#include <functional>

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
  
  struct Data {
    long D[16];
    long& operator[](std::size_t const I) noexcept {
      return D[I]; }
    long operator[](std::size_t const I) const noexcept {
      return D[I]; }
    union {
      char B[20];
      short S[10];
      long L[5];
    } msg;

    char KEYMAP_VECTOR[32];
  };

  class Ev {
    using T = std::function<void()>;
    using S = std::function<void(Data const&)>;
    using TS = std::pair<T, S>;
    public:
    Ev();
    ~Ev() = default;
    bool next() noexcept;
    void sync() const noexcept;
    void call() const noexcept;
    void init_key(S const&) noexcept;
    void key() noexcept;
    void init_button(S const&) noexcept;
    void button() noexcept;
    void init_motion(S const&) noexcept;
    void motion() noexcept;
    void init_crossing(S const&) noexcept;
    void crossing() noexcept;
    void init_focuschange(S const&) noexcept;
    void focuschange() noexcept;
    void init_expose(S const&) noexcept;
    void expose() noexcept;
    void init_graphicsexpose(S const&) noexcept;
    void graphicsexpose() noexcept;
    void init_noexpose(S const&) noexcept;
    void noexpose() noexcept;
    void init_visibility(S const&) noexcept;
    void visibility() noexcept;
    void init_createwindow(S const&) noexcept;
    void createwindow() noexcept;
    void init_destroywindow(S const&) noexcept;
    void destroywindow() noexcept;
    void init_unmap(S const&) noexcept;
    void unmap() noexcept;
    void init_map(S const&) noexcept;
    void map() noexcept;
    void init_maprequest(S const&) noexcept;
    void maprequest() noexcept;
    void init_reparent(S const&) noexcept;
    void reparent() noexcept;
    void init_configure(S const&) noexcept;
    void configure() noexcept;
    void init_gravity(S const&) noexcept;
    void gravity() noexcept;
    void init_resizerequest(S const&) noexcept;
    void resizerequest() noexcept;
    void init_configurerequest(S const&) noexcept;
    void configurerequest() const noexcept;
    void init_circulate(S const&) noexcept;
    void circulate() noexcept;
    void init_circulaterequest(S const&) noexcept;
    void circulaterequest() noexcept;
    void init_property(S const&) noexcept;
    void property() noexcept;
    void init_selectionclear(S const&) noexcept;
    void selectionclear() noexcept;
    void init_selectionrequest(S const&) noexcept;
    void selectionrequest() noexcept;
    void init_selection(S const&) noexcept;
    void selection() noexcept;
    void init_colormap(S const&) noexcept;
    void colormap() noexcept;
    void init_clientmessage(S const&) noexcept;
    void clientmessage() noexcept;
    void init_mapping(S const&) noexcept;
    void mapping() noexcept;
    void init_keymap(S const&) noexcept;
    void keymap() noexcept;
    private:
    static const Display dpy;
    ::XEvent xev;
    Data data;
    std::array<TS, LASTEvent> F;
  };

  class Sys {
    public:
    void spawn(char const []) const noexcept;
    private:
    static Display const dpy;
  };
  
  namespace xlib {
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
      SubstructureNotifyMask |
      ExposureMask
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
      void iconify_win(Win const) const noexcept;
      void set_winbg(Win const, std::size_t const)
      const noexcept;
      void set_bdrcolor(Win const, std::size_t const) 
      const noexcept;
      void set_bdrwidth(Win const, int const)
      const noexcept;
      void move_win(Win const, int const, int const)
      const noexcept;
      void repar_win(Win const, Win const, int const,
      int const) const noexcept;
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
        int text_width(char const [], std::size_t const)
        const noexcept;
        int text_width16(wchar_t const [],
        std::size_t const) const noexcept;
        private:
        static Display const dpy;
        ::XFontStruct* fn;
        int scent;

      };

      class Gc {
        public:
        Gc() = delete;
        Gc(Win const) noexcept;
        Gc(Gc const&) = delete;
        Gc(Gc&&) noexcept;
        Gc& operator=(Gc&&) noexcept;
        ~Gc();
        ::GC get() const noexcept;
        void set_fg(std::size_t const) const noexcept;
        void set_bg(std::size_t const) const noexcept;
        private:
        static Display const dpy;
        ::GC gc;
      };

      class Draw {
        public:
        Draw() = delete;
        Draw(Win const, int const, int const, int const)
        noexcept;
        Draw(Draw const&) = delete;
        Draw(Draw&&) noexcept;
        Draw& operator=(Draw&&) noexcept;
        ~Draw();
        void fill(::GC const, int const, int const, 
        int const, int const) const noexcept;
        void string(char const [], std::size_t const, 
        ::GC const, int const, int const) const noexcept;
        void stipple(::GC const, unsigned const, 
        unsigned const, int const, int const) 
        const noexcept;
        void set_clip(::GC const, int const, int const)
        const noexcept;
        void copy_plane(::GC const, Win const, int const, 
        int const, int const, int const, int const, 
        int const) const noexcept;
        private:
        static Display const dpy;
        ::Window win;
        ::Drawable drawable;
      };

      class Pixmap {
        public:
        Pixmap() = delete;
        Pixmap(Win const, char const [], unsigned const,
        unsigned const) noexcept;
        ~Pixmap();
        
        void copy_plane(::GC const, int const, int const, 
        int const, int const, int const, int const) 
        const noexcept;
        
        private:
        static Display const dpy;
        ::Window const win;
        ::Pixmap pixmap;
      };
    }
      
    class Hints {
      public:
      Hints();
      ~Hints();
      private:
      static Display const dpy;
      ::XWMHints* win_hints;
    };
  }
}
