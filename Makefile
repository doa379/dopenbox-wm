INCS = -I /usr/X11R6/include \
  -I /usr/include \
  -I /usr/local/include \
  -I /usr/lib/dbus-1.0/include \
  -I /usr/local/lib/dbus-1.0/include \
  -I /usr/include/dbus-1.0 \
  -I /usr/local/include/dbus-1.0
LIBSPATH = -L . -Wl,-R . '-Wl,-R$$ORIGIN' \
  -L /usr/X11R6/lib \
  -L /usr/lib \
  -L /usr/lib64 \
  -L /usr/local/lib

LIBS = -l X11 -l Xinerama -l dbus-1

CPPC = clang++
FLAGS = -std=c++23 -Wall -fPIE -fPIC -pedantic

CFLAGS = -O3
LDFLAGS = -s
EXEC = dopenboxwm

DBG_CFLAGS = -O1 -g -fno-omit-frame-pointer
DBG_LDFLAGS =
DBG_EXEC = dopenboxwm~dbg

SRC = src/main.cpp src/Xlib.cpp src/wm.cpp
OBJ = obj/main.o obj/Xlib.o obj/wm.o

.OBJDIR: $(.CURDIR)

.SUFFIXES: .cpp .o
.POSIX:
all: init $(EXEC)

init:
	@mkdir -p obj

${EXEC}: ${OBJ} config.h
	@echo Linking...
	@$(CPPC) $(FLAGS) $(LIBSPATH) $(LIBS) $(LDFLAGS) ${OBJ} -o $@.bin
	@echo $(EXEC).bin

config.h: $(OBJ)

obj/main.o: src/main.cpp
	@echo Build $? "-->" $@ ...
	@$(CPPC) $(FLAGS) -c $(CFLAGS) $(INCS) $? -o $@

obj/Xlib.o: src/Xlib.cpp
	@echo Build $? "-->" $@ ...
	@$(CPPC) $(FLAGS) -c $(CFLAGS) $(INCS) $? -o $@

obj/wm.o: src/wm.cpp
	@echo Build $? "-->" $@ ...
	@$(CPPC) $(FLAGS) -c $(CFLAGS) $(INCS) $? -o $@

clean:
	@echo Cleaning...
	rm -r obj
	rm -f $(EXEC).bin $(EXEC)~dbg.bin $(DBG_EXEC) *.tmp *.core
