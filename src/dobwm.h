#pragma once

#include <vector>
#include <string>
#include <X11/Xutil.h>

namespace dobwm {
  enum class Mode {
    TRANS,
    FLOAT,
    TILE,
    CASC,
    MONO
  };

  struct Client {
    ::Window win;
    std::string name;
    unsigned x { }, y { }, w { }, h { };
    Mode mode;
  };

  struct Tag {
    std::vector<Client> C;
  };

  struct Mon {
    std::vector<Tag> T;
    unsigned w { }, h { };
  };

  class Dob {
  std::vector<Mon> M;
  public:
    Dob(void);
    void map_request(void);
  };
}
