#include <iostream>
#include <wm.h>
#include <../config.h>

bool some::Wm::xerror;

some::Wm::Wm() : rootw { xlib.root() } {
  xlib.set_err(handler);
  input.select(rootw, Xlib::ROOTMASK);
  if (xerror)
    throw std::runtime_error("Initialization error (another wm running?)");

  input.ungrab_allkey(rootw);
  const auto MASK { input.modmask() };
  for (const auto& K : KBD)
    input.grab_key(rootw, K.mod & MASK, K.key);
    
  CALL[WK0] = [] { };
  CALL[WK1] = [] { };
  CALL[WK2] = [] { };
  CALL[WK3] = [] { };
  CALL[WK4] = [] { };
  CALL[WK5] = [] { };
  CALL[WK6] = [] { };
  CALL[WK7] = [] { };
  CALL[WK8] = [] { };
  CALL[WK9] = [] { };
  CALL[MON0] = [] { };
  CALL[MON1] = [] { };
  CALL[MON2] = [] { };
  CALL[MON3] = [] { };
  CALL[MON4] = [] { };
  CALL[MON5] = [] { };
  CALL[MON6] = [] { };
  CALL[MON7] = [] { };
  CALL[MON8] = [] { };
  CALL[MON9] = [] { };
  CALL[UNMAPALL] = [] { };
  CALL[REMAPALL] = [] { };
  CALL[KILL] = [] { };
  CALL[SWFOCUS] = [] { };
  CALL[TOGGLEMODE] = [] { };
  CALL[PREVCLI] = [] { };
  CALL[NEXTCLI] = [] { };
  CALL[SELTOGGLE] = [] { };
  CALL[SELCLEAR] = [] { };
  CALL[MOVEUP] = [] { };
  CALL[MOVEDOWN] = [] { };
  CALL[MOVELEFT] = [] { };
  CALL[MOVERIGHT] = [] { };
  CALL[RESIZEVINC] = [] { };
  CALL[RESIZEVDEC] = [] { };
  CALL[RESIZEHDEC] = [] { };
  CALL[RESIZEHINC] = [] { };
  CALL[QUIT] = []{ };
  CALL[SELECT] = [] { };
  CALL[RESIZE] = [] { };
  CALL[STATE] = [] { };

  some::Xlib::QueryTree query { rootw };
  const auto W { query.get() };
  for (const auto& W_ : W)
    ;

  some::Xlib::Xinerama xinerama;
}

some::Wm::~Wm() {
  input.set_focus(rootw);
}

void some::Wm::mapnotify() {

}

void some::Wm::unmapnotify() {

}

void some::Wm::clientmessage() {

}

void some::Wm::configurenotify(const ::Window W, const int WIDTH, 
const int HEIGHT) {
  if (W == rootw) {

  }
}

void
some::Wm::maprequest(const ::Window W, const int WIDTH, const int HEIGHT) {

}

void some::Wm::configurerequest() {

}

void some::Wm::motionnotify() {

}

void some::Wm::keypress() {

}

void some::Wm::btnpress(const ::Window W, const int STATE, const int CODE) {
  // ungrab_pointer();
}

void some::Wm::enternotify() {

}

void some::Wm::propertynotify() {

}

void some::Wm::expose() {

}
