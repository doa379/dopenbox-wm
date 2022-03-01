LOCAL = .
LIBSPATH = -L $(LOCAL)/ -Wl,-R$(LOCAL)/ '-Wl,-R$$ORIGIN' -L /usr/lib64 -L /usr/local/lib
INCS = -I /usr/local/include -I $(LOCAL)/src
LIBS = -l X11 -l Xinerama

CPPC = c++
CPPC_FLAGS = -std=c++17 -Wall -fPIE -fPIC -pedantic
DBG_CFLAGS = -O1 -g -fsanitize=address -fno-omit-frame-pointer
DBG_LDFLAGS = -g -fsanitize=address
REL_CFLAGS = -O3
REL_LDFLAGS = -s

CFLAGS = $(REL_CFLAGS)
LDFLAGS = $(REL_LDFLAGS)

SRC = src/main.cpp src/dobwm.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = dobwm

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
