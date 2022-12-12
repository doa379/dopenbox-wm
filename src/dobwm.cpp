#include <iostream>
//#include <print>
#include <algorithm>
#include <cstdlib>
#include <dobwm.h>
#include <msg.h>
#include <../config.h>

static bool quit { };
static dobwm::Msg msg;

auto DBGMSG(const char MSG[]) {
  msg.send("Debug", MSG, dobwm::Urg::NORMAL, 1000);
}

dobwm::Box::Box(void) {
  for (const auto &D : x.MONS()) {
    std::vector<Tag> T { Nt };
    Mon m { T, D };
    this->M.emplace_back(std::move(m));
  }
}

dobwm::Box::~Box(void) {

}

auto dobwm::Box::focus(Hnd &H) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) {
        x.client(H.win, BDR_WIDTH, static_cast<unsigned long>(INACTBDR_COLOR)); });

  x.focus(H.win);
  x.client(H.win, BDR_WIDTH, static_cast<unsigned long>(ACTBDR_COLOR));
  curr = HndRef { std::ref(H) };
}

auto dobwm::Box::sw_focus(void) {
  for (auto &m : M)
    for (auto &t : m.T)
      for (auto h { t.H.begin() }; h < t.H.end(); h++)
        if (*h == curr->get()) {
          if (h < t.H.end() - 1) {
            focus(*(h + 1));
            curr = HndRef { std::ref(*(h + 1)) };
            return;
          } else if (h == t.H.end() - 1) {
            focus(*t.H.begin());
            curr = HndRef { std::ref(*t.H.begin()) };
            return;
          }
        }
}

auto dobwm::Box::map_request(const ::Window W) {
  /*
  x.map_window(W);
  if (const auto D { x.client_dim(W) }; D.has_value() &&
      !x.client_trans(W)) {
    M.back().T.back().H.emplace_back(Hnd { W, D.value(), true });
    focus(M.back().T.back().H.back());
  }
  */

  if (const auto D { x.client_dim(W) }; D.has_value()) {
    M.back().T.back().H.emplace_back(Hnd { W, D.value(), true });
    focus(M.back().T.back().H.back());
  }
}

auto dobwm::Box::map_all(void) const {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) {
        x.map_window(H.win); });
}

auto dobwm::Box::unmap_all(void) const {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) {
        x.unmap_window(H.win); });
}

auto dobwm::Box::del_hnd(const Hnd &H) {
  for (auto &m : M)
    for (auto &t : m.T)
      if (const auto H_ { std::ranges::find(t.H, H) }; H_ < t.H.end()) {
          t.H.erase(H_);
          return;
      }
}

auto dobwm::Box::key(void) {
  const Kb KB { 
    x.key_state(), static_cast<Key>(x.key_press(x.key_code())) };
  if (KB == QUIT) {
      ::DBGMSG("Quit WM");
      quit = true;
  } else if (KB == KILLCLI) {
      ::DBGMSG("Kill Curr");
      if (curr.has_value()) {
        const auto W { curr->get().win };
        sw_focus();
        if (x.kill_client(W))
          del_hnd(*curr);
      }
  } else if (KB == UNMAPALL) {
      ::DBGMSG("Unmap all");
      unmap_all();
  } else if (KB == REMAPALL) {
      ::DBGMSG("Remap all");
      map_all();
  }

  if (curr.has_value()) {
    if (KB == SWCLIFOCUS)
        sw_focus();
    else if (KB == MOVEUP)
        x.move(curr->get().win, curr->get().d.x, curr->get().d.y -= MOVESTEP_PX);
    else if (KB == MOVEDOWN)
        x.move(curr->get().win, curr->get().d.x, curr->get().d.y += MOVESTEP_PX);
    else if (KB == MOVELEFT)
        x.move(curr->get().win, curr->get().d.x -= MOVESTEP_PX, curr->get().d.y);
    else if (KB == MOVERIGHT)
        x.move(curr->get().win, curr->get().d.x += MOVESTEP_PX, curr->get().d.y);
  }
  
  for (const auto &CMD : CMDS)
    if (KB == std::get<0>(CMD)) {
      ::system(std::string(std::get<1>(CMD)).c_str());
      return;
    }
  
  for (const auto &CMD : CMDS_ASYNC)
    if (KB == std::get<0>(CMD)) {
      // fork()
      return;
    }
}

