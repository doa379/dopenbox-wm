#include <dobwm.h>
#include <X11/Xproto.h>

bool dobwm::X::error { };

dobwm::X::X(void) {
  if (!(dpy = ::XOpenDisplay(nullptr)))
    throw "Unable to open display";

  //Nm
  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&X::XError);
  ::XSelectInput(dpy, root, ROOTMASK);
  ::XSync(dpy, false);
  if (X::error)
    throw "X error (XError)";

  ::XSetErrorHandler(&X::XError);
  //::XSync(dpy, false);
}

dobwm::X::~X(void) {
  ::XCloseDisplay(dpy);
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
    
void dobwm::X::create_notify(void) {
  (void) ev.xcreatewindow;
}

void dobwm::X::destroy_notify(void) {
  (void) ev.xdestroywindow;
}

void dobwm::X::reparent_notify(void) {

}

void dobwm::X::map_notify(void) {

}

void dobwm::X::unmap_notify(void) {

}

void dobwm::X::configure_notify(void) {

}

void dobwm::X::map_request(dobwm::Client &c) {
  const ::XMapRequestEvent &ev { this->ev.xmaprequest };
  ::Window w { ev.window };
  ::XWindowAttributes wa { };
  if (::XGetWindowAttributes(dpy, w, &wa) && wa.override_redirect)
    return;

  c = dobwm::Client { w, "Client", 0, 0, 80, 80, /* ::XGetTransientForHint(dpy, w, &w) */ };
  ::XMapWindow(dpy, w);
}

void dobwm::X::configure_request(void) {

}

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
