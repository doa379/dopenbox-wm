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

    using T = some::Data;
    some::Recv<T> wm;
    some::Ev<T> ev;
    ev.init_key([&wm](const T& data) { wm.key(data); });
    ev.init_button([&wm](const T& data) { wm.button(data); });
    ev.init_motion([&wm](const T& data) { wm.motion(data); });
    ev.init_crossing([&wm](const T& data) { wm.crossing(data); });
    ev.init_expose([&wm](const T& data) { wm.expose(data); });
    ev.init_unmap([&wm](const T& data) { wm.unmap(data); });
    ev.init_map([&wm](const T& data) { wm.map(data); });
    ev.init_maprequest([&wm](const T& data) { wm.maprequest(data); });
    ev.init_configure([&wm](const T& data) { wm.configure(data); });
    ev.init_configurerequest([&wm](const T& data) { 
      wm.configurerequest(data); });
    ev.init_property([&wm](const T& data) { wm.property(data); });
    ev.init_clientmessage([&wm](const T& data) { wm.clientmessage(data); });
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
  }

  return 0;
}
