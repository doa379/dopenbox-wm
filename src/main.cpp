#include <memory>
#include <iostream>
#include <dobwm.h>
#include <config.h>

static bool quit { };
static std::unique_ptr<dobwm::X> x;
static dobwm::Box box;

dobwm::Box::Box(void) {
  for (auto i { 0U }; i < dobwm::Nm; i++) {
    std::vector<dobwm::Tag> T(dobwm::Nt);
    dobwm::Mon m { T };
    M.emplace_back(std::move(m));
  }
}

void dobwm::Box::map_request(void) {
  ::Window w { };
  auto wa { x->map_request(w) };
  Client c {
    x->manage(w, wa, BORDER_WIDTH, BORDER_COLOR, BG_COLOR),
    "Client name", 0, 0, 80, 80
  };

  M[0].T[0].C.emplace_back(std::move(c));
}

void dobwm::Box::configure_request(void) {
  x->configure_request();
}

int main(const int ARGC, const char *ARGV[]) {
  try {
    x = std::make_unique<dobwm::X>();
  } catch (const char E[]) {
    std::cerr << "EX: " + std::string(E) << "\n";
    return -1;
  }

  while (!quit && x->next_event()) {
    if (x->type() == CreateNotify) x->create_notify();
    else if (x->type() == DestroyNotify) x->destroy_notify();
    else if (x->type() == ReparentNotify) x->reparent_notify();
    else if (x->type() == MapNotify) x->map_notify();
    else if (x->type() == UnmapNotify) x->unmap_notify();
    else if (x->type() == ConfigureNotify) x->configure_notify();
    else if (x->type() == MapRequest) box.map_request();
    else if (x->type() == ConfigureRequest) box.configure_request();
    else if (x->type() == ButtonPress) x->button_press();
    else if (x->type() == ButtonRelease) x->button_release();
    else if (x->type() == MotionNotify) x->motion_notify();
    else if (x->type() == KeyPress) x->key_press();
    else if (x->type() == KeyRelease) x->key_release();
  }

  return 0;
}
