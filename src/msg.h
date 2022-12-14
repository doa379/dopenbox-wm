#pragma once

#include <dbus-1.0/dbus/dbus.h>
#include <string_view>

namespace dobwm {
  enum class Urg : unsigned { LOW, NORMAL, CRITICAL };
  class Msg {
    ::DBusConnection *connection { ::dbus_bus_get(DBUS_BUS_SESSION, nullptr) };
  public:
    Msg(void);
    ~Msg(void);
    auto send(const std::string_view, const std::string_view, const Urg, const unsigned) -> void;
  };
}
