//#include <print>
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
    some::Wm wm { dpy.ptr };
    some::Ev ev { wm };
    while (sig.none()) {
      if (ev.next(dpy.ptr))
        ev.call();

      ev.sync(dpy.ptr);
    }
  } catch (const std::exception& E) {
    std::cerr << E.what() << "\n";
  }

  return 0;
}
