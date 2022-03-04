#include <memory>
#include <iostream>
#include <Xlib.h>
#include <config.h>

static bool quit { };
static std::unique_ptr<Xlib::Box> box;

int main(const int ARGC, const char *ARGV[]) {
  try {
    box = std::make_unique<Xlib::Box>(dobwm::Nm, dobwm::Nt);
  } catch (const char E[]) {
    std::cout << "EX: " + std::string(E) << "\n";
    return -1;
  }

  while (!quit || box->event()) {
    if (box->type() == CreateNotify);
    else if (box->type() == DestroyNotify);
    else if (box->type() == ReparentNotify);
    else if (box->type() == MapNotify);
    else if (box->type() == UnmapNotify);
    else if (box->type() == ConfigureNotify);
    else if (box->type() == MapRequest);
    else if (box->type() == ConfigureRequest);
    else if (box->type() == ButtonPress);
    else if (box->type() == ButtonRelease);
    else if (box->type() == MotionNotify);
    else if (box->type() == KeyPress);
    else if (box->type() == KeyRelease);
  }

  return 0;
}
