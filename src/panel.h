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
    Panel(void);
    ~Panel(void);
    void init(::Display *, ::Window, const int);
    void draw(std::string_view);
  };
}
