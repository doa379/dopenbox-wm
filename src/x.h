#pragma once

#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdexcept>

namespace dobwm {
  namespace {
    static bool xerror;
    static int XError(::Display*, ::XErrorEvent* xev) {
      xerror = xev->error_code == BadAccess;
      return 0;
    }
  };

  struct Root {  // Base class
    static inline ::Display* dpy { ::XOpenDisplay(nullptr) };
    static inline int scrn;
    static inline ::Window w;
    static inline std::pair<std::size_t, std::size_t> size;
    static inline int modmask;  // This mask needs repeated updates
    static constexpr auto ROOTMASK {
      SubstructureRedirectMask | 
      SubstructureNotifyMask | 
      ButtonPressMask |
      PointerMotionMask |
      EnterWindowMask |
      LeaveWindowMask |
      StructureNotifyMask |
      PropertyChangeMask
    };
    
    struct {
      const std::size_t PROTO { 
          ::XInternAtom(dpy, "WM_PROTOCOLS", false) },
        NAME { ::XInternAtom(dpy, "WM_NAME", false) },
        DELETE_WINDOW { ::XInternAtom(dpy, "WM_DELETE_WINDOW", false) },
        STATE { ::XInternAtom(dpy, "WM_STATE", false) },
        TAKE_FOCUS { ::XInternAtom(dpy, "WM_TAKE_FOCUS", false) },
        SUPPORTED { ::XInternAtom(dpy, "_NET_SUPPORTED", false) },
        WM_STATE { ::XInternAtom(dpy, "_NET_WM_STATE", false) },
        WM_NAME { ::XInternAtom(dpy, "_NET_WM_NAME", false) },
        ACTIVE_WINDOW { ::XInternAtom(dpy, "_NET_ACTIVE_WINDOW", false) },
        WM_STATE_FULLSCREEN { 
          ::XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", false) },
        WM_WINDOW_TYPE { ::XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", false) },
        WM_WINDOW_TYPE_DIALOG { 
          ::XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", false) },
        CLIENT_LIST { ::XInternAtom(dpy, "_NET_CLIENT_LIST", false) },
        NUMBER_OF_DESKTOPS {
          ::XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", false) },
        WM_DESKTOP { ::XInternAtom(dpy, "_NET_WM_DESKTOP", false) },
        CURRENT_DESKTOP { ::XInternAtom(dpy, "_NET_CURRENT_DESKTOP", false) },
        SHOWING_DESKTOP { ::XInternAtom(dpy, "_NET_SHOWING_DESKTOP", false) };
    } atom;

    Root() {
      if (!dpy)
        throw std::runtime_error("Unable to open display");
      
      scrn = DefaultScreen(dpy);
      w = RootWindow(dpy, scrn);
      ::XSetErrorHandler(XError);
      ::XSelectInput(dpy, w, ROOTMASK);
      if (xerror) {
        ::XCloseDisplay(dpy);
        throw std::runtime_error("Initialization error (another wm running?)");
      }
      
      size = std::pair<std::size_t, std::size_t> { 
        DisplayWidth(dpy, scrn), DisplayHeight(dpy, scrn) };
      ::XUngrabKey(dpy, AnyKey, AnyModifier, w);
      // Modifier Mask
      ::XModifierKeymap* modmap { ::XGetModifierMapping(dpy) };
      unsigned numlockmask { };
      for (int k { }; k < 8; k++)
        for (int j { }; j < modmap->max_keypermod; j++)
          if (modmap->modifiermap[modmap->max_keypermod * k + j] ==
              ::XKeysymToKeycode(dpy, XK_Num_Lock))
            numlockmask = (1 << k);
      
      ::XFreeModifiermap(modmap);
      modmask = ~(numlockmask | LockMask);
    }

    ~Root() {
      ::XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
      ::XCloseDisplay(dpy);
    }

    void grab_key(const std::size_t MOD, const std::size_t KEY) {
      ::XGrabKey(dpy, ::XKeysymToKeycode(dpy, KEY), MOD & modmask, w, 
        true, GrabModeAsync, GrabModeAsync);
    }
    
    void ungrab_key(const std::size_t MOD, const std::size_t KEY) {
      ::XUngrabKey(dpy, ::XKeysymToKeycode(dpy, KEY), MOD & modmask, w);
    }
  };
}
