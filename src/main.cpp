#include <memory>
#include <iostream>
#include <algorithm>
#include <dobwm.h>
#include <../config.h>

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
  const ::Window win { x->map_request() };
  x->window(win, BORDER_WIDTH, BORDER_COLOR0);
}

void dobwm::Box::unmap_request(void) {
  ::Window win { x->unmap_notify() };
  for (auto &m : M)
    for (auto &t : m.T)
      if (auto c { std::find_if(t.C.begin(), t.C.end(),
          [&](auto &c) -> bool { return win == c.win; }) }; c < t.C.end()) {
        x->unmap_request(win);
        t.C.erase(c);
        return;
      }
}

void dobwm::Box::configure_request(void) {
  auto &ev { x->configure_request() };
  for (auto &m : M)
    for (auto &t : m.T)
      if (auto c { std::find_if(t.C.begin(), t.C.end(),
          [&](auto &c) -> bool { return ev.window == c.win; }) }; c < t.C.end()) {
        x->configure_window(ev, c->win);
        return;
      }
}

void dobwm::Box::init_windows(void) {
  x->query_tree(BORDER_WIDTH, BORDER_COLOR0);
}

void dobwm::Box::grab_button(void) {
  x->grab_button();
}

void dobwm::Box::grab_key(void) {
  x->grab_key();
}

int main(const int ARGC, const char *ARGV[]) {
  try {
    x = std::make_unique<dobwm::X>();
  } catch (const std::exception &e) {
    std::cerr << "EX: " + std::string(e.what()) << "\n";
    return -1;
  }

  box.init_windows();
  std::cout << "Dopenbox Window Manager\n";
  while (!quit) {
    x->next_event();
    if (x->event() == CreateNotify) x->create_notify();
    else if (x->event() == DestroyNotify) x->destroy_notify();
    else if (x->event() == ReparentNotify) x->reparent_notify();
    else if (x->event() == MapNotify) x->map_notify();
    else if (x->event() == UnmapNotify) box.unmap_request();
    else if (x->event() == ConfigureNotify) x->configure_notify();
    else if (x->event() == MapRequest) box.map_request();
    else if (x->event() == ConfigureRequest) box.configure_request();
    else if (x->event() == ButtonPress) x->button_press();
    else if (x->event() == ButtonRelease) x->button_release();
    else if (x->event() == MotionNotify) x->motion_notify();
    else if (x->event() == KeyPress) x->key_press();
    else if (x->event() == KeyRelease) x->key_release();
  }

  return 0;
}
