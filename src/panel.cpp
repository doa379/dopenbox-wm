#include <panel.h>

dobwm::Panel::Panel(void) {

}

dobwm::Panel::~Panel(void) { 
  ::XFlush(dpy);
  ::XFreeGC(dpy, gc);
  ::XFreePixmap(dpy, drawable); 
}

void dobwm::Panel::init(::Display *dpy, ::Window w, const int SCR) {
  this->dpy = dpy;
  this->w = w;
  scr = SCR;
  size = { DisplayWidth(dpy, SCR), DisplayHeight(dpy, SCR) };
  drawable = ::XCreatePixmap(dpy, w, std::get<0>(size), std::get<1>(size), DefaultDepth(dpy, SCR));
  gc = ::XCreateGC(dpy, w, 0, nullptr);
  ::XSetLineAttributes(dpy, gc, 1, LineSolid, CapButt, JoinMiter);
}

void dobwm::Panel::draw(std::string_view S) {
  ::XSetForeground(dpy, gc, 0xF9F9F9);
  ::XFillRectangle(dpy, w, gc, 0, 0, std::get<0>(size), 10);
  ::XSetForeground(dpy, gc, 0x000000);
  ::XDrawString(dpy, w, gc, 0, 10, H.data(), H.size());
  ::XDrawString(dpy, w, gc, H.size() + 30, 10, T[0].data(), T[0].size());
}
