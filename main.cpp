#include <X11/Xutil.h>
#include <dopenbox.h>
#include <config.h>

static bool quit { };
static dopenbox::Box box { dopenbox::Nm, dopenbox::Nt };
static ::Display *dpy = ::XOpenDisplay(NULL);

int main(const int ARGC, const char *ARGV[]) {
  ::XEvent e;
  while (quit == false && !::XNextEvent(dpy, &e)) {
    ;
  }

  return 0;
}
