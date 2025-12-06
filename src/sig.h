#pragma once

#include <csignal>

namespace some {
  class Sig {
    public:
    Sig();
    Sig(Sig const&) = delete;
    ~Sig();
    bool none() const noexcept { return status == 0; }
    private:
    static sig_atomic_t status;
    static void handler(int) {
      status = 1;
      std::cout << "\nSig.\n";
    }
  };

  sig_atomic_t Sig::status;

  Sig::Sig() {
    if (std::signal(SIGINT, handler) == SIG_ERR)
      throw std::runtime_error("Failed to set handler");
  }

  Sig::~Sig() {

  }
}
