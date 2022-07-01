#include <msg.h>
#include <stdexcept>

dobwm::Msg::Msg(void) {
  if (!connection)
    throw std::runtime_error("Unable to init DBus connection");
}

dobwm::Msg::~Msg(void) {
  ::dbus_connection_unref(connection);
}

void dobwm::Msg::send(const char SUMM[], const char BODY[], const Urg urg, const int timeout_ms)
{
  ::DBusMessage *message { ::dbus_message_new_method_call(
      "org.freedesktop.Notifications",
      "/org/freedesktop/Notifications",
      "org.freedesktop.Notifications",
      "Notify") };
  ::DBusMessageIter iter[4];
  ::dbus_message_iter_init_append(message, iter);
  const char *application { "notify_send" };
  ::dbus_message_iter_append_basic(iter, 's', &application);
  unsigned id { };
  ::dbus_message_iter_append_basic(iter, 'u', &id);
  const char *icon { "dialog-information" };
  ::dbus_message_iter_append_basic(iter, 's', &icon);
  
  const char *summary { SUMM };
  ::dbus_message_iter_append_basic(iter, 's', &summary);
  
  const char *body { BODY };
  ::dbus_message_iter_append_basic(iter, 's', &body);
  ::dbus_message_iter_open_container(iter, 'a', "s", iter + 1);
  ::dbus_message_iter_close_container(iter, iter + 1);
  ::dbus_message_iter_open_container(iter, 'a', "{sv}", iter + 1);
  ::dbus_message_iter_open_container(iter + 1, 'e', 0, iter + 2);
  
  const char *urgency { "urgency" };
  ::dbus_message_iter_append_basic(iter + 2, 's', &urgency);
  ::dbus_message_iter_open_container(iter + 2, 'v', "y", iter + 3);
  
  const auto level { static_cast<int>(urg) };
  ::dbus_message_iter_append_basic(iter + 3, 'y', &level);
  ::dbus_message_iter_close_container(iter + 2, iter + 3);
  ::dbus_message_iter_close_container(iter + 1, iter + 2);
  ::dbus_message_iter_close_container(iter, iter + 1);
  
  const auto timeout { timeout_ms };
  ::dbus_message_iter_append_basic(iter, 'i', &timeout);
  ::dbus_connection_send(connection, message, nullptr);
  ::dbus_connection_flush(connection);
  ::dbus_message_unref(message);
}
