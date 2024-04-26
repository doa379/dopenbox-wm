#pragma once

#include <X11/Xlib.h>
#include <array>
#include <functional>

namespace some {
  struct Display {
    static ::Display* ptr;
    Display();
    Display(const Display&) = delete;
    ~Display();
  };
  
  class Wm;
  class Ev {
    public:
    Ev(Wm&);
    ~Ev();
    bool next(::Display* dpy) { return ::XNextEvent(dpy, &xev) == 0; }
    void sync(::Display* dpy) { ::XSync(dpy, false); }
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
      ExposureMask };
    static constexpr auto PARMASK {
      EnterWindowMask | 
      FocusChangeMask |
      PropertyChangeMask | 
      StructureNotifyMask };
    
    class Xlib {
      public:
      Xlib(::Display* dpy) : dpy { dpy } { };
      ::Window root() { return ::XRootWindow(dpy, DefaultScreen(dpy)); }
      ::XErrorHandler set_err(int (*handler)(::Display*, ::XErrorEvent*)) {
        return ::XSetErrorHandler(handler); }
      private:
      ::Display* dpy;
    };

    class Input {
      public:
      Input(::Display* dpy);
      ~Input();
      void select_input(const ::Window W, const long MASK) {
        ::XSelectInput(dpy, W, MASK); }
      void set_inputfocus(const ::Window W) {
        ::XSetInputFocus(dpy, W, RevertToPointerRoot, CurrentTime); }
      unsigned modmask();
      void grab_key(const ::Window, const int, const int);
      void ungrab_key(const ::Window, const int, const int);
      void ungrab_allkey(const ::Window W) {
        ungrab_key(W, AnyModifier, AnyKey); }
      void grab_btn(const ::Window, const int, const int);
      void ungrab_btn(const ::Window, const int, const int);
      void warp_pointer(const ::Window, const int, const int);
      private:
      ::Display* dpy;
    };

    class QueryTree {
      public:
      QueryTree(::Display*, const ::Window);
      ~QueryTree();
      std::vector<::Window> get();
      private:
      ::Window* w { };
      unsigned n { };
    };

    class Xinerama {
      public:
      Xinerama(::Display* dpy) : dpy { dpy } { }
      private:
      ::Display* dpy;
    };

    class Draw {

    };
  }
}
