#include <dobwm.h>
#include <xkb.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <stdexcept>

bool dobwm::X::error { };

dobwm::X::X(void) {
  if (!dpy)
    throw std::runtime_error("Unable to open display");

  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&X::XError);
  ::XSelectInput(dpy, root, ROOTMASK | BUTTONMASK | NOTIFMASK);
  ::XSync(dpy, false);
  if (error) {
    ::XCloseDisplay(dpy);
    throw std::runtime_error("Initialization error (another wm running?)");
  }

  ::XUngrabKey(dpy, AnyKey, AnyModifier, root);
  // Modifier Mask
  ::XModifierKeymap *modmap { ::XGetModifierMapping(dpy) };
  unsigned numlockmask { };
  for (auto k { 0U }; k < 8; k++)
    for (auto j { 0 }; j < modmap->max_keypermod; j++)
      if (modmap->modifiermap[modmap->max_keypermod * k + j] ==
          ::XKeysymToKeycode(dpy, _Numlock_))
        numlockmask = (1 << k);
  ::XFreeModifiermap(modmap);
  modmask = ~(numlockmask | LockMask);
  // Atoms
  WM[static_cast<unsigned>(Wm::PROTOCOLS)] =
    ::XInternAtom(dpy, "WM_PROTOCOLS", false);
  WM[static_cast<unsigned>(Wm::DELWIN)] =
    ::XInternAtom(dpy, "WM_DELETE_WINDOW", false);
  NET[static_cast<unsigned>(Net::SUPPORTED)] =
    ::XInternAtom(dpy, "_NET_SUPPORTED", false);
  NET[static_cast<unsigned>(Net::STATE)] =
    ::XInternAtom(dpy, "_NET_WM_STATE", false);
  NET[static_cast<unsigned>(Net::ACTIVE)] =
    ::XInternAtom(dpy, "_NET_ACTIVE_WINDOW", false);
  NET[static_cast<unsigned>(Net::FULLSCRN)] =
    ::XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", false);
  ::XChangeProperty(dpy, root, 
    NET[static_cast<unsigned>(Net::SUPPORTED)], XA_ATOM, 32,
      PropModeReplace, reinterpret_cast<unsigned char *>(NET.data()), NNET);

  ::XSync(dpy, false);
}

dobwm::X::~X(void) {
  ::XCloseDisplay(dpy);
}

int dobwm::X::XError(::Display *dpy, ::XErrorEvent *ev) {
  error = ev->error_code == BadAccess;
  return 0;
}

void dobwm::X::client(::Window win, const unsigned bw, const Palette bc) {
  ::XWindowAttributes wa { };
  if (!::XGetWindowAttributes(dpy, win, &wa) || wa.override_redirect)
    return;
  ::XSetWindowBorder(dpy, win, static_cast<unsigned long>(bc));
  ::XSetWindowBorderWidth(dpy, win, bw);
  //::XSelectInput(dpy, win, ROOTMASK | BUTTONMASK | NOTIFMASK);
  ::XSelectInput(dpy, win, PropertyChangeMask | FocusChangeMask | EnterWindowMask);
  ::XSetInputFocus(dpy, win, RevertToPointerRoot, CurrentTime);
  ::XChangeProperty(dpy, root, NET[static_cast<unsigned>(Net::ACTIVE)], XA_WINDOW, 32, PropModeReplace, reinterpret_cast<unsigned char *>(&win), 1);
  ::XMapWindow(dpy, win);
}

void dobwm::X::unmap_request(const ::Window win) const {
  ::XUnmapWindow(dpy, win);
  ::XReparentWindow(dpy, win, root, 0, 0);
  ::XDestroyWindow(dpy, win);
  ::XSync(dpy, false);
}

void dobwm::X::configure_window(::XConfigureRequestEvent &ev, const ::Window win) const {
  ::XWindowChanges wc {
    ev.x,
    ev.y,
    ev.width,
    ev.height,
    ev.border_width,
    ev.above,
    ev.detail
  };
  
  if (::XConfigureWindow(dpy, win, ev.value_mask, &wc))
    ::XSync(dpy, false);
}

void dobwm::X::query_tree(const unsigned bw, const Palette bc) {
  ::Window root { }, parent { };
  ::Window *W { };  // Children
  unsigned NW { };
  ::XGrabServer(dpy);
  if (::XQueryTree(dpy, this->root, &root, &parent, &W, &NW) && 
      root == this->root)
    for (auto i { 0U }; i < NW; i++)
      client(W[i], bw, bc);

  ::XUngrabServer(dpy);
  if (W)
    ::XFree(W);
}

void dobwm::X::grab_button(::Window w, const int MOD, const int B) {
  ::XGrabButton(dpy, B, MOD & modmask, w, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
  //::XGrabButton(dpy, B, MOD | modmask, w, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
}

void dobwm::X::grab_buttons(void) {

}

void dobwm::X::grab_key(const int MOD, const int K) const {
  const ::KeyCode KC { ::XKeysymToKeycode(dpy, K) };
  ::XGrabKey(dpy, KC, MOD & modmask, root, True, GrabModeAsync, GrabModeAsync);
  //::XGrabKey(dpy, KC, MOD | modmask, root, True, GrabModeAsync, GrabModeAsync);
}

::KeySym dobwm::X::key_press(const ::KeyCode KC) {
  return ::XkbKeycodeToKeysym(dpy, KC, 0, 0);
}
