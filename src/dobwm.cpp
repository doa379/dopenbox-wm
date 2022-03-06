#include <memory>
#include <iostream>
#include <Xlib.h>
#include <config.h>

static bool quit { };
static std::unique_ptr<Xlib::Box> box;

namespace dobwm {
  struct Client {
    ::Window win;
    std::string name;
    unsigned x { }, y { }, w { }, h { };
  };

  struct Tag {
    std::list<Client> C;
  };

  struct Mon {
    std::list<Tag> T;
    unsigned w { }, h { };
  };
}

static std::list<dobwm::Mon> M;

int main(const int ARGC, const char *ARGV[]) {
  for (auto i { 0U }; i < dobwm::Nm; i++) {
    std::list<dobwm::Tag> T(dobwm::Nt);
    dobwm::Mon m { T };
    M.emplace_back(std::move(m));
  }

  try {
    box = std::make_unique<Xlib::Box>();
  } catch (const char E[]) {
    std::cout << "EX: " + std::string(E) << "\n";
    return -1;
  }

  while (!quit || box->event()) {
    if (box->type() == CreateNotify)
      box->create_notify();
    else if (box->type() == DestroyNotify)
      box->destroy_notify();
    else if (box->type() == ReparentNotify)
      box->reparent_notify();
    else if (box->type() == MapNotify)
      box->map_notify();
    else if (box->type() == UnmapNotify)
      box->unmap_notify();
    else if (box->type() == ConfigureNotify)
      box->configure_notify();
    else if (box->type() == MapRequest)
      box->map_request();
    else if (box->type() == ConfigureRequest)
      box->configure_request();
    else if (box->type() == ButtonPress)
      box->button_press();
    else if (box->type() == ButtonRelease)
      box->button_release();
    else if (box->type() == MotionNotify)
      box->motion_notify();
    else if (box->type() == KeyPress)
      box->key_press();
    else if (box->type() == KeyRelease)
      box->key_release();
  }

  return 0;
}
