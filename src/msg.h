#pragma once

#include <string_view>
#include <sstream>
#include <iostream>
#include <dbus-1.0/dbus/dbus.h>

namespace dobwm {
  template<typename T>
  concept Stringular = requires(T& t) {
    { t } -> std::convertible_to<std::basic_string<char>>;
  };

  template<typename T>
  concept Strangular = requires(T& t) {
    { t } -> std::convertible_to<std::basic_string_view<char>>;
  };

  class Dbg {
    struct Arg {
      std::stringstream ss;
      template<typename T>
      Arg& operator<<(const T& t) {
        ss << t;
        ss.flush();
        return *this;
      }
    };
    
    public:
      template<typename T, typename... Args>
      //requires (Stringular<T> || Strangular<T>)
      void msg(const T& t, const Args&... args) {
        if constexpr (std::is_convertible_v<T, std::string> ||
                        std::is_convertible_v<T, std::string_view>)
          arg << t << " ";
        else arg << std::to_string(t) << " ";
        msg(args...);
      }

      void msg() {
        std::cout << arg.ss.str() << "\n";
        arg.ss.clear();
      }

    private:
      Arg arg;
  };
  
  template<typename... Args>
  void DBG(const Args&... args) {
    Dbg dbg;
    dbg.msg(args...);
  }

  enum class Urg : unsigned { LOW, NORMAL, CRITICAL };
  class Msg {
  public:
    Msg();
    ~Msg();
    void send(std::string_view, std::string_view, const Urg, const unsigned)
      const noexcept;
  private:
    static inline ::DBusConnection* connection { 
      ::dbus_bus_get(DBUS_BUS_SESSION, nullptr) };
  };
}
