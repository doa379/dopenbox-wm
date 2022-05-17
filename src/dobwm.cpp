#include <dobwm.h>
#include <X11/Xproto.h>

bool dobwm::X::error { };

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
 
void dobwm::X::create_notify(void) const {
  (void) ev.xcreatewindow;
}

void dobwm::X::destroy_notify(void) const {
  (void) ev.xdestroywindow;
}

void dobwm::X::reparent_notify(void) const {
  (void) ev.xreparent;
}

void dobwm::X::map_notify(void) const {
  (void) ev.xmap;
}

::Window dobwm::X::unmap_notify(void) const {
  return ev.xunmap.window;
}

void dobwm::X::configure_notify(void) const {
  (void) ev.xconfigure;
}

::Window dobwm::X::map_request(void) const {
  return ev.xmaprequest.window;
}

void dobwm::X::window(::Window v, const unsigned bw, const unsigned bc, const unsigned bgc) {
  ::XWindowAttributes wa { };
  if (!::XGetWindowAttributes(dpy, v, &wa) || wa.override_redirect) return;
  const ::Window u {
    ::XCreateSimpleWindow(dpy, root, wa.x, wa.y, wa.width, wa.height, bw, bc, bgc) };
  ::XSelectInput(dpy, u, SubstructureRedirectMask | SubstructureNotifyMask);
  //::XAddToSaveSet(dpy, w);
  ::XReparentWindow(dpy, v, u, 0, 0);
  ::XMapWindow(dpy, u);
  ::XMapWindow(dpy, v);
  // return pair (u, v)
}

void dobwm::X::unmap_request(::Window u, ::Window v) const {
  ::XUnmapWindow(dpy, v);
  ::XReparentWindow(dpy, u, root, 0, 0);
  //::XRemoveFromSaveSet(dpy, w);
  ::XDestroyWindow(dpy, u);
  ::XDestroyWindow(dpy, v);
}

::XConfigureRequestEvent &dobwm::X::configure_request(void) {
  return ev.xconfigurerequest;
}

void dobwm::X::configure_window(::XConfigureRequestEvent &ev, ::Window u) const {
  ::XWindowChanges wc {
    ev.x,
    ev.y,
    ev.width,
    ev.height,
    ev.border_width,
    ev.above,
    ev.detail
  };
  
  if (::XConfigureWindow(dpy, u, ev.value_mask, &wc)) ::XSync(dpy, false);
}

void dobwm::X::query_tree(const unsigned bw, const unsigned bc, const unsigned bgc) {
  ::XGrabServer(dpy);
  ::Window root { }, parent { };
  ::Window *W { };  // Children
  unsigned NW { };
  if (::XQueryTree(dpy, this->root, &root, &parent, &W, &NW) && root == this->root)
    for (auto i { 0U }; i < NW; i++)
      window(W[i], bw, bc, bgc);

  if (W) ::XFree(W);
  ::XUngrabServer(dpy);
  // return collection of (u, v) pairs
}

void dobwm::X::button_press(void) {
  const ::XButtonEvent &ev { this->ev.xbutton };
}

void dobwm::X::button_release(void) {
  const ::XButtonEvent &ev { this->ev.xbutton };
  (void) ev;
}

void dobwm::X::grab_button(void) {

}

void dobwm::X::grab_button(::Window v, const std::vector<int> &B) {
  ::XGrabButton(dpy, B[1], B[0], v, false, BUTTONMASK, GrabModeAsync, GrabModeAsync, None, None);
}

void dobwm::X::motion_notify(void) {
  const ::XMotionEvent &ev { this->ev.xmotion };
}

void dobwm::X::key_press(void) {
  const ::XKeyEvent &ev { this->ev.xkey };
}

void dobwm::X::key_release(void) {
  const ::XKeyEvent &ev { this->ev.xkey };
  (void) ev;
}

void dobwm::X::grab_key(void) {

}
