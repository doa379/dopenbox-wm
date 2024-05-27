INCS=-I /usr/X11R6/include \
  -I /usr/include \
  -I /usr/local/include \
  -I /usr/lib/dbus-1.0/include \
  -I /usr/local/lib/dbus-1.0/include \
  -I /usr/include/dbus-1.0 \
  -I /usr/local/include/dbus-1.0
LIBSPATH= -L . -Wl,-R . '-Wl,-R$$ORIGIN' \
  -L /usr/X11R6/lib \
  -L /usr/lib \
  -L /usr/lib64 \
  -L /usr/local/lib

LIBS=-l X11 -l Xinerama -l dbus-1

CPPC=clang++
FLAGS=-std=c++20 -Wall -fPIE -fPIC -pedantic

REL_CFLAGS=-O3
DBG_CFLAGS=-O1 -g -fno-omit-frame-pointer
REL_LFLAGS=-s
DBG_LFLAGS=
REL=dopenboxwm.bin
DBG=dopenboxwm~dbg.bin

SRC=main.cpp Xlib.cpp wm.cpp
