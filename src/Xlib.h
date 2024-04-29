#pragma once

#include <X11/Xlib.h>
#include <array>
#include <functional>

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
  
  template<typename T>
  class Ev {
    using S = std::function<void(T&)>;
    public:
    Ev();
    ~Ev() = default;
    bool next() noexcept;
    void sync() const noexcept;
    S call(T&) const noexcept;
    void init_mapnotify(const S&) noexcept;
    S mapnotify(const S&, T&) noexcept;
    void init_unmapnotify(const S&) noexcept;
    S unmapnotify(const S&, T&) noexcept;
    void init_clientmessage(const S&) noexcept;
    S clientmessage(const S&, T&) noexcept;
    void init_configurenotify(const S&) noexcept;
    S configurenotify(const S&, T&) noexcept;
    void init_maprequest(const S&) noexcept;
    S maprequest(const S&, T&) noexcept;
    void init_configurerequest(const S&) noexcept;
    S configurerequest(const S&, T&) noexcept;
    void init_motionnotify(const S&) noexcept;
    S motionnotify(const S&, T&) noexcept;
    void init_keypress(const S&) noexcept;
    S keypress(const S&, T&) noexcept;
    void init_btnpress(const S&) noexcept;
    S btnpress(const S&, T&) noexcept;
    void init_enternotify(const S&) noexcept;
    S enternotify(const S&, T&) noexcept;
    void init_propertynotify(const S&) noexcept;
    S propertynotify(const S&, T&) noexcept;
    void init_expose(const S&) noexcept;
    S expose(const S&, T&) noexcept;
    private:
    Display dpy;
    ::XEvent xev;
    std::array<std::function<S(T&)>, LASTEvent> F;
  };
  
  namespace Xlib {
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
      EnterWindowMask | 
      FocusChangeMask |
      PropertyChangeMask | 
      StructureNotifyMask
    };
    
    class Xlib {
      public:
      ::Window root();
      ::XErrorHandler set_err(int (*)(::Display*, ::XErrorEvent*));
      void mapwindow(const ::Window);
      void unmapwindow(const ::Window);
      void set_bdrcolor(const ::Window, const std::size_t);
      void set_bdrwidth(const ::Window, const std::size_t);
      void movewindow(const ::Window, const int, const int);
      private:
      Display dpy;
    };

    class Input {
      public:
      void select(const ::Window, const long);
      void set_focus(const ::Window);
      unsigned modmask();
      void grab_key(const ::Window, const int, const int);
      void ungrab_key(const ::Window, const int, const int);
      void ungrab_allkey(const ::Window);
      void grab_btn(const ::Window, const int, const int);
      void ungrab_btn(const ::Window, const int, const int);
      void ungrab_pointer();
      void warp_pointer(const ::Window, const int, const int);
      private:
      Display dpy;
    };

    class QueryTree {
      public:
      QueryTree(const ::Window);
      ~QueryTree();
      std::vector<::Window> get();
      private:
      Display dpy;
      ::Window* w { };
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
