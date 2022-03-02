LOCAL = .
LIBSPATH = -L $(LOCAL)/ -Wl,-R$(LOCAL)/ '-Wl,-R$$ORIGIN' -L /usr/lib64 -L /usr/local/lib
INCS = -I /usr/local/include -I $(LOCAL)/src
LIBS = -l X11 -l Xinerama

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
  EXEC = dobwm_dbg
.endif

SRC = src/xlib.cpp src/dobwm.cpp
OBJ = $(SRC:.cpp=.o)

.POSIX:
all: $(EXEC)

clean:
	@echo Cleaning...
	@rm -f $(OBJ)
	@rm -f $(EXEC).bin $(EXEC)_dbg.bin *.tmp *.core

$(EXEC): $(OBJ)
	@echo Linking...
	@$(CPPC) $(CPPC_FLAGS) $(LIBSPATH) $(LIBS) $(LDFLAGS) $(OBJ) -o $@.bin
	@echo $(EXEC).bin

.SUFFIXES: .cpp .o
.cpp.o:
	@echo CPPC $< "-->" $@
	@$(CPPC) $(CPPC_FLAGS) -c $(CFLAGS) $(INCS) $< -o $@
