#include <iostream>
#include <csignal>
#include <variant>
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
    
    using T = some::Data;
    some::Recv<T> wm;
    some::Ev<T> ev;
    ev.init_key([&wm](const T& DATA) { wm.key(DATA); });
    /*
    ev.init_button([&wm](const T& DATA) { wm.button(DATA); });
    ev.init_motion([&wm](const T& DATA) { wm.motion(DATA); });
    ev.init_crossing([&wm](const T& DATA) { wm.crossing(DATA); });
    ev.init_expose([&wm](const T& DATA) { wm.expose(DATA); });
    ev.init_unmap([&wm](const T& DATA) { wm.unmap(DATA); });
    ev.init_map([&wm](const T& DATA) { wm.map(DATA); });
    ev.init_maprequest([&wm](const T& DATA) { wm.maprequest(DATA); });
    ev.init_configure([&wm](const T& DATA) { wm.configure(DATA); });
    ev.init_configurerequest([&wm](const T& DATA) { 
      wm.configurerequest(DATA); });
    ev.init_property([&wm](const T& DATA) { wm.property(DATA); });
    */
    ev.init_clientmessage([&wm](const T& DATA) { wm.clientmessage(DATA); });
    std::variant<long[10], some::MsgData, some::KeymapData> data_var;
    while (sig.none()) {
      if (ev.next()) {
        static T data;
        const auto CALL { ev.call(data) };
        CALL(data);
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
