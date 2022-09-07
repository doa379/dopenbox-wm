#include <memory>
#include <iostream>
#include <algorithm>
#include <cstdlib>
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
  if (x->key_state() == QUIT.first && 
    x->key_press(KC) == static_cast<int>(QUIT.second)) {
      DBGMSG("Quit WM");
      quit = true;
  } else if (x->key_state() == RESTART.first &&
    x->key_press(KC) == static_cast<int>(RESTART.second)) {
      DBGMSG("Restart WM");
      restart = true;
  } else if (x->key_state() == KILLCLI.first &&
    x->key_press(KC) == static_cast<int>(KILLCLI.second)) {
      DBGMSG("Kill Last");
      const auto C { M.back().T.back().C.back() };
      x->kill_msg(C.win);
      M.back().T.back().C.pop_back();
  } else if (x->key_state() == UNMAPALL.first &&
    x->key_press(KC) == static_cast<int>(UNMAPALL.second)) {
      DBGMSG("Unmap all");
      unmap_all();
  } else if (x->key_state() == REMAPALL.first &&
    x->key_press(KC) == static_cast<int>(REMAPALL.second)) {
      DBGMSG("Remap all");
      init();
  } else if (x->key_state() == LAUNCHER.first &&
    x->key_press(KC) == static_cast<int>(LAUNCHER.second)) {
      DBGMSG("Launcher");
      ::system(dobwm::LAUNCHER_CMD);
  } else if (x->key_state() == SWCLIFOCUS.first &&
    x->key_press(KC) == static_cast<int>(SWCLIFOCUS.second)) {
      swfocus();
  }
}

void dobwm::Box::init(void) {
  for (const auto &C : x->query_tree()) {
    x->client(C, BRDR_WIDTH, INACTBRDR_COLOR);
    M.back().T.back().C.emplace_back(dobwm::Client { C });
  }

  x->grab_buttons();
  x->grab_key(QUIT.first, static_cast<int>(QUIT.second));
  x->grab_key(RESTART.first, static_cast<int>(RESTART.second));
  x->grab_key(UNMAPALL.first, static_cast<int>(UNMAPALL.second));
  x->grab_key(REMAPALL.first, static_cast<int>(REMAPALL.second));
  x->grab_key(LAUNCHER.first, static_cast<int>(LAUNCHER.second));
  x->grab_key(KILLCLI.first, static_cast<int>(KILLCLI.second));
  x->grab_key(SWCLIFOCUS.first, static_cast<int>(SWCLIFOCUS.second));
  x->grab_key(SELTOGGLE.first, static_cast<int>(SELTOGGLE.second));
  x->grab_key(SELCLEAR.first, static_cast<int>(SELCLEAR.second));
}

void dobwm::Box::map_request(void) {
  const auto WIN { x->map_request() };
  x->client(WIN, BRDR_WIDTH, INACTBRDR_COLOR);
  M.back().T.back().C.emplace_back(dobwm::Client { WIN });
  focus(M.back().T.back().C.back());
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

void dobwm::Box::cli_msg(void) const {
  DBGMSG("Client Msg Event");
  x->kill_msg();
}

void dobwm::Box::swfocus(void) const {

}

void dobwm::Box::focus(const Client &C) const {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      for (auto &c : T.C) {
          //unfocus(c.win);
          ;
      }

  // set focus on C.win
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
  box->init();
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
      box->cli_msg();
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
