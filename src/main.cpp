#include <iostream>
#include <csignal>

#include "../inc/wm.h"
#include "../inc/Xlib.h"

class Sig {
  public:
  Sig();
  Sig(Sig const&) = delete;
  ~Sig();
  bool none() const noexcept { return status == 0; }
  private:
  static sig_atomic_t volatile status;
  static void handler(int) {
    status = 1;
    std::cout << "\nSig.\n";
  }
};

sig_atomic_t volatile Sig::status;

Sig::Sig() {
  if (std::signal(SIGINT, handler) == SIG_ERR)
    throw std::runtime_error("Failed to set handler");
}

Sig::~Sig() {

};

int
main(int const argc, char const* ARGV[]) {
  try {
    Sig sig;
    some::Display dpy;
    dpy.init();
    
    some::Recv wm;
    some::Ev ev;
    ev.init_key([&wm](some::data::T const& data) { 
      wm.key(std::get<some::data::L>(data)); });
    ev.init_button([&wm](some::data::T const& data) { 
      wm.button(std::get<some::data::L>(data)); });
    ev.init_motion([&wm](some::data::T const& data) { 
      wm.motion(std::get<some::data::L>(data)); });
    ev.init_crossing([&wm](some::data::T const& data) { 
      wm.crossing(std::get<some::data::L>(data)); });
    ev.init_expose([&wm](some::data::T const& data) { 
      wm.expose(std::get<some::data::L>(data)); });
    ev.init_unmap([&wm](some::data::T const& data) { 
      wm.unmap(std::get<some::data::L>(data)); });
    ev.init_map([&wm](some::data::T const& data) { 
      wm.map(std::get<some::data::L>(data)); });
    ev.init_maprequest([&wm](some::data::T const& data) { 
      wm.maprequest(std::get<some::data::L>(data)); });
    ev.init_configure([&wm](some::data::T const& data) { 
      wm.configure(std::get<some::data::L>(data)); });
    ev.init_configurerequest([&wm](some::data::T const& data) { 
      wm.configurerequest(std::get<some::data::L>(data)); });
    ev.init_property([&wm](some::data::T const& data) { 
      wm.property(std::get<some::data::L>(data)); });
    ev.init_clientmessage([&wm](some::data::T const& data) { 
      wm.clientmessage(std::get<some::data::Msg>(data)); });
    while (sig.none()) {
      if (ev.next()) {
        static some::data::T data_var;
        auto const call { ev.call(data_var) };
        call(data_var);
      }

      ev.sync();
    }

    dpy.deinit();
  } catch (std::exception const& ex) {
    std::cerr << ex.what() << "\n";
    return -1;
  }

  return 0;
}
