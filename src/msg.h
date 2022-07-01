#pragma once

#include <dbus-1.0/dbus/dbus.h>
#include <memory>

namespace dobwm {
  enum class Urg { LOW, NORMAL, CRITICAL };
  class Msg {
    ::DBusConnection *connection { ::dbus_bus_get(DBUS_BUS_SESSION, nullptr) };
  public:
    Msg(void);
    ~Msg(void);
    void send(const char [], const char [], const Urg, const int);
  };
}
