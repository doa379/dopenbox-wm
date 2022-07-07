#include <memory>
#include <iostream>
#include <algorithm>
#include <dobwm.h>
#include <msg.h>
#include <../config.h>

static bool quit { }, restart { };
static std::unique_ptr<dobwm::X> x;
static std::unique_ptr<dobwm::Msg> msg;
static dobwm::Box box;

///////////////////////////////////////////////////////////////////
// Notation
// Arrays/Vects Uppercase
// const primitives Uppercase
// Structs Capital
// Objects Lowercase
///////////////////////////////////////////////////////////////////

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
  const auto kc { x->key_code() };
  if (x->key_state() == QUIT_KEY[0] && x->key_press(kc) == QUIT_KEY[1]) {
    DBGMSG("Quit WM");
    quit = true;
  } else if (x->key_state() == RESTART_KEY[0] && x->key_press(kc) == RESTART_KEY[1]) {
    DBGMSG("Restart WM");
    quit = true;
    restart = true;
  } else if (x->key_state() == SOME_KEY[0] && x->key_press(kc) == SOME_KEY[1]) {
    DBGMSG("...");
  }
}

void dobwm::Box::init_windows(void) {
  x->query_tree(BORDER_WIDTH, BORDER_COLOR0);
  x->grab_buttons();
  x->grab_key(RESTART_KEY[0], RESTART_KEY[1]);
  x->grab_key(QUIT_KEY[0], QUIT_KEY[1]);
  x->grab_key(SOME_KEY[0], SOME_KEY[1]);
}

int main(const int ARGC, const char *ARGV[]) {
__restart__:
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

  //box.unmap_all();
  if (restart) {
    restart = false;
    quit = false;
    goto __restart__;
  }

  return 0;
}
