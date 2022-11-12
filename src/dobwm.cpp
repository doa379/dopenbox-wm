#include <memory>
#include <iostream>
//#include <print>
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
    std::vector<Tag> T { Nt };
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
      ::DBGMSG("Quit WM");
      quit = true;
  } else if (x->key_state() == RESTART.first &&
    x->key_press(KC) == static_cast<int>(RESTART.second)) {
      ::DBGMSG("Restart WM");
      restart = true;
  } else if (x->key_state() == KILLCLI.first &&
    x->key_press(KC) == static_cast<int>(KILLCLI.second)) {
      ::DBGMSG("Kill Curr");
      if (curr.has_value()) {
        const auto W { curr->get().win };
        sw_focus();
        if (x->kill_client(W))
          del_client(*curr);
      }
  } else if (x->key_state() == UNMAPALL.first &&
    x->key_press(KC) == static_cast<int>(UNMAPALL.second)) {
      ::DBGMSG("Unmap all");
      unmap_all();
  } else if (x->key_state() == REMAPALL.first &&
    x->key_press(KC) == static_cast<int>(REMAPALL.second)) {
      ::DBGMSG("Remap all");
      map_all();
  }

  if (curr.has_value()) {
    if (x->key_state() == SWCLIFOCUS.first &&
      x->key_press(KC) == static_cast<int>(SWCLIFOCUS.second))
        sw_focus();
    else if (x->key_state() == MOVEUP.first &&
      x->key_press(KC) == static_cast<int>(MOVEUP.second))
        x->move(curr->get().win, 0, MOVESTEP_PX);
    else if (x->key_state() == MOVEDOWN.first &&
      x->key_press(KC) == static_cast<int>(MOVEDOWN.second))
        x->move(curr->get().win, 0, -MOVESTEP_PX);
    else if (x->key_state() == MOVELEFT.first &&
      x->key_press(KC) == static_cast<int>(MOVELEFT.second))
        x->move(curr->get().win, -MOVESTEP_PX, 0);
    else if (x->key_state() == MOVERIGHT.first &&
      x->key_press(KC) == static_cast<int>(MOVERIGHT.second))
        x->move(curr->get().win, MOVESTEP_PX, 0);
  }

  for (const auto &CMD : CMDS)
    if (x->key_state() == std::get<0>(CMD) &&
        x->key_press(KC) == static_cast<unsigned long>(std::get<1>(CMD))) {
      ::system(std::string(std::get<2>(CMD)).c_str());
      break;
    }
}

void dobwm::Box::init(void) {
  ::DBGMSG(std::to_string(x->query_tree().size()).c_str());
  for (const auto &W : x->query_tree()) {
    x->client(W, BDR_WIDTH, INACTBDR_COLOR);
    x->map_window(W);
    M.back().T.back().C.emplace_back(Client { W });
    focus(M.back().T.back().C.back());
  }
  
  x->grab_key(QUIT.first, static_cast<int>(QUIT.second));
  x->grab_key(RESTART.first, static_cast<int>(RESTART.second));
  x->grab_key(UNMAPALL.first, static_cast<int>(UNMAPALL.second));
  x->grab_key(REMAPALL.first, static_cast<int>(REMAPALL.second));
  x->grab_key(KILLCLI.first, static_cast<int>(KILLCLI.second));
  x->grab_key(SWCLIFOCUS.first, static_cast<int>(SWCLIFOCUS.second));
  x->grab_key(SELTOGGLE.first, static_cast<int>(SELTOGGLE.second));
  x->grab_key(SELCLEAR.first, static_cast<int>(SELCLEAR.second));
  x->grab_key(MOVEUP.first, static_cast<int>(MOVEUP.second));
  x->grab_key(MOVEDOWN.first, static_cast<int>(MOVEDOWN.second));
  x->grab_key(MOVELEFT.first, static_cast<int>(MOVELEFT.second));
  x->grab_key(MOVERIGHT.first, static_cast<int>(MOVERIGHT.second));
  for (const auto &CMD : CMDS)
    x->grab_key(std::get<0>(CMD), static_cast<int>(std::get<1>(CMD)));
  
  //x->grab_button(SELECT.first, static_cast<int>(SELECT.second));
  //x->grab_button(RESIZE.first, static_cast<int>(RESIZE.second));
}

void dobwm::Box::configure_request(void) {
  ::DBGMSG("Config Req Event");
  auto &ev { x->configure_request() };
  x->configure_window(ev);
}

