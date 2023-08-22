#include <iostream>
//#include <print>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <dobwm.h>
#include <../config.h>

static bool quit { };

template<typename T>
concept Stringular = requires(T &t) {
  { t } -> std::convertible_to<std::basic_string<char>>;
};

template<typename T>
concept Strangular = requires(T &t) {
  { t } -> std::convertible_to<std::basic_string_view<char>>;
};

class Dbg {
  struct Arg {
    std::stringstream ss;
    template<typename T>
    auto operator <<(const T &t) -> Arg & {
      ss << t;
      ss.flush();
      return *this;
    }
  };
  
  Arg arg;
  public:
  template<typename T, typename ...Args>
  //requires (Stringular<T> || Strangular<T>)
  auto msg(const T &t, const Args &...args) {
    if constexpr (std::is_convertible_v<T, std::string> ||
                    std::is_convertible_v<T, std::string_view>)
      arg << t << " ";
    else
      arg << std::to_string(t) << " ";
    msg(args...);
  }
  
  auto msg(void) {
    std::cout << arg.ss.str() << "\n";
    arg.ss.clear();
  }
};

template<typename ...Args>
auto DBGMSG(const Args &...args) {
  /*static*/ Dbg dbg;
  dbg.msg(args...);
}

dobwm::Box::Box(void) {
  for (const auto &D : x.MONS()) {
    std::vector<Tag> T { Nt };
    Mon m { T, D };
    M.emplace_back(std::move(m));
  }
  
  for (const auto W : x.query_tree()) {
    map_request(W);
    ::DBGMSG("Init w/ client", W);
  }
  
  for (const auto &CMDS_ : { CMDS, CMDS_ASYNC })
    for (const auto &CMD : CMDS_) {
      const Kb &KB { std::get<0>(CMD) };
      x.grab_key(std::get<0>(KB), static_cast<unsigned long>(std::get<1>(KB)));
    }
}

dobwm::Box::~Box(void) {

}

auto dobwm::Box::MSG(std::string_view MSG, const Urg URG, const unsigned TO) {
  msg.send("Dopenbox WM", MSG, URG, TO);
}

auto dobwm::Box::print_hint(const std::pair<std::string, std::string> R) const {
  ::DBGMSG("Hint (class, res).", std::get<0>(R), std::get<1>(R));
}