auto dobwm::Box::init(void) {
  ::DBGMSG(std::to_string(x.query_tree().size()).c_str());
  for (const auto &W : x.query_tree())
    map_request(W);
  
  x.grab_key(std::get<0>(QUIT), static_cast<unsigned long>(std::get<1>(QUIT)));
  x.grab_key(std::get<0>(QUIT), static_cast<unsigned long>(std::get<1>(QUIT)));
  x.grab_key(std::get<0>(UNMAPALL), static_cast<unsigned long>(std::get<1>(UNMAPALL)));
  x.grab_key(std::get<0>(REMAPALL), static_cast<unsigned long>(std::get<1>(REMAPALL)));
  x.grab_key(std::get<0>(KILLCLI), static_cast<unsigned long>(std::get<1>(KILLCLI)));
  x.grab_key(std::get<0>(SWCLIFOCUS), static_cast<unsigned long>(std::get<1>(SWCLIFOCUS)));
  x.grab_key(std::get<0>(SELTOGGLE), static_cast<unsigned long>(std::get<1>(SELTOGGLE)));
  x.grab_key(std::get<0>(SELCLEAR), static_cast<unsigned long>(std::get<1>(SELCLEAR)));
  x.grab_key(std::get<0>(MOVEUP), static_cast<unsigned long>(std::get<1>(MOVEUP)));
  x.grab_key(std::get<0>(MOVEDOWN), static_cast<unsigned long>(std::get<1>(MOVEDOWN)));
  x.grab_key(std::get<0>(MOVELEFT), static_cast<unsigned long>(std::get<1>(MOVELEFT)));
  x.grab_key(std::get<0>(MOVERIGHT), static_cast<unsigned long>(std::get<1>(MOVERIGHT)));
  for (const auto &CMDS_ : { CMDS, CMDS_ASYNC })
    for (const auto &CMD : CMDS_) {
      const Kb KB { std::get<0>(CMD) };
      x.grab_key(std::get<0>(KB), static_cast<unsigned long>(std::get<1>(KB)));
    }
  
  //x.grab_button(SELECT.first, static_cast<int>(SELECT.second));
  //x.grab_button(RESIZE.first, static_cast<int>(RESIZE.second));
}

auto dobwm::Box::configure_request(void) {
  ::DBGMSG("Config Req Event");
  auto &ev { x.configure_request() };
  x.configure_window(ev);
}

auto dobwm::Box::unmap_request(void) {
  const auto W { x.unmap_notify() };
  x.unmap_window(W);
}

auto dobwm::Box::cli_msg(void) const {
  ::DBGMSG("Client Msg Event");
  //x.kill_msg();
}

auto dobwm::Box::hnd(const ::Window W) -> HndRef {
  for (auto &m : M)
    for (auto &t : m.T)
      if (auto h { 
          std::ranges::find_if(t.H, [&](const Hnd &H) -> bool {
            return H.win == W; }) }; h < t.H.end())
        return *h;
 
  return std::nullopt;
}

auto dobwm::Box::enter_notify(void) {
  if (SLOPPY_FOCUS && curr.has_value() &&
      x.crossing_window() != curr->get().win)
    focus(*hnd(x.crossing_window()));
}

auto dobwm::Box::button(void) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) { 
        x.grab_button(H.win, std::get<0>(SELECT), 
          static_cast<unsigned>(std::get<1>(SELECT)));
      });

  const Btn B { x.button_state(), static_cast<Button>(x.button()) }; 
  if (B == SELECT) {
    const auto H { hnd(x.button_window()) };
    if (H.has_value() && H->get() != *curr)
      focus(H->get());
  } else if (B == RESIZE) {
    ::DBGMSG("Resize button");
  }
  
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) { 
        x.ungrab_button(H.win, std::get<0>(SELECT),
          static_cast<unsigned>(std::get<1>(SELECT)));
      });
}

auto dobwm::Box::ev(void) {
  if (x.next_event()) {
    if (x.event() == dobwm::XEvent::Map)
      x.map_notify();
    else if (x.event() == dobwm::XEvent::Unmap)
      unmap_request();
    else if (x.event() == dobwm::XEvent::CliMsg)
      cli_msg();
    else if (x.event() == dobwm::XEvent::Config)
      x.configure_notify();
    else if (x.event() == dobwm::XEvent::MapReq) {
      const auto W { x.Event::map_request() };
      map_request(W);
    } else if (x.event() == dobwm::XEvent::ConfigReq)
      configure_request();
    else if (x.event() == dobwm::XEvent::Motion)
      x.motion_notify();
    else if (x.event() == dobwm::XEvent::Key)
      key();
    else if (x.event() == dobwm::XEvent::Button)
      button();
    else if (x.event() == dobwm::XEvent::Enter)
      enter_notify();
  }
}

auto main(const int ARGC, const char *ARGV[]) -> int {
  try {
    dobwm::Box box;
    box.init();
    //std::println("...");
    std::cout << "Dopenbox Window Manager ver. " << dobwm::VER << "\n";
    ::DBGMSG("WM init.");
    while (!quit)
      box.ev();
  } catch (const std::exception &E) {
    std::cerr << "EX: " + std::string(E.what()) << "\n";
    //std::println("EX: { }", E.what());
    return -1;
  }
  
  return 0;
}