void dobwm::Box::map_request(void) {
  const auto W { x->Event::map_request() };
  x->map_window(W);
  M.back().T.back().C.emplace_back(Client { W });
  focus(M.back().T.back().C.back());
}

void dobwm::Box::unmap_request(void) {
  const auto W { x->unmap_notify() };
  x->unmap_window(W);
}

void dobwm::Box::map_all(void) const {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.C, [&](const Client &C) {
        x->map_window(C.win); });
}

void dobwm::Box::unmap_all(void) const {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.C, [&](const Client &C) {
        x->unmap_window(C.win); });
}

void dobwm::Box::cli_msg(void) const {
  ::DBGMSG("Client Msg Event");
  //x->kill_msg();
}

void dobwm::Box::focus(Client &C) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.C, [&](const Client &C) {
          x->client(C.win, BDR_WIDTH, INACTBDR_COLOR); });
          
  x->focus(C.win);
  x->client(C.win, BDR_WIDTH, ACTBDR_COLOR);
  curr = HndRef { std::ref(C) };
}

void dobwm::Box::sw_focus(void) {
  for (auto &M : this->M)
    for (auto &T : M.T)
      for (auto c { T.C.begin() }; c < T.C.end(); c++) {
        if (*c == curr->get()) {
          if (c < T.C.end() - 1) {
            focus(*(c + 1));
            curr = HndRef { std::ref(*(c + 1)) };
            return;
          } else if (T.C.size() > 1 && c == T.C.end() - 1) {
            focus(*(c - 1));
            curr = HndRef { std::ref(*(c - 1)) };
            return;
          }
        }
      }
}

decltype(dobwm::Box::curr) dobwm::Box::client(const ::Window W) {
  for (auto &m : M)
    for (auto &t : m.T)
      if (auto c { 
          std::ranges::find_if(t.C, [&](const Client &C) -> bool {
            return C.win == W; }) }; c < t.C.end())
        return *c;
 
  return std::nullopt;
}

void dobwm::Box::del_client(const Client &C) {
  for (auto &m : M)
    for (auto &t : m.T)
      if (const auto C_ { std::ranges::find(t.C, C) }; C_ < t.C.end()) {
          t.C.erase(C_);
          return;
      }
}

void dobwm::Box::enter_notify(void) {
  if (SLOPPY_FOCUS && 
        curr.has_value() && 
          x->crossing_window() != curr->get().win)
    focus(*client(x->crossing_window()));
}

void dobwm::Box::button(void) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.C, [&](const Client &C) { 
        x->grab_button(C.win, SELECT.first, static_cast<int>(SELECT.second));
      });

  if (x->button_state() == SELECT.first && 
    x->button() == static_cast<int>(SELECT.second)) {
    const auto C { client(x->button_window()) };
    if (C.has_value() && C->get() != *curr)
      focus(C->get());
  } else if (x->button_state() == RESIZE.first && 
      x->button() == static_cast<int>(RESIZE.second)) {
    ::DBGMSG("Resize button");
  }
  
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.C, [&](const Client &C) { 
        x->ungrab_button(C.win, SELECT.first, static_cast<int>(SELECT.second));
      });
}

int main(const int ARGC, const char *ARGV[]) {
__start__:
  try {
    x = std::make_unique<dobwm::X>();
  } catch (const std::exception &E) {
    std::cerr << "EX: " + std::string(E.what()) << "\n";
    //std::println("EX: { }", E.what());
    return -1;
  }
  
  try {
    msg = std::make_unique<dobwm::Msg>();
  } catch (const std::exception &E) {
    std::cerr << "EX: " + std::string(E.what()) << "\n";
    //std::println("EX: { }", E.what());
  }

  box = std::make_unique<dobwm::Box>();
  box->init();
  //std::println("...");
  std::cout << "Dopenbox Window Manager ver. " << dobwm::VER << "\n";
  ::DBGMSG("WM init.");
  while (!quit && !restart && !x->next_event()) {
    /*
    if (x->event() == dobwm::XEvent::Create)
      x->create_notify();
    else if (x->event() == dobwm::XEvent::Destroy)
      x->destroy_notify();
    else if (x->event() == dobwm::XEvent::Reparent)
      x->reparent_notify();
    */
    if (x->event() == dobwm::XEvent::Map)
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
    else if (x->event() == dobwm::XEvent::Key)
      box->key();
    else if (x->event() == dobwm::XEvent::Button)
      box->button();
    else if (x->event() == dobwm::XEvent::Enter)
      box->enter_notify();
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
