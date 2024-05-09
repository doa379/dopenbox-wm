#include <iostream>
#include <csignal>
#include <wm.h>
#include <Xlib.h>

class Sig {
  public:
  Sig();
  Sig(const Sig&) = delete;
  ~Sig();
  bool none() const noexcept { return status == 0; }
  private:
  static volatile sig_atomic_t status;
  static void handler(int) {
    status = 1;
    std::cout << "\nSig.\n";
  }
};

volatile sig_atomic_t Sig::status;

Sig::Sig() {
  if (std::signal(SIGINT, handler) == SIG_ERR)
    throw std::runtime_error("Failed to set handler");
}

Sig::~Sig() {

};

int main(const int ARGC, const char* ARGV[]) {
  try {
    Sig sig;
    some::Display dpy;
    dpy.init();
    
    some::Recv wm;
    some::Ev ev;
    ev.init_key([&wm](const some::data::T& DATA) { 
      wm.key(std::get<some::data::L>(DATA)); });
    ev.init_button([&wm](const some::data::T& DATA) { 
      wm.button(std::get<some::data::L>(DATA)); });
    ev.init_motion([&wm](const some::data::T& DATA) { 
      wm.motion(std::get<some::data::L>(DATA)); });
    ev.init_crossing([&wm](const some::data::T& DATA) { 
      wm.crossing(std::get<some::data::L>(DATA)); });
    ev.init_expose([&wm](const some::data::T& DATA) { 
      wm.expose(std::get<some::data::L>(DATA)); });
    ev.init_unmap([&wm](const some::data::T& DATA) { 
      wm.unmap(std::get<some::data::L>(DATA)); });
    ev.init_map([&wm](const some::data::T& DATA) { 
      wm.map(std::get<some::data::L>(DATA)); });
    ev.init_maprequest([&wm](const some::data::T& DATA) { 
      wm.maprequest(std::get<some::data::L>(DATA)); });
    ev.init_configure([&wm](const some::data::T& DATA) { 
      wm.configure(std::get<some::data::L>(DATA)); });
    ev.init_configurerequest([&wm](const some::data::T& DATA) { 
      wm.configurerequest(std::get<some::data::L>(DATA)); });
    ev.init_property([&wm](const some::data::T& DATA) { 
      wm.property(std::get<some::data::L>(DATA)); });
    ev.init_clientmessage([&wm](const some::data::T& DATA) { 
      wm.clientmessage(std::get<some::data::Msg>(DATA)); });
    while (sig.none()) {
      if (ev.next()) {
        static some::data::T data_var;
        const auto CALL { ev.call(data_var) };
        CALL(data_var);
      }

      ev.sync();
    }

    dpy.deinit();
  } catch (const std::exception& E) {
    std::cerr << E.what() << "\n";
    return -1;
  }

  return 0;
}
