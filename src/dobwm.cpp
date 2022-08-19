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
  const auto kc { x->key_code() };
  if (x->key_state() == QUIT_KEY[0] &&
      x->key_press(kc) == QUIT_KEY[1]) {
    DBGMSG("Quit WM");
    quit = true;
  } else if (x->key_state() == RESTART_KEY[0] &&
              x->key_press(kc) == RESTART_KEY[1]) {
    DBGMSG("Restart WM");
    restart = true;
  } else if (x->key_state() == KILLCLI_KEY[0] &&
              x->key_press(kc) == KILLCLI_KEY[1]) {
    DBGMSG("Kill all");
  } else if (x->key_state() == UNMAPALL_KEY[0] &&
              x->key_press(kc) == UNMAPALL_KEY[1]) {
    DBGMSG("Unmap all");
    unmap_all();
  } else if (x->key_state() == REMAPALL_KEY[0] &&
              x->key_press(kc) == REMAPALL_KEY[1]) {
    DBGMSG("Remap all");
    init_clients();
  } else if (x->key_state() == SWCLIFOCUS_KEY[0] &&
              x->key_press(kc) == SWCLIFOCUS_KEY[1]) {
    swfocus();
  }
}

void dobwm::Box::init_clients(void) {
  for (const auto &C : x->query_tree()) {
    x->client(C, BRDR_WIDTH, INACTBRDR_COLOR);
    M.back().T.back().C.emplace_back(dobwm::Client { C });
  }

  x->grab_buttons();
  x->grab_key(RESTART_KEY[0], RESTART_KEY[1]);
  x->grab_key(QUIT_KEY[0], QUIT_KEY[1]);
  x->grab_key(UNMAPALL_KEY[0], UNMAPALL_KEY[1]);
  x->grab_key(REMAPALL_KEY[0], REMAPALL_KEY[1]);
  x->grab_key(KILLCLI_KEY[0], KILLCLI_KEY[1]);
  x->grab_key(SWCLIFOCUS_KEY[0], SWCLIFOCUS_KEY[1]);
}

void dobwm::Box::map_request(void) {
  const auto WIN { x->map_request() };
  x->client(WIN, BRDR_WIDTH, ACTBRDR_COLOR);
  M.back().T.back().C.emplace_back(dobwm::Client { WIN });
}

void dobwm::Box::configure_request(void) {
  DBGMSG("Config Req Event");
  auto &ev { x->configure_request() };
  x->configure_window(ev);
}

void dobwm::Box::unmap_request(void) {
  const auto WIN { x->unmap_notify() };
  x->unmap_request(WIN);
  /*
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      if (auto c { std::find_if(T.C.begin(), T.C.end(),
          [&](const auto &C) -> bool { return C.win == WIN; }) }; 
            c < T.C.end()) {
        x->unmap_request(WIN);
        T.C.erase(c);
        return;
      }
  */
}

void dobwm::Box::unmap_all(void) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      for (const auto &C : T.C)
        x->unmap_request(C.win);
}

void dobwm::Box::swfocus(void) const {

}

int main(const int ARGC, const char *ARGV[]) {
__start__:
  try {
    x = std::make_unique<dobwm::X>();
  } catch (const std::exception &E) {
    std::cerr << "EX: " + std::string(E.what()) << "\n";
    return -1;
  }
  
  try {
    msg = std::make_unique<dobwm::Msg>();
  } catch (const std::exception &E) {
    std::cerr << "EX: " + std::string(E.what()) << "\n";
  }

  box = std::make_unique<dobwm::Box>();
  box->init_clients();
  std::cout << "Dopenbox Window Manager ver. " << dobwm::VER << "\n";
  ::DBGMSG("WM init.");
  while (!quit && !restart && !x->next_event()) {
    if (x->event() == dobwm::XEvent::Create)
      x->create_notify();
    else if (x->event() == dobwm::XEvent::Destroy)
      x->destroy_notify();
    else if (x->event() == dobwm::XEvent::Reparent)
      x->reparent_notify();
    else if (x->event() == dobwm::XEvent::Map)
      x->map_notify();
    else if (x->event() == dobwm::XEvent::Unmap)
      box->unmap_request();
    else if (x->event() == dobwm::XEvent::CliMsg)
      ;
    else if (x->event() == dobwm::XEvent::Config)
      x->configure_notify();
    else if (x->event() == dobwm::XEvent::MapReq)
      box->map_request();
    else if (x->event() == dobwm::XEvent::ConfigReq)
      box->configure_request();
    else if (x->event() == dobwm::XEvent::Motion)
      x->motion_notify();
    else if (x->event() == dobwm::XEvent::Button)
      x->button();
    else if (x->event() == dobwm::XEvent::Key)
      box->key();
  }

  if (restart) {
    x.reset();
    msg.reset();
    box.reset();
    restart = false;
    goto __start__;
  }

  return 0;
}
