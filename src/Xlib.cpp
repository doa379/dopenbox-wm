#include <Xlib.h>

Xlib::Box::Box(const unsigned char Nm, const unsigned char Nt) {
  if (!(dpy = ::XOpenDisplay(nullptr)))
    throw "Unable to open display";

  for (auto i { 0U }; i < Nm; i++) {
    std::list<Tag> T(Nt);
    Mon m { T };
    M.emplace_back(std::move(m));
  }
}

Xlib::Box::~Box(void)
{
  ::XCloseDisplay(dpy);
}
