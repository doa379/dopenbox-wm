#include <Xlib.h>
#include <X11/Xproto.h>

bool Xlib::Box::error { };

Xlib::Box::Box(void) {
  if (!(dpy = ::XOpenDisplay(nullptr)))
    throw "Unable to open display";

  //Nm
  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&Box::XError);
  ::XSelectInput(dpy, root, ROOTMASK);
  ::XSync(dpy, false);
  if (Box::error)
    throw "Box error (XError)";

  ::XSetErrorHandler(&Box::XError);
  //::XSync(dpy, false);
}

Xlib::Box::~Box(void) {
  ::XCloseDisplay(dpy);
}

int Xlib::Box::XError(::Display *dpy, ::XErrorEvent *ev) {
  return !(Xlib::Box::error = ((ev->error_code == BadAccess && 
        (ev->request_code == X_GrabKey ||  ev->request_code == X_GrabButton)) ||
      (ev->error_code  == BadMatch && (ev->request_code == X_SetInputFocus ||
        ev->request_code == X_ConfigureWindow)) ||
      (ev->error_code  == BadDrawable && (ev->request_code == X_PolyFillRectangle || 
        ev->request_code == X_CopyArea  || ev->request_code == X_PolySegment ||
          ev->request_code == X_PolyText8)) ||
      ev->error_code == BadWindow));
}
    
void Xlib::Box::create_notify(void) {
  (void) ev.xcreatewindow;
}

void Xlib::Box::destroy_notify(void) {
  (void) ev.xdestroywindow;
}

void Xlib::Box::reparent_notify(void) {

}

void Xlib::Box::map_notify(void) {

}

void Xlib::Box::unmap_notify(void) {

}

void Xlib::Box::configure_notify(void) {

}

void Xlib::Box::map_request(dobwm::Client &c) {
  const ::XMapRequestEvent &ev { this->ev.xmaprequest };
  ::Window w { ev.window };
  ::XWindowAttributes wa { };
  if (::XGetWindowAttributes(dpy, w, &wa) && wa.override_redirect)
    return;

  c = dobwm::Client { w, "Client", 0, 0, 80, 80, /* ::XGetTransientForHint(dpy, w, &w) */ };
  ::XMapWindow(dpy, w);
}

void Xlib::Box::configure_request(void) {

}

void Xlib::Box::button_press(void) {

}

void Xlib::Box::button_release(void) {

}

void Xlib::Box::motion_notify(void) {

}

void Xlib::Box::key_press(void) {

}

void Xlib::Box::key_release(void) {

}
