#pragma once

#include <X11/Xlib.h>
#include <array>
#include <functional>

namespace some {
  struct Display {
    Display() = default;
    Display(const Display&) = delete;
    Display(Display&&) = delete;
    ~Display() = default;
    static ::Display* ptr;
    static void init();
    static void deinit();
  };
  
  class Wm;
  class Ev {
    public:
    Ev(Wm&);
    ~Ev();
    bool next();
    void sync();
    void call() { F[xev.type](); }
    void mapnotify(Wm&) noexcept;
    void unmapnotify(Wm&) noexcept;
    void clientmessage(Wm&) noexcept;
    void configurenotify(Wm&) noexcept;
    void maprequest(Wm&) noexcept;
    void configurerequest(Wm&) noexcept;
    void motionnotify(Wm&) noexcept;
    void keypress(Wm&) noexcept;
    void btnpress(Wm&) noexcept;
    void enternotify(Wm&) noexcept;
    void propertynotify(Wm&) noexcept;
    void expose(Wm&) noexcept;
    private:
    Display dpy;
    ::XEvent xev;
    std::array<std::function<void()>, LASTEvent> F;
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
