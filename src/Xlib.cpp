#include <Xlib.h>
#include <X11/Xproto.h>

bool Xlib::Box::error { };

Xlib::Box::Box(const unsigned char Nm, const unsigned char Nt) {
  if (!(dpy = ::XOpenDisplay(nullptr)))
    throw "Unable to open display";

  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&Box::XError);
  ::XSelectInput(dpy, root, ROOTMASK);
  ::XSync(dpy, false);
  if (Box::error)
    throw "Box error (XError)";

  ::XSetErrorHandler(&Box::XError);
  //::XSync(dpy, false);
  for (auto i { 0U }; i < Nm; i++) {
    std::list<Tag> T(Nt);
    Mon m { T };
    M.emplace_back(std::move(m));
  }
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

void Xlib::Box::map_request(void) {

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