auto dobwm::Box::focus(Hnd &H) {
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) {
        x.client(H.win, BDR_WIDTH, static_cast<unsigned long>(INACTBDR_COLOR)); });

  if (x.focus(H.win) &&
      x.client(H.win, BDR_WIDTH, static_cast<unsigned long>(ACTBDR_COLOR))) {
    curr = HndRef { std::ref(H) };
    ::DBGMSG("Focus.", curr->get().win);
    if (curr->get().res.has_value())
      print_hint(curr->get().res.value());
  }
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
/*
	if (XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
		for (i = 0; i < num; i++) {
			if (!XGetWindowAttributes(dpy, wins[i], &wa)
			|| wa.override_redirect || XGetTransientForHint(dpy, wins[i], &d1))
				continue;
			if (wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
				manage(wins[i], &wa);
		}
		for (i = 0; i < num; i++) { // now the transients
			if (!XGetWindowAttributes(dpy, wins[i], &wa))
				continue;
			if (XGetTransientForHint(dpy, wins[i], &d1)
			&& (wa.map_state == IsViewable || getstate(wins[i]) == IconicState))
				manage(wins[i], &wa);
		}
		if (wins)
			XFree(wins);
	}
*/
/*
	if (!XGetWindowAttributes(dpy, ev->window, &wa))
		return;
	if (wa.override_redirect)
		return;
	if (!wintoclient(ev->window))
		manage(ev->window, &wa);
*/
auto dobwm::Box::map_request(const ::Window W) -> void {
  ::Window tra { };
  if (const auto D { x.client_dim(W) }; D.has_value() && x.map_window(W)) {
    M.back().T.back().H.emplace_back(Hnd { 
      W, D.value(), true, { }, x.client_hint(W) });
    focus(M.back().T.back().H.back());
    ////
    ::DBGMSG("Mapped.", W);
    if (M.back().T.back().H.back().res.has_value())
      print_hint(M.back().T.back().H.back().res.value());


  } else if (const auto D { x.trans_dim(W) }; D.has_value() &&
      x.client_trans(W, tra) && x.map_window(W) && x.map_window(tra)) {
    M.back().T.back().H.emplace_back(Hnd { W, D.value(), true });
    M.back().T.back().H.emplace_back(Hnd { tra, D.value(), false });
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
  const Kb KB { x.key_state(), static_cast<Key>(x.key_press(x.key_code())) };
  for (const auto &CMD : CMDS)
    if (KB == std::get<0>(CMD)) {
      if (std::get<1>(CMD) == "QUIT") {
          ::DBGMSG("WM exit");
          quit = true;
      } else if (std::get<1>(CMD) == "KILLCLI" && curr.has_value()) {
          ::DBGMSG("Kill Curr");
          const auto W { curr->get().win };
          // Take reverse focus instead
          sw_focus();
          if (x.kill_client(W))
            del_hnd(*curr);
      } else if (std::get<1>(CMD) == "UNMAPALL" && curr.has_value()) {
          ::DBGMSG("Unmap all");
          unmap_all();
      } else if (std::get<1>(CMD) == "REMAPALL" && curr.has_value()) {
          ::DBGMSG("Remap all");
          map_all();
      } else if (std::get<1>(CMD) == "SWFOCUS" && curr.has_value())
          sw_focus();
      else if (std::get<1>(CMD) == "MOVEUP" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x, curr->get().d.y -= MOVESTEP_PX);
      else if (std::get<1>(CMD) == "MOVEDOWN" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x, curr->get().d.y += MOVESTEP_PX);
      else if (std::get<1>(CMD) == "MOVELEFT" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x -= MOVESTEP_PX, curr->get().d.y);
      else if (std::get<1>(CMD) == "MOVERIGHT" && curr.has_value())
        x.move(curr->get().win, curr->get().d.x += MOVESTEP_PX, curr->get().d.y);
      else {
        ::system(std::string(std::get<1>(CMD)).c_str());
        return;
      }
    }
  
  for (const auto &CMD : CMDS_ASYNC)
    if (KB == std::get<0>(CMD)) {
      // fork() on std::get<1>(CMD)
      return;
    }
}

auto dobwm::Box::configure_request(void) {
  ::DBGMSG("Ev.", "Config Req Event");
  auto &ev { x.configure_request() };
  x.configure_window(ev);
}

auto dobwm::Box::unmap_request(void) {
  const auto W { x.unmap_notify() };
  x.unmap_window(W);
}

auto dobwm::Box::cli_msg(void) const {
  ::DBGMSG("Ev.", "Client Msg Event");
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
  /*
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
    ::DBGMSG("Ev.", "Resize button");
  }
  
  for (const auto &M : this->M)
    for (const auto &T : M.T)
      std::ranges::for_each(T.H, [&](const Hnd &H) { 
        x.ungrab_button(H.win, std::get<0>(SELECT),
          static_cast<unsigned>(std::get<1>(SELECT)));
      });
  */
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
    //std::println("...");
    ::DBGMSG("Dopenbox Window Manager ver.", dobwm::VER);
    dobwm::Box box;
    ::DBGMSG("WM", "initialized");
    box.MSG("Welcome msg", dobwm::Urg::NORMAL, 1000);
    while (!quit)
      box.ev();
  } catch (const std::exception &E) {
    ::DBGMSG("Ex.", E.what());
    //std::println("Ex. { }", E.what());
    return -1;
  }
  
  return 0;
}
