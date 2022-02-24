#pragma once

#include <list>
#include <string>

namespace dopenbox {
  struct Client {
    std::string name;
    unsigned x { }, y { }, w { }, h { };
  };

  struct Tag {
    std::list<Client> C;
  };

  struct Mon {
    std::list<Tag> T;
    unsigned w { }, h { };
  };

  class Box {
    std::list<Mon> M;
  public:
    Box(const unsigned char, const unsigned char);
  };
}
