#include <stddef.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>

int main(const int ARGC, const char *ARGV[]) {
  Display *dpy = XOpenDisplay(NULL);
	Window root = RootWindow(dpy, DefaultScreen(dpy));

  unsigned int i, nw;
	Window root_, parent, *W = NULL;
	XWindowAttributes wa;
	if (XQueryTree(dpy, root, &root_, &parent, &W, &nw)) {
		for (i = 0; i < nw; i++) {
			if (!XGetWindowAttributes(dpy, W[i], &wa)
			|| wa.override_redirect || XGetTransientForHint(dpy, W[i], &root_))
				continue;
			if (wa.map_state == IsViewable)
				printf("Window %ld\n", W[i]);
		}
		for (i = 0; i < nw; i++) {
			if (!XGetWindowAttributes(dpy, W[i], &wa))
				continue;
			if (XGetTransientForHint(dpy, W[i], &root_)
			&& wa.map_state == IsViewable)
				printf("Transient Window %ld\n", W[i]);
		}
		
    if (W)
			XFree(W);
	}

  return 0;
}
