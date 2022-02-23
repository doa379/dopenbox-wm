#pragma once

#include <list>

namespace dopenbox {
  struct Client {

  };

  struct Tag {
    std::list<Client> C;
  };

  struct Mon {
    std::list<Tag> T;
  };

  class Box {
    std::list<Mon> M;
  public:
    Box(const unsigned char, const unsigned char);
  };
}
