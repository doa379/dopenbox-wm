#include <string>
#include <string_view>
#include <vector>
#include <X11/Xutil.h>

namespace dobwm {
  class Panel {
    const std::string_view H { "HOST" };
    std::vector<std::string_view> T { "[1]", "[2]" };
    ::Display *dpy;
    ::Window w;
    int scr;
    std::pair<unsigned, unsigned> size;
    ::Drawable drawable;
    ::GC gc;
    std::vector<std::string> S;
public:
    Panel(::Display *, ::Window, const int);
    ~Panel(void);
    void draw(std::string_view);
  };
}
