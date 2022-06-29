#include <memory>
#include <iostream>
#include <algorithm>
#include <dobwm.h>
#include <../config.h>

static bool quit { };
static std::unique_ptr<dobwm::X> x;
static dobwm::Box box;

dobwm::Box::Box(void) {
  for (auto i { 0U }; i < Nm; i++) {
    std::vector<Tag> T(Nt);
    Mon m { T };
    M.emplace_back(std::move(m));
  }
}

dobwm::Box::~Box(void) {
  /*
  for (auto &m : M)
    for (auto &t : m.T)
      for (auto &c : t.C)
        x->unmap_request(c.win);
  */
}

void dobwm::Box::unmap_all(void) {
  for (auto &m : M)
    for (auto &t : m.T)
      for (auto &c : t.C)
        x->unmap_request(c.win);
}

void dobwm::Box::map_request(void) {
  const auto win { x->map_request() };
  x->window(win, BORDER_WIDTH, BORDER_COLOR0);
}

void dobwm::Box::unmap_request(void) {
  auto win { x->unmap_notify() };
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
  for (const auto &m : M)
    for (const auto &t : m.T)
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
  std::cout << "Dopenbox Window Manager ver. " << VER << "\n";
  while (!quit) {
    x->next_event();
    if (x->event() == dobwm::XEvent::Create) x->create_notify();
    else if (x->event() == dobwm::XEvent::Destroy) x->destroy_notify();
    else if (x->event() == dobwm::XEvent::Reparent) x->reparent_notify();
    else if (x->event() == dobwm::XEvent::Map) x->map_notify();
    else if (x->event() == dobwm::XEvent::Unmap) box.unmap_request();
    else if (x->event() == dobwm::XEvent::Config) x->configure_notify();
    else if (x->event() == dobwm::XEvent::MapReq) box.map_request();
    else if (x->event() == dobwm::XEvent::ConfigReq) box.configure_request();
    else if (x->event() == dobwm::XEvent::BDown) x->button_press();
    else if (x->event() == dobwm::XEvent::BUp) x->button_release();
    else if (x->event() == dobwm::XEvent::Motion) x->motion_notify();
    else if (x->event() == dobwm::XEvent::KDown) x->key_press();
    else if (x->event() == dobwm::XEvent::KUp) x->key_release();
  }

  box.unmap_all();
  return 0;
}
