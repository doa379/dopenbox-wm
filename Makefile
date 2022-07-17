LOCAL = .
INCS = -I /usr/X11R6/include \
  -I /usr/include \
  -I /usr/local/include \
  -I /usr/lib/dbus-1.0/include \
  -I /usr/local/lib/dbus-1.0/include \
  -I /usr/include/dbus-1.0 \
  -I /usr/local/include/dbus-1.0 \
  -I $(LOCAL)/src
LIBSPATH = -L $(LOCAL)/ -Wl,-R$(LOCAL)/ '-Wl,-R$$ORIGIN' \
  -L /usr/X11R6/lib \
  -L /usr/lib \
  -L /usr/lib64 \
  -L /usr/local/lib

LIBS = -l X11 -l Xinerama -l dbus-1

CPPC = c++
CPPC_FLAGS = -std=c++17 -Wall -fPIE -fPIC -pedantic
REL_CFLAGS = -O3
REL_LDFLAGS = -s
#DBG_CFLAGS = -O1 -g -fsanitize=address -fno-omit-frame-pointer
#DBG_LDFLAGS = -g -fsanitize=address
DBG_CFLAGS = -O1 -g -fno-omit-frame-pointer
DBG_LDFLAGS =

CFLAGS = $(REL_CFLAGS)
LDFLAGS = $(REL_LDFLAGS)
EXEC = dobwm

.if "$(DEBUG)" == "1"
  CFLAGS = $(DBG_CFLAGS)
  LDFLAGS = $(DBG_LDFLAGS)
  EXEC = dobwm~dbg
.endif

SRC = src/dobwm.cpp src/x.cpp src/msg.cpp
OBJ = $(SRC:.cpp=.o)

.POSIX:
all: $(EXEC)

clean:
	@echo Cleaning...
	rm -f $(OBJ)
	rm -f $(EXEC).bin $(EXEC)~dbg.bin *.tmp *.core

$(EXEC): $(OBJ)
	@echo Linking...
	@$(CPPC) $(CPPC_FLAGS) $(LIBSPATH) $(LIBS) $(LDFLAGS) $(OBJ) -o $@.bin
	@echo $(EXEC).bin

.SUFFIXES: .cpp .o
.cpp.o:
	@echo CPPC $< "-->" $@
	@$(CPPC) $(CPPC_FLAGS) -c $(CFLAGS) $(INCS) $< -o $@
