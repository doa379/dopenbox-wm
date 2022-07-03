#include <memory>
#include <iostream>
#include <algorithm>
#include <dobwm.h>
#include <msg.h>
#include <../config.h>

static bool quit { };
static std::unique_ptr<dobwm::X> x;
static std::unique_ptr<dobwm::Msg> msg;
static dobwm::Box box;

void DBGMSG(const char MSG[]) {
  msg->send("Debug", MSG, dobwm::Urg::NORMAL, 1000);
}

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

void dobwm::Box::key(void) {
  DBGMSG("Input handler");
  auto key { x->key() };
  if (x->key_press(key))
    DBGMSG("Input handler");
}

void dobwm::Box::init_windows(void) {
  x->query_tree(BORDER_WIDTH, BORDER_COLOR0);
  x->grab_buttons();
  const std::vector<int> K {
    RESTART_KEY, 
    QUIT_KEY };
  x->grab_keys(K);
}

int main(const int ARGC, const char *ARGV[]) {
  try {
    x = std::make_unique<dobwm::X>();
  } catch (const std::exception &e) {
    std::cerr << "EX: " + std::string(e.what()) << "\n";
    return -1;
  }
  
  try {
    msg = std::make_unique<dobwm::Msg>();
  } catch (const std::exception &e) {
    std::cerr << "EX: " + std::string(e.what()) << "\n";
  }

  box.init_windows();
  std::cout << "Dopenbox Window Manager ver. " << dobwm::VER << "\n";
  ::DBGMSG("WM init.");
  while(!quit && !x->next_event()) {
    if (x->event() == dobwm::XEvent::Create) x->create_notify();
    else if (x->event() == dobwm::XEvent::Destroy) x->destroy_notify();
    else if (x->event() == dobwm::XEvent::Reparent) x->reparent_notify();
    else if (x->event() == dobwm::XEvent::Map) x->map_notify();
    else if (x->event() == dobwm::XEvent::Unmap) box.unmap_request();
    else if (x->event() == dobwm::XEvent::Config) x->configure_notify();
    else if (x->event() == dobwm::XEvent::MapReq) box.map_request();
    else if (x->event() == dobwm::XEvent::ConfigReq) box.configure_request();
    else if (x->event() == dobwm::XEvent::Motion) x->motion_notify();
    else if (x->event() == dobwm::XEvent::Button) x->button();
    else if (x->event() == dobwm::XEvent::Key) box.key();
  }

  box.unmap_all();
  return 0;
}
