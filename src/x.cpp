#include <dobwm.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xinerama.h>
#include <stdexcept>

bool dobwm::X::error { };

dobwm::X::X(void) {
  if (dpy == nullptr)
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
  ::XUngrabButton(dpy, AnyButton, AnyModifier, root);
  // Modifier Mask
  ::XModifierKeymap *modmap { ::XGetModifierMapping(dpy) };
  unsigned numlockmask { };
  for (int k { }; k < 8; k++)
    for (int j { }; j < modmap->max_keypermod; j++)
      if (modmap->modifiermap[modmap->max_keypermod * k + j] ==
          ::XKeysymToKeycode(dpy, XK_Num_Lock))
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
        static_cast<int>(Net::CNT));
  ::XSync(dpy, false);
}

dobwm::X::~X(void) {
  //::XUngrabKey(dpy, AnyKey, AnyModifier, root);
  ::XCloseDisplay(dpy);
}

dobwm::X::Xinerama::Xinerama(::Display *dpy) {
  int Nm { };
  if (auto *INF { ::XineramaQueryScreens(dpy, &Nm) }; INF) {
    for (auto i { 0 }; i < Nm; i++)
      D.emplace_back(Dim {
        INF[i].x_org, INF[i].y_org, INF[i].width, INF[i].height });

    ::XFree(INF);
  } else
    throw std::runtime_error("Monitor initialization error");
}

dobwm::X::Xinerama::~Xinerama(void) {

}

auto dobwm::X::XError(::Display *dpy, ::XErrorEvent *ev) -> int {
  error = ev->error_code == BadAccess;
  return 0;
}

auto dobwm::X::MONS(void) const -> std::vector<Dim> {
  Xinerama xinerama { dpy };
  return xinerama.M();
}

auto dobwm::X::client(const ::Window W, const unsigned BW, const unsigned long BC) const -> void {
  ::XSetWindowBorder(dpy, W, BC);
  ::XSetWindowBorderWidth(dpy, W, BW);
}

auto dobwm::X::focus(::Window w) const -> void {
	//::XDeleteProperty(dpy, root, NET[static_cast<int>(Net::ACT)]);
  ::XChangeProperty(dpy, root,
    NET[static_cast<int>(Net::ACT)], XA_WINDOW, 32,
      PropModeReplace, reinterpret_cast<unsigned char *>(&w), 1);
  ::XSelectInput(dpy, w, PropertyChangeMask | FocusChangeMask | EnterWindowMask);
  ::XSetInputFocus(dpy, w, RevertToPointerRoot, CurrentTime);
  ::XRaiseWindow(dpy, w);
  ::XSync(dpy, false);
}

auto dobwm::X::isactive(const ::Window W) const -> bool {
// Maybe use post refactoring Xinerama
  return false;
}

auto dobwm::X::map_window(const ::Window W) const -> void {
  ::XMapWindow(dpy, W);
  ::XSync(dpy, false);
}

auto dobwm::X::unmap_window(const ::Window W) const -> void {
  ::XUnmapWindow(dpy, W);
  //::XReparentWindow(dpy, W, root, 0, 0);
  //::XDestroyWindow(dpy, W);
  ::XSync(dpy, false);
}

auto dobwm::X::configure_window(::XConfigureRequestEvent &ev) const -> void {
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

auto dobwm::X::query_tree(void) -> std::vector<::Window> {
  std::vector<::Window> C;
  ::XGrabServer(dpy);
  {
    ::Window root { }, parent { };
    ::Window *W { };  // Children
    unsigned NW { };
    if (::XQueryTree(dpy, this->root, &root, &parent, &W, &NW))
      C = std::vector<::Window> { W, W + NW };
    if (W)
      ::XFree(W);
  }

  ::XUngrabServer(dpy);
  return C;
}

auto dobwm::X::client_attr(const ::Window W) -> std::optional<Dim> {
  ::XWindowAttributes wa { };
  return ::XGetWindowAttributes(dpy, W, &wa) && 
    wa.override_redirect == 0 &&
      wa.map_state == IsViewable ? 
       std::make_optional<Dim>(Dim { wa.x, wa.y, wa.width, wa.height }) : 
        std::nullopt;
}

auto dobwm::X::client_trans(const ::Window W) -> bool {
  ::Window tra;
  return ::XGetTransientForHint(dpy, W, &tra);
}

auto dobwm::X::client_hint(const ::Window W) -> std::optional<std::string> {
  return std::nullopt;
}

auto dobwm::X::grab_key(const unsigned MOD, const ::KeySym K) const -> void {
  const ::KeyCode KC { ::XKeysymToKeycode(dpy, K) };
  ::XGrabKey(dpy, KC, MOD & modmask, root, true, GrabModeAsync, GrabModeAsync);
}

auto dobwm::X::grab_button(const unsigned MOD, const unsigned B) -> void {
  ::XGrabButton(dpy, B, MOD & modmask, root, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
}

auto dobwm::X::grab_button(const ::Window W, const unsigned MOD, const unsigned B) -> void {
  ::XGrabButton(dpy, B, MOD & modmask, W, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
}

auto dobwm::X::ungrab_button(const ::Window W, const unsigned MOD, const unsigned B) -> void {
  ::XUngrabButton(dpy, B, MOD & modmask, W);
}

auto dobwm::X::key_press(const ::KeyCode KC) -> ::KeySym {
  return ::XkbKeycodeToKeysym(dpy, KC, 0, 0);
}

auto dobwm::X::kill_msg(const ::Window W) const {
  ::XEvent ev { };
  ev.type = static_cast<int>(XEvent::CliMsg);
  ev.xclient.window = W;
  ev.xclient.format = 32;
  ev.xclient.message_type = WM[static_cast<int>(Wm::PROTO)];
  ev.xclient.data.l[0] = WM[static_cast<int>(Wm::DELWIN)];
  ev.xclient.data.l[1] = CurrentTime;
  ::XSendEvent(dpy, W, false, NoEventMask, &ev);
}

auto dobwm::X::kill_msg(void) const {
  if (ev.xclient.message_type == WM[static_cast<int>(Wm::PROTO)] && 
    ev.xclient.data.l[0] == static_cast<long>(WM[static_cast<int>(Wm::DELWIN)]))
      kill_client(ev.xclient.window);
}

auto dobwm::X::kill_client(const ::Window W) const -> bool {
  return ::XKillClient(dpy, W) != BadValue;
}

auto dobwm::X::move(const ::Window W, const int X, const int Y) const -> bool {
  return ::XMoveWindow(dpy, W, X, Y) != (BadMatch | BadValue);
}
