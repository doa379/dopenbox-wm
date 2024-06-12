#include <iostream>
#include <csignal>

#include "../inc/wm.h"
#include "../inc/Xlib.h"

class Sig {
  public:
  Sig();
  Sig(Sig const&) = delete;
  ~Sig();
  bool none() const noexcept { return status == 0; }
  private:
  static sig_atomic_t volatile status;
  static void handler(int) {
    status = 1;
    std::cout << "\nSig.\n";
  }
};

sig_atomic_t volatile Sig::status;

Sig::Sig() {
  if (std::signal(SIGINT, handler) == SIG_ERR)
    throw std::runtime_error("Failed to set handler");
}

Sig::~Sig() {

}

int
main(int const argc, char const* ARGV[]) {
  try {
    some::Display const dpy;
    dpy.init();
    Sig const sig;
    some::Ev ev;
    some::xlib::DefaultXError const error; 
    {
      some::xlib::Input const input;
      some::xlib::Xlib const xlib;
      input.select(xlib.root_win(), 
        some::xlib::mask::SUBSTRUCTREDIR);
      ev.sync();
      if (error.get())
        throw std::runtime_error(
          "Initialization error (another wm running?)");
    }

    some::Recv wm;
    ev.init_key([&wm](some::Data const& data) { 
      wm.key(data); });
    ev.init_button_press([&wm](some::Data const& data) { 
      wm.button_press(data); });
    //ev.init_button_release([&wm](some::Data const& data) { 
      //wm.button_release(data); });
    ev.init_motion([&wm](some::Data const& data) { 
      wm.motion(data); });
    ev.init_crossing([&wm](some::Data const& data) { 
      wm.crossing(data); });
    ev.init_expose([&wm](some::Data const& data) { 
      wm.expose(data); });
    ev.init_unmap([&wm](some::Data const& data) { 
      wm.unmap(data); });
    ev.init_map([&wm](some::Data const& data) { 
      wm.map(data); });
    ev.init_maprequest([&wm](some::Data const& data) { 
      wm.maprequest(data); });
    ev.init_configure([&wm](some::Data const& data) { 
      wm.configure(data); });
    ev.init_configurerequest([&wm](some::Data const& data) { 
      wm.configurerequest(data); });
    ev.init_property([&wm](some::Data const& data) { 
      wm.property(data); });
    ev.init_clientmessage([&wm](some::Data const& data) { 
      wm.clientmessage(data); });

    while (sig.none()) {
      if (ev.next())
        ev.call();

      ev.sync();
    }

    wm.exit();
    dpy.deinit();
  } catch (std::exception const& ex) {
    std::cerr << ex.what() << "\n";
    return -1;
  }

  return 0;
}
