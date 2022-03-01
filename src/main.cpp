#include <X11/Xutil.h>
#include <dobwm.h>
#include <config.h>

static bool quit { };
static dobwm::Box box { dobwm::Nm, dobwm::Nt };
static ::Display *dpy { ::XOpenDisplay(nullptr) };

int main(const int ARGC, const char *ARGV[]) {
  if (!dpy) {
    ::XCloseDisplay(dpy);
    return -1;
  }

  ::XEvent ev;
  while (quit == false && !::XNextEvent(dpy, &ev)) {
    if (true) ;
  }

  ::XCloseDisplay(dpy);
  return 0;
}
