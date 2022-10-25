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
  int numlockmask { };
  for (int k { }; k < 8; k++)
    for (int j { }; j < modmap->max_keypermod; j++)
      if (modmap->modifiermap[modmap->max_keypermod * k + j] ==
          ::XKeysymToKeycode(dpy, static_cast<int>(Key::Numlock)))
        numlockmask = (1 << k);
  
  ::XFreeModifiermap(modmap);
  modmask = ~(numlockmask | LockMask);
  // Atoms
  WM[static_cast<int>(Wm::PROTO)] =
    ::XInternAtom(dpy, "WM_PROTOCOLS", false);
  WM[static_cast<int>(Wm::DELWIN)] =
    ::XInternAtom(dpy, "WM_DELETE_WINDOW", false);
  NET[static_cast<int>(Net::SUPP)] =
    ::XInternAtom(dpy, "_NET_SUPPORTED", false);
  NET[static_cast<int>(Net::STATE)] =
    ::XInternAtom(dpy, "_NET_WM_STATE", false);
  NET[static_cast<int>(Net::ACT)] =
    ::XInternAtom(dpy, "_NET_ACTIVE_WINDOW", false);
  NET[static_cast<int>(Net::FSCRN)] =
    ::XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", false);
  ::XChangeProperty(dpy, root, NET[static_cast<int>(Net::SUPP)],
    XA_ATOM, 32, PropModeReplace,
      reinterpret_cast<unsigned char *>(NET.data()),
        static_cast<int>(dobwm::Net::CNT));
  ::XSync(dpy, false);
}

dobwm::X::~X(void) {
  ::XCloseDisplay(dpy);
}

int dobwm::X::XError(::Display *dpy, ::XErrorEvent *ev) {
  error = ev->error_code == BadAccess;
  return 0;
}

void dobwm::X::client(::Window w, const int BW, const Palette BC) const {
  ::XSetWindowBorder(dpy, w, static_cast<unsigned long>(BC));
  ::XSetWindowBorderWidth(dpy, w, BW);
}

void dobwm::X::focus(::Window w) const {
	//::XDeleteProperty(dpy, root, NET[static_cast<int>(Net::ACT)]);
  ::XChangeProperty(dpy, root,
    NET[static_cast<int>(Net::ACT)], XA_WINDOW, 32,
      PropModeReplace, reinterpret_cast<unsigned char *>(&w), 1);
  ::XSelectInput(dpy, w, PropertyChangeMask | FocusChangeMask | EnterWindowMask);
  ::XSetInputFocus(dpy, w, RevertToPointerRoot, CurrentTime);
  ::XRaiseWindow(dpy, w);
}

bool dobwm::X::isactive(const ::Window w) const {
// Maybe use post refactoring Xinerama
  return false;
}

void dobwm::X::map_window(const ::Window W) const {
  ::XMapWindow(dpy, W);
  ::XSync(dpy, false);
}

void dobwm::X::unmap_window(const ::Window W) const {
  ::XUnmapWindow(dpy, W);
  //::XReparentWindow(dpy, W, root, 0, 0);
  //::XDestroyWindow(dpy, W);
  ::XSync(dpy, false);
}

void dobwm::X::configure_window(::XConfigureRequestEvent &ev) const {
  ::XWindowChanges wc {
    ev.x,
    ev.y,
    ev.width,
    ev.height,
    ev.border_width,
    ev.above,
    ev.detail
  };

  if (::XConfigureWindow(dpy, ev.window, ev.value_mask, &wc))
    ::XSync(dpy, false);
}

std::vector<::Window> dobwm::X::query_tree(void) {
  ::Window root { }, parent { };
  ::Window *W { };  // Children
  unsigned NW { };
  std::vector<::Window> C;
  ::XGrabServer(dpy);
  if (::XQueryTree(dpy, this->root, &root, &parent, &W, &NW))
    for (auto i { 0U }; i < NW; i++) {
      ::XWindowAttributes wa { };
      if (::XGetWindowAttributes(dpy, W[i], &wa) &&
          !wa.override_redirect && 
            /*!XGetTransientForHint(dpy, W[i], &root) &&*/
              wa.map_state == IsViewable)
        C.emplace_back(W[i]);
    }

  ::XUngrabServer(dpy);
  if (W)
    ::XFree(W);
  
  return C;
}

void dobwm::X::grab_button(const ::Window W, const int MOD, const int B) {
  ::XGrabButton(dpy, B, MOD & modmask, W, false, 
      BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
  //::XGrabButton(dpy, B, MOD | modmask, w, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
}

void dobwm::X::grab_buttons(void) {

}

void dobwm::X::grab_key(const int MOD, const int K) const {
  const ::KeyCode KC { ::XKeysymToKeycode(dpy, K) };
  ::XGrabKey(dpy, KC, MOD & modmask, root, True,
      GrabModeAsync, GrabModeAsync);
  //::XGrabKey(dpy, KC, MOD | modmask, root, True, GrabModeAsync, GrabModeAsync);
}

::KeySym dobwm::X::key_press(const ::KeyCode KC) {
  return ::XkbKeycodeToKeysym(dpy, KC, 0, 0);
}

void dobwm::X::kill_msg(const ::Window W) const {
  ::XEvent ev { };
  ev.type = static_cast<int>(XEvent::CliMsg);
  ev.xclient.window = W;
  ev.xclient.format = 32;
  ev.xclient.message_type = WM[static_cast<int>(Wm::PROTO)];
  ev.xclient.data.l[0] = WM[static_cast<int>(Wm::DELWIN)];
  ev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(dpy, W, false, NoEventMask, &ev);
}

void dobwm::X::kill_msg(void) const {
  if (ev.xclient.message_type == WM[static_cast<int>(Wm::PROTO)] && 
    ev.xclient.data.l[0] == static_cast<long>(WM[static_cast<int>(Wm::DELWIN)]))
      kill_client(ev.xclient.window);
}

void dobwm::X::kill_client(const ::Window W) const {
  ::XKillClient(dpy, W);
}
