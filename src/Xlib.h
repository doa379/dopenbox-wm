#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xinerama.h>
#include <array>
#include <functional>
#include <optional>
#include <string_view>

#include "lib.h"

namespace some {
  struct Display {
    static void init();
    static void deinit();
    static int connection();
    static ::Display* ptr;
    static int width;
    static int height;
    static int depth;
    static ::Window root;
  };
  
  class Ev {
    public:
    Ev();
    ~Ev() = default;
    void mask_event(long const) noexcept;
    lib::Generator<::XEvent> seq();
    private:
    static Display const dpy;
    ::XEvent xev;
  };

  class Sys {
    public:
    Sys() = default;
    void spawn(char const []) const noexcept;
    private:
    static Display const dpy;
  };
  
  namespace xlib {
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
    
    class KeyMod {
      public:
      KeyMod();
      KeyMod(KeyMod const&) = delete;
      KeyMod& operator=(KeyMod const&) = delete;
      KeyMod(KeyMod&&) noexcept;
      KeyMod& operator=(KeyMod&&) noexcept;
      ~KeyMod();
      unsigned numlock_mask() noexcept;
      private:
      static Display const dpy;
      ::XModifierKeymap* map { };
      unsigned numlock_kcode { };
    };
    
    class KeySym {
      public:
      KeySym();
      ~KeySym();
      std::vector<unsigned> get_ksyms() const noexcept;
      private:
      static Display const dpy;
      ::KeySym* syms { };
      int start { };
      int end { };
      int skip { };
    };

    class WinAttr {
      public:
      WinAttr() = delete;
      WinAttr(::Window const);
      bool override_redirect() const noexcept;
      std::pair<int, int> size() const noexcept;
      std::pair<int, int> pos() const noexcept;
      private:
      static Display const dpy;
      ::XWindowAttributes wa;
    };

    class QueryTree {
      public:
      QueryTree(::Window const);
      ~QueryTree();
      std::vector<::Window> get() const noexcept;
      private:
      static Display const dpy;
      ::Window* wins { };
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
    
    class Font {
      public:
      Font() = default;
      ~Font() = default;
      static void init(char const []);
      static void deinit();
      static int get_scent() noexcept;
      static int get_ascent() noexcept;
      static int get_descent() noexcept;
      static int text_width(char const [], 
      std::size_t const) noexcept;
      static int text_width16(wchar_t const [],
      std::size_t const) noexcept;
      private:
      static Display const dpy;
      static ::XFontStruct* fn;
      static int scent;
    };

    class Gc {
      public:
      Gc() = default;
      Gc(::Window const) noexcept;
      Gc(Gc const&) = delete;
      Gc& operator=(Gc const&) = delete;
      Gc(Gc&&) noexcept;
      Gc& operator=(Gc&&) noexcept;
      ~Gc();
      ::GC get() const noexcept;
      void set_fg(std::size_t const) const noexcept;
      void set_bg(std::size_t const) const noexcept;
      private:
      static Display const dpy;
      ::GC gc { };
    };

    class Draw {
      public:
      Draw() = default;
      Draw(::Window const, int const, int const, int const)
      noexcept;
      Draw(Draw const&) = delete;
      Draw& operator=(Draw const&) = delete;
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
      void copy_plane(::GC const, ::Window const, int const, 
      int const, int const, int const, int const, 
      int const) const noexcept;
      private:
      static Display const dpy;
      ::Window win { };
      ::Drawable drawable { };
    };

    class Pixmap {
      public:
      Pixmap() = delete;
      Pixmap(::Window const, char const [], unsigned const,
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

    class Cursor {
      static auto constexpr PTR_SYM { XC_left_ptr };
      static auto constexpr MOVE_SYM { XC_fleur };
      static auto constexpr RESIZE_SYM { XC_sizing };
      static auto constexpr HDBL_SYM { 
        XC_sb_h_double_arrow };
      static auto constexpr VDBL_SYM { 
        XC_sb_v_double_arrow }; 
      public:
      Cursor() = default;
      ~Cursor() = default;
      static void init() noexcept;
      static void deinit();
      static ::Cursor ptr;
      static ::Cursor move;
      static ::Cursor resize;
      static ::Cursor h;
      static ::Cursor v;
      private:
      static Display const dpy;
    };
    
    class Prop {
      public:
      Prop() = default;
      ~Prop() = default;
      static void init() noexcept;
      static void deinit() noexcept;
      static ::Atom wm_protocols;
      static ::Atom wm_name;
      static ::Atom wm_delete_window;
      static ::Atom wm_state;
      static ::Atom wm_take_focus;
      static ::Atom wm_icon_name;
      static ::Atom net_supported;
      static ::Atom net_wm_state;
      static ::Atom net_wm_name;
      static ::Atom net_wm_window_opacity;
      static ::Atom net_active_window;
      static ::Atom net_wm_state_fullscreen;
      static ::Atom net_wm_window_type;
      static ::Atom net_wm_window_type_dialog;
      static ::Atom net_client_list;
      static ::Atom net_number_of_desktops;
      static ::Atom net_wm_desktop;
      static ::Atom net_current_desktop;
      static ::Atom net_showing_desktop;
      static ::Atom net_wm_icon;
      static ::Atom net_wm_icon_name;
      static void change_state(::Window const) noexcept;
      static std::optional<std::string_view> get_name(::Window const) 
      noexcept;
      static std::optional<std::string_view> get_icon(::Window const) 
      noexcept;
      private:
      static Display const dpy;
      static auto constexpr DATASIZE { 64 };
      static unsigned char* data;
      static ::Atom actual_type;
      static int actual_format;
      static unsigned long nitems;
      static unsigned long bytes_after;
    };

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
