#pragma once

namespace some {
  enum evid {
    MAPNOTIFY,
  };


  struct Data {
    enum evid ID;
    std::function<void()> f;
  };
}
