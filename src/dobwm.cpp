#include <memory>
#include <iostream>
#include <algorithm>
#include <dobwm.h>
#include <msg.h>
#include <../config.h>

static bool quit { }, restart { };
static std::unique_ptr<dobwm::X> x;
static std::unique_ptr<dobwm::Msg> msg;
static std::unique_ptr<dobwm::Box> box;

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

}

void dobwm::Box::key(void) {
  const auto KC { x->key_code() };
  if (x->key_state() == QUIT_KEY[0] && x->key_press(KC) == QUIT_KEY[1]) {
    DBGMSG("Quit WM");
    quit = true;
  } else if (x->key_state() == RESTART_KEY[0] && x->key_press(KC) == RESTART_KEY[1]) {
    DBGMSG("Restart WM");
    restart = true;
  } else if (x->key_state() == KILLCLI_KEY[0] && x->key_press(KC) == KILLCLI_KEY[1]) {
    DBGMSG("Killall");
    unmap_all();
  } else if (x->key_state() == SWCLIFOCUS_KEY[0] && x->key_press(KC) == SWCLIFOCUS_KEY[1]) {
    swfocus();
  }
}

void dobwm::Box::init_clients(void) {
  x->query_tree(BORDER_WIDTH, BORDER_COLOR0);
  x->grab_buttons();
  x->grab_key(RESTART_KEY[0], RESTART_KEY[1]);
  x->grab_key(QUIT_KEY[0], QUIT_KEY[1]);
  x->grab_key(KILLCLI_KEY[0], KILLCLI_KEY[1]);
  x->grab_key(SWCLIFOCUS_KEY[0], SWCLIFOCUS_KEY[1]);
}

void dobwm::Box::map_request(void) {
  const auto win { x->map_request() };
  x->client(win, BORDER_WIDTH, BORDER_COLOR0);
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

void dobwm::Box::unmap_all(void) {
  for (auto &m : M)
    for (auto &t : m.T)
      for (auto &c : t.C)
        x->unmap_request(c.win);
}

void dobwm::Box::swfocus(void) const {

}

int main(const int ARGC, const char *ARGV[]) {
__start__:
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

  box = std::make_unique<dobwm::Box>();
  box->init_clients();
  std::cout << "Dopenbox Window Manager ver. " << dobwm::VER << "\n";
  ::DBGMSG("WM init.");
  while(!quit && !restart && !x->next_event()) {
    if (x->event() == dobwm::XEvent::Create) x->create_notify();
    else if (x->event() == dobwm::XEvent::Destroy) x->destroy_notify();
    else if (x->event() == dobwm::XEvent::Reparent) x->reparent_notify();
    else if (x->event() == dobwm::XEvent::Map) x->map_notify();
    else if (x->event() == dobwm::XEvent::Unmap) box->unmap_request();
    else if (x->event() == dobwm::XEvent::Config) x->configure_notify();
    else if (x->event() == dobwm::XEvent::MapReq) box->map_request();
    else if (x->event() == dobwm::XEvent::ConfigReq) box->configure_request();
    else if (x->event() == dobwm::XEvent::Motion) x->motion_notify();
    else if (x->event() == dobwm::XEvent::Button) x->button();
    else if (x->event() == dobwm::XEvent::Key) box->key();
  }

  //box->unmap_all();
  if (restart) {
    x.reset();
    msg.reset();
    box.reset();
    restart = false;
    goto __start__;
  }

  return 0;
}
