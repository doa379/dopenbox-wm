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

    using T = long[8];
    some::Recv<T> wm;
    T data;
    some::Ev<T> ev;
    ev.init_mapnotify([&wm](T& data) { wm.mapnotify(data); });
    ev.init_unmapnotify([&wm](T& data) { wm.unmapnotify(data); });
    ev.init_clientmessage([&wm](T& data) { wm.clientmessage(data); });
    ev.init_configurenotify([&wm](T& data) { wm.configurenotify(data); });
    ev.init_maprequest([&wm](T& data) { wm.maprequest(data); });
    ev.init_configurerequest([&wm](T& data) { wm.configurerequest(data); });
    ev.init_motionnotify([&wm](T& data) { wm.motionnotify(data); });
    ev.init_keypress([&wm](T& data) { wm.keypress(data); });
    ev.init_btnpress([&wm](T& data) { wm.btnpress(data); });
    ev.init_enternotify([&wm](T& data) { wm.enternotify(data); });
    ev.init_propertynotify([&wm](T& data) { wm.propertynotify(data); });
    ev.init_expose([&wm](T& data) { wm.expose(data); });
    while (sig.none()) {
      if (ev.next()) {
        const auto F { ev.call(data) };
        F(data);
      }

      ev.sync();
    }

    dpy.deinit();
  } catch (const std::exception& E) {
    std::cerr << E.what() << "\n";
  }

  return 0;
}
