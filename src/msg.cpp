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
  if (!connection) return;
  ::DBusMessage *message { ::dbus_message_new_method_call(
      "org.freedesktop.Notifications",
      "/org/freedesktop/Notifications",
      "org.freedesktop.Notifications",
      "Notify") };
  ::DBusMessageIter I[4];
  ::dbus_message_iter_init_append(message, I);
  const char *application { "notify_send" };
  ::dbus_message_iter_append_basic(I, 's', &application);
  unsigned id { };
  ::dbus_message_iter_append_basic(I, 'u', &id);
  const char *icon { "dialog-information" };
  ::dbus_message_iter_append_basic(I, 's', &icon);
  
  const char *summary { SUMM };
  ::dbus_message_iter_append_basic(I, 's', &summary);
  
  const char *body { BODY };
  ::dbus_message_iter_append_basic(I, 's', &body);
  ::dbus_message_iter_open_container(I, 'a', "s", I + 1);
  ::dbus_message_iter_close_container(I, I + 1);
  ::dbus_message_iter_open_container(I, 'a', "{sv}", I + 1);
  ::dbus_message_iter_open_container(I + 1, 'e', 0, I + 2);
  
  const char *urgency { "urgency" };
  ::dbus_message_iter_append_basic(I + 2, 's', &urgency);
  ::dbus_message_iter_open_container(I + 2, 'v', "y", I + 3);
  
  const auto level { static_cast<int>(urg) };
  ::dbus_message_iter_append_basic(I + 3, 'y', &level);
  ::dbus_message_iter_close_container(I + 2, I + 3);
  ::dbus_message_iter_close_container(I + 1, I + 2);
  ::dbus_message_iter_close_container(I, I + 1);
  
  const auto timeout { timeout_ms };
  ::dbus_message_iter_append_basic(I, 'i', &timeout);
  ::dbus_connection_send(connection, message, nullptr);
  ::dbus_connection_flush(connection);
  ::dbus_message_unref(message);
}
