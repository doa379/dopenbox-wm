#include <msg.h>
#include <stdexcept>

dobwm::Msg::Msg(void) {
  if (!connection)
    throw std::runtime_error("Unable to init DBus connection");
}

dobwm::Msg::~Msg(void) {
  ::dbus_connection_unref(connection);
}

auto dobwm::Msg::send(const std::string_view SMRY, const std::string_view BODY, const Urg URG, const unsigned TOMS) -> void
{
  if (!connection)
    return;

  ::DBusMessage *message { ::dbus_message_new_method_call(
      "org.freedesktop.Notifications",
      "/org/freedesktop/Notifications",
      "org.freedesktop.Notifications",
      "Notify") };
  ::DBusMessageIter I[4];
  ::dbus_message_iter_init_append(message, I);
  const char *APP { "notify_send" };
  ::dbus_message_iter_append_basic(I, 's', &APP);
  unsigned id { };
  ::dbus_message_iter_append_basic(I, 'u', &id);
  const char *ICO { "dialog-information" };
  ::dbus_message_iter_append_basic(I, 's', &ICO);
  
  const char *S { SMRY.data() };
  ::dbus_message_iter_append_basic(I, 's', &S);
  const char *B { BODY.data() };
  ::dbus_message_iter_append_basic(I, 's', &B);
  ::dbus_message_iter_open_container(I, 'a', "s", I + 1);
  ::dbus_message_iter_close_container(I, I + 1);
  ::dbus_message_iter_open_container(I, 'a', "{sv}", I + 1);
  ::dbus_message_iter_open_container(I + 1, 'e', 0, I + 2);
  const char *URGENCY { "urgency" };
  ::dbus_message_iter_append_basic(I + 2, 's', &URGENCY);
  ::dbus_message_iter_open_container(I + 2, 'v', "y", I + 3);
  const auto URGV { static_cast<unsigned>(URG) };
  ::dbus_message_iter_append_basic(I + 3, 'y', &URGV);
  ::dbus_message_iter_close_container(I + 2, I + 3);
  ::dbus_message_iter_close_container(I + 1, I + 2);
  ::dbus_message_iter_close_container(I, I + 1);
  const auto TO { TOMS };
  ::dbus_message_iter_append_basic(I, 'i', &TO);
  ::dbus_connection_send(connection, message, nullptr);
  ::dbus_connection_flush(connection);
  ::dbus_message_unref(message);
}
