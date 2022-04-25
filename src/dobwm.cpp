#include <dobwm.h>
#include <X11/Xproto.h>

bool dobwm::X::error { };

dobwm::X::X(void) {
  if (!(dpy = ::XOpenDisplay(nullptr))) throw "Unable to open display";
  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&X::init_XError);
  ::XSelectInput(dpy, root, ROOTMASK);
  ::XSync(dpy, false);
  if (X::error) throw "Initialization XError (another wm running?)";
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

void dobwm::X::unmap_notify(void) const {
  unmanage(ev.xunmap.window);
}

void dobwm::X::configure_notify(void) const {
  (void) ev.xconfigure;
}

dobwm::WAttr dobwm::X::map_request(::Window &w) const {
  const ::XMapRequestEvent &ev { this->ev.xmaprequest };
  w = ::Window { ev.window };
  WAttr wa { };
  if (!::XGetWindowAttributes(dpy, w, &wa) || wa.override_redirect) return { };
  ::XMapWindow(dpy, w);
  return wa;
}

::Window dobwm::X::manage(::Window w, WAttr &wa, const unsigned bw, const unsigned bc, const unsigned bgc) const {
  const ::Window u {
    ::XCreateSimpleWindow(dpy, root, wa.x, wa.y, wa.width, wa.height, bw, bc, bgc) };
  ::XSelectInput(dpy, u, ROOTMASK);
  ::XAddToSaveSet(dpy, w);
  ::XReparentWindow(dpy, w, u, 0, 0);
  ::XMapWindow(dpy, u);
  ::XSync(dpy, false);
  return u;
}

void dobwm::X::unmanage(::Window w) const {
  ::XUnmapWindow(dpy, w);
  ::Window u { };
  ::XReparentWindow(dpy, u, root, 0, 0);
  ::XRemoveFromSaveSet(dpy, w);
  ::XDestroyWindow(dpy, u);
}

::XConfigureRequestEvent &dobwm::X::configure_request(void) {
  return this->ev.xconfigurerequest;
}

void dobwm::X::configure_window(::XConfigureRequestEvent &ev, const ::Window w) const {
  ::XWindowChanges wc {
    ev.x, 
    ev.y, 
    ev.width, 
    ev.height, 
    ev.border_width, 
    ev.above, 
    ev.detail 
  };
  
  if (::XConfigureWindow(dpy, w, ev.value_mask, &wc)) ::XSync(dpy, false);
}
/*
void dobwm::X::configure_request(void) const {
  const ::XConfigureRequestEvent &ev { this->ev.xconfigurerequest };
  ::XWindowChanges wc {
    ev.x, 
    ev.y, 
    ev.width, 
    ev.height, 
    ev.border_width, 
    ev.above, 
    ev.detail 
  };
  
  if (::XConfigureWindow(dpy, ev.window, ev.value_mask, &wc)) ::XSync(dpy, false);
}

void dobwm::X::configure_request(::Window &u) const {
  const ::XConfigureRequestEvent &ev { this->ev.xconfigurerequest };
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
*/
void dobwm::X::button_press(void) {

}

void dobwm::X::button_release(void) {

}

void dobwm::X::motion_notify(void) {

}

void dobwm::X::key_press(void) {

}

void dobwm::X::key_release(void) {

}
