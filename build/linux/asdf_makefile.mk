SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

# Include
_INCLUDES := $(addprefix -I , $(INCLUDES))
_SYSINCLUDES := $(addprefix -isystem , $(SYSINCLUDES))

# Libs
ifdef LIBS
PKG_CFLAGS += `pkg-config $(LIBS) --cflags`
PKG_LFLAGS += `pkg-config $(LIBS) --libs`
endif

CFLAGS     += $(PKG_CFLAGS)
LINK_FLAGS += $(PKG_LFLAGS)

# Objects
OBJECT_NO_PATH := $(notdir $(addsuffix .o, $(basename $(SOURCES))))
OBJECTS := $(addprefix $(OBJPATH)/, $(OBJECT_NO_PATH))

# $@	name of whichever target caused rule's recipe to run.
# $%	target's member name
# $<	name of first prerequisite
# $^	all prerequisites


#Fancy Colours
ENDCOLOR := '\e[0m'
BLACK	 := '\e[1;30m'
RED	 	 := '\e[1;31m'
GREEN	 := '\e[1;32m'
YELLOW	 := '\e[1;33m'
BLUE	 := '\e[1;34m'
MAGENTA	 := '\e[1;35m'
CYAN	 := '\e[1;36m'
WHITE	 := '\e[1;37m'


ifndef PROJNAME
$(error PROJNAME not defined);
endif
ifndef CC
$(error no C compiler specified);
endif
ifndef CPP
$(error no c++ compiler specified);
endif
ifndef LINK
$(error no Linker specified);
endif

# ifdef EXECUTABLE
# $(error Use BIN_OUT instead of EXECUTABLE)
# endif
# ifndef BIN_OUT
# $(no output filename specified)
# endif
# ifeq ($(suffix $(BIN_OUT)),.so)
# CFLAGS += -fpic
# LINK_FLAGS += -shared -Wl
# $(info Compiling $(BIN_OUT) as shared object)
# endif

ifeq ($(INVOCATION),1)
CFLAGS += -v
LINK_FLAGS += -v
$(info Diplaying link invocation)
endif

ifeq ($(DEBUG),1)
$(warning Todo - Implement DEBUG)
CFLAGS += -DDEBUG
endif

# Make

# if rebuild is the target,  prevent all from initiating
# compiliation before the object files are destroyed
ifeq ($(MAKECMDGOALS), rebuild)
all: clean
endif

all: $(PROJNAME)
	@echo -e '\e[1;32m'----- $(PROJNAME) End ------- $(ENDCOLOR)

# shared: intro $(PROJNAME)_SHARED
# 	@echo -e '\e[1;32m'----- $(PROJNAME) SHARED End ------- $(ENDCOLOR)
# dynamic: shared

# static: intro $(PROJNAME)_STATIC
# 	@echo -e '\e[1;32m'----- $(PROJNAME) STATIC End ------- $(ENDCOLOR)
# archive: static

intro:
	@echo -e '\e[1;32m'----- $(PROJNAME) Start ----- $(ENDCOLOR)
	mkdir -pv $(BINPATH)
	mkdir -pv $(OBJPATH)
	@echo Current Working Dir:
	pwd
	@echo Libs: $(LIBS)
	# @echo Lib Flags: $(PKG_CFLAGS) $(PKG_LFLAGS)
	# @echo LINK_FLAGS: $(LINK_FLAGS)
	# @echo CFLAGS: $(CFLAGS)
	# @echo CPPFLAGS: $(CPPFLAGS)

	# @echo Objs: $(OBJECTS)
	# @echo Srcs: $(SOURCES)
	# @echo Includes: $(_INCLUDES)
	# @echo SysIncludes: $(SYSINCLUDES)
	@echo -e '\e[1;32m'-------------------------------- $(ENDCOLOR)
	@echo -e $(CYAN)--------- COMPILING --------- $(ENDCOLOR)

rebuild: clean all

$(PROJNAME): $(OBJECTS)
	@echo -e $(YELLOW)---------- LINKING  --------- $(ENDCOLOR)
	@$(CPP) $(LINK_FLAGS) $(OBJECTS) -o $(BINPATH)/$(PROJNAME)

# $(PROJNAME)_SHARED: $(OBJECTS)
# 	@echo -e $(YELLOW)---------- LINKING SHARED LIBRARY  --------- $(ENDCOLOR)
# 	@$(CPP) -shared -Wl,-soname,lib$(PROJNAME).so $(LINK_FLAGS) $(OBJECTS) -o $(LIBPATH)/lib$(PROJNAME).so

# $(PROJNAME)_STATIC: $(OBJECTS)
# 	@echo $(PROJNAME) : Creating Static Lib
# 	ar -cr lib$(PROJNAME).a $(OBJECTS)
# 	@mv lib$(PROJNAME).a $(LIBPATH)/lib$(PROJNAME).a

# link:
# 	@echo -e $(YELLOW)---------- LINKING  --------- $(ENDCOLOR)
# 	@echo OBJPATH: $(OBJPATH)
# 	# @echo Libs: $(LIBS)
# 	# @echo Lib Flags: $(PKG_CFLAGS) $(PKG_LFLAGS)
# 	@echo LINK_FLAGS: $(LINK_FLAGS)
	
# 	@$(CPP) $(LINK_FLAGS) $(OBJECTS) -o $(BINPATH)/$(PROJNAME)	

# Clean
clean:
	@echo $(PROJNAME) : Nuking obj folder
	@-\rm $(OBJPATH)/*


.PHONY: all intro rebuild clean

###############################
define BUILD_SHIT

$$(OBJPATH)/%.o: $(SRCPATH)/$(1)/%.c | intro
	@echo -e $$(CYAN)$<$$(ENDCOLOR)
	@$$(CC) -c $$(CFLAGS) -I $$(INCLUDES) $$(SYSINCLUDES) -o $$@ $$<

$$(OBJPATH)/%.o: $(SRCPATH)/$(1)/%.cc | intro
	@echo -e $(CYAN)$<$(ENDCOLOR)
	@$(CC) -c $(CFLAGS) -I $$(INCLUDES) $$(SYSINCLUDES) -o $$@ $$<

$$(OBJPATH)/%.o: $$(SRCPATH)/$(1)/%.cpp | intro
	@echo -e $$(CYAN) $$(CPP) $$(addprefix $$(PROJNAME)/$(1)/,$$(notdir $$<)) $$(ENDCOLOR)
	# use $CPP to compile with $CPPFLAGS $_INCLUDES AND $_SYSINCLUDES
	# output an object file with a name equal to the rule's name (using $@)
	# the file compiled is the name of the first prerequisite (using $<)
	@$$(CPP) -c $$(CPPFLAGS) $$(_INCLUDES) $$(_SYSINCLUDES) -o $$@ $$<

endef

define BUILD_SHIT_WITH
$(foreach src_folder,$(1),$(eval $(call BUILD_SHIT,$(src_folder))))
endef

$(call BUILD_SHIT_WITH, $(SRC_FOLDERS))
###############################
# define MAKE_DEPENDANCY
# %:
# 	@echo $(RED)Dont mind me just doing dependancy things
# 	make -f $@
# endef
# define MAKE_DEPENDANCY_WITH
# $(foreach dependancy,$(1),$(eval $(call MAKE_DEPENDANCY,$(dependancy))))
# endef
# $(call MAKE_DEPENDANCY_WITH, $(DEPENDANCY_MAKEFILES))
###############################

###############################