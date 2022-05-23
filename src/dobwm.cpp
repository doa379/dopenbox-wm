#include <dobwm.h>
#include <X11/Xproto.h>

bool dobwm::X::error { };

void dobwm::Event::button_press(void) {
  const ::XButtonEvent &ev { this->ev.xbutton };
}

void dobwm::Event::button_release(void) {
  const ::XButtonEvent &ev { this->ev.xbutton };
  (void) ev;
}

void dobwm::Event::motion_notify(void) {
  const ::XMotionEvent &ev { this->ev.xmotion };
}

void dobwm::Event::key_press(void) {
  const ::XKeyEvent &ev { this->ev.xkey };
}

void dobwm::Event::key_release(void) {
  const ::XKeyEvent &ev { this->ev.xkey };
  (void) ev;
}
///////////////////////////////////////////////////////////////////////////////
dobwm::X::X(void) {
  if (!(dpy = ::XOpenDisplay(nullptr))) throw "Unable to open display";
  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&X::init_XError);
  ::XSelectInput(dpy, root, ROOTMASK);
  ::XSync(dpy, false);
  if (X::error) {
    ::XCloseDisplay(dpy);
    throw "Initialization XError (another wm running?)";
  }

  ::XSetErrorHandler(&X::XError);
}

dobwm::X::~X(void) {
  ::XCloseDisplay(dpy);
}

int dobwm::X::init_XError(::Display *dpy, ::XErrorEvent *ev) {
  return !(dobwm::X::error = (ev->error_code == BadAccess));
}

int dobwm::X::XError(::Display *dpy, ::XErrorEvent *ev) {
  return !(dobwm::X::error = ((ev->error_code == BadAccess && 
        (ev->request_code == X_GrabKey ||  ev->request_code == X_GrabButton)) ||
      (ev->error_code  == BadMatch && (ev->request_code == X_SetInputFocus ||
        ev->request_code == X_ConfigureWindow)) ||
      (ev->error_code  == BadDrawable && (ev->request_code == X_PolyFillRectangle || 
        ev->request_code == X_CopyArea  || ev->request_code == X_PolySegment ||
          ev->request_code == X_PolyText8)) ||
      ev->error_code == BadWindow));
}
 
void dobwm::X::window(::Window win, const unsigned bw, const unsigned bc) {
  ::XWindowAttributes wa { };
  if (!::XGetWindowAttributes(dpy, win, &wa) || wa.override_redirect) return;
  /*
  const ::Window u {
    ::XCreateSimpleWindow(dpy, root, wa.x, wa.y, wa.width, wa.height, bw, bc, bgc) };
  ::XSelectInput(dpy, u, SubstructureRedirectMask | SubstructureNotifyMask);
  ::XReparentWindow(dpy, v, u, 0, 0);
  ::XMapWindow(dpy, u);
  ::XMapWindow(dpy, v);
  // return pair (u, v)
  */
  ::XSelectInput(dpy, win, SubstructureRedirectMask | SubstructureNotifyMask);
  ::XMapWindow(dpy, win);
}

void dobwm::X::unmap_request(::Window win) const {
  /*
  ::XUnmapWindow(dpy, v);
  ::XReparentWindow(dpy, u, root, 0, 0);
  ::XDestroyWindow(dpy, u);
  ::XDestroyWindow(dpy, v);
  */
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

void dobwm::X::query_tree(const unsigned bw, const unsigned bc) {
  ::XGrabServer(dpy);
  ::Window root { }, parent { };
  ::Window *W { };  // Children
  unsigned NW { };
  if (::XQueryTree(dpy, this->root, &root, &parent, &W, &NW) && root == this->root)
    for (auto i { 0U }; i < NW; i++)
      window(W[i], bw, bc);

  if (W) ::XFree(W);
  ::XUngrabServer(dpy);
}

void dobwm::X::grab_button(void) {

}

void dobwm::X::grab_button(::Window v, const std::vector<int> &B) {
  ::XGrabButton(dpy, B[1], B[0], v, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
}

void dobwm::X::grab_key(void) {

}
