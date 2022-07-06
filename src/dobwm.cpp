#include <dobwm.h>
#include <X11/Xproto.h>
#include <X11/XKBlib.h>
#include <stdexcept>

bool dobwm::X::error { };

dobwm::X::X(void) {
  if (!(dpy = ::XOpenDisplay(nullptr)))
    throw std::runtime_error("Unable to open display");
  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&X::init_XError);
  ::XSelectInput(dpy, root, ROOTMASK | BUTTONMASK | NOTIFMASK);
  ::XUngrabKey(dpy, AnyKey, AnyModifier, root);
  ::XSync(dpy, false);
  if (X::error) {
    ::XCloseDisplay(dpy);
    throw std::runtime_error("Initialization XError (another wm running?)");
  }

  ::XSetErrorHandler(&X::XError);
}

dobwm::X::~X(void) {
  ::XCloseDisplay(dpy);
}

int dobwm::X::init_XError(::Display *dpy, ::XErrorEvent *ev) {
  return !(X::error = (ev->error_code == BadAccess));
}

int dobwm::X::XError(::Display *dpy, ::XErrorEvent *ev) {
  return !(X::error = ((ev->error_code == BadAccess && 
        (ev->request_code == X_GrabKey ||  ev->request_code == X_GrabButton)) ||
      (ev->error_code  == BadMatch && (ev->request_code == X_SetInputFocus ||
        ev->request_code == X_ConfigureWindow)) ||
      (ev->error_code  == BadDrawable && (ev->request_code == X_PolyFillRectangle || 
        ev->request_code == X_CopyArea  || ev->request_code == X_PolySegment ||
          ev->request_code == X_PolyText8)) ||
      ev->error_code == BadWindow));
}
 
void dobwm::X::window(::Window win, const unsigned bw, const Palette bc) {
  ::XWindowAttributes wa { };
  if (!::XGetWindowAttributes(dpy, win, &wa) || wa.override_redirect) return;
  ::XSetWindowBorder(dpy, win, static_cast<unsigned long>(bc));
  ::XSetWindowBorderWidth(dpy, win, bw);
  ::XSelectInput(dpy, win, ROOTMASK);
  ::XMapWindow(dpy, win);
}

void dobwm::X::unmap_request(::Window win) const {
  ::XUnmapWindow(dpy, win);
  ::XReparentWindow(dpy, win, root, 0, 0);
  ::XDestroyWindow(dpy, win);
}

void dobwm::X::configure_window(::XConfigureRequestEvent &ev, ::Window win) const {
  ::XWindowChanges wc {
    ev.x,
    ev.y,
    ev.width,
    ev.height,
    ev.border_width,
    ev.above,
    ev.detail
  };
  
  if (::XConfigureWindow(dpy, win, ev.value_mask, &wc)) ::XSync(dpy, false);
}

void dobwm::X::query_tree(const unsigned bw, const Palette bc) {
  ::Window root { }, parent { };
  ::Window *W { };  // Children
  unsigned NW { };
  ::XGrabServer(dpy);
  if (::XQueryTree(dpy, this->root, &root, &parent, &W, &NW) && root == this->root)
    for (auto i { 0U }; i < NW; i++)
      window(W[i], bw, bc);

  ::XUngrabServer(dpy);
  if (W) ::XFree(W);
}

void dobwm::X::grab_button(::Window w, const int MOD, const int B) {
  ::XGrabButton(dpy, B, MOD, w, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
}

void dobwm::X::grab_buttons(void) {

}

void dobwm::X::grab_key(const int MOD, const int K) const {
  const ::KeyCode kc { ::XKeysymToKeycode(dpy, K) };
  ::XGrabKey(dpy, kc, MOD, root, True, GrabModeAsync, GrabModeAsync);
}

::KeySym dobwm::X::key_press(const ::KeyCode kc) {
  return ::XkbKeycodeToKeysym(dpy, kc, 0, 0);
}
