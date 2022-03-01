#include <dobwm.h>

dobwm::Box::Box(const unsigned char Nm, const unsigned char Nt) {
  for (auto i { 0U }; i < Nm; i++) {
    std::list<Tag> T(Nt);
    Mon m { T };
    M.emplace_back(std::move(m));
  }
}
