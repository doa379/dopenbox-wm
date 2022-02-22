LOCAL = .
LIBSPATH = -L $(LOCAL)/ -Wl,-R$(LOCAL)/ '-Wl,-R$$ORIGIN' -L /usr/lib64 -L /usr/local/lib
INCS = -I /usr/local/include -I $(LOCAL)/
LIBS =

CPPC = c++
CPPC_FLAGS = -std=c++17 -Wall -fPIE -fPIC -pedantic -c
DBG_CFLAGS = -O1 -g -fsanitize=address -fno-omit-frame-pointer
DBG_LDFLAGS = -g -fsanitize=address
REL_CFLAGS = -O3
REL_LDFLAGS = -s

SRC_DOPENBOX = main.cpp dopenbox.cpp
OBJ_DOPENBOX = $(SRC_DOPENBOX:.cpp=.o)

define compile_dbg
	@echo CPPC $(1) "-->" $(basename $(1)).o
	@$(CPPC) $(CPPC_FLAGS) $(DBG_CFLAGS) $(INCS) $(1) -o $(basename $(1)).o

endef

define compile_rel
	@echo CPPC $(1) "-->" $(basename $(1)).o
  @$(CPPC) $(CPPC_FLAGS) $(REL_CFLAGS) $(INCS) $(1) -o $(basename $(1)).o

endef

all: dopenbox

dopenbox:
	@echo "Building Dopenbox Release"
	$(foreach src, $(SRC_DOPENBOX), $(call compile_rel, $(src)))
	@echo "Linking"
	@${CPPC} -o $@ $(OBJ_DOPENBOX) $(REL_LDFLAGS) $(LIBSPATH) $(LIBS)
	@echo $@

dopenbox_dbg:
	@echo "Building Dopenbox Debug"
	$(foreach src, $(SRC_DOPENBOX), $(call compile_dbg, $(src)))
	@echo "Linking"
	@${CPPC} -o $@ $(OBJ_DOPENBOX) $(DBG_LDFLAGS) $(LIBSPATH) $(LIBS)
	@echo $@

clean:
	@echo Cleaning
	@rm -f $(OBJ_DOPENBOX)
	@rm -f dopenbox dopenbox_dbg *.tmp *.core
