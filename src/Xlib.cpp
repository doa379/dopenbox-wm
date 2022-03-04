#include <Xlib.h>
#include <X11/Xproto.h>

bool Xlib::Box::error { };

Xlib::Box::Box(const unsigned char Nm, const unsigned char Nt) {
  if (!(dpy = ::XOpenDisplay(nullptr)))
    throw "Unable to open display";

  root = RootWindow(dpy, DefaultScreen(dpy));
  ::XSetErrorHandler(&Box::XError);
  ::XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);
  ::XSync(dpy, false);
  if (Box::error)
    throw "Box error";

  ::XSetErrorHandler(&Box::XError);
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
      ev->error_code   == BadWindow));
}
