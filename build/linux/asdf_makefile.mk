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

DEPS_NO_PATH := $(notdir $(addsuffix .d, $(basename $(SOURCES))))
DEPS := $(addprefix $(OBJPATH)/, $(DEPS_NO_PATH))


### GNU MAKE BUILT IN VARIABLES ###
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



ifndef CXX
$(error no c++ compiler specified);
endif
ifndef LINK
$(error no Linker specified);
endif





ifndef BIN_OUT
BIN_OUT = $(BINPATH)/$(PROJNAME)
$(info No output filename specified, using $(BINPATH)/$(PROJNAME))
endif


ifndef TARGET_TYPE

ifeq ($(suffix $(BIN_OUT)),.so)
CFLAGS += -fpic
LINK_FLAGS += -shared -Wl,-soname,$(SO_NAME) -lc
TARGET_TYPE=shared
$(info Compiling $(BIN_OUT) as shared object)
else ifeq ($(suffix $(BIN_OUT)),.a)
TARGET_TYPE=static
$(info Compiling $(BIN_OUT) as static library)
else
TARGET_TYPE=executable
endif

endif



ifeq ($(INVOCATION),1)
CFLAGS += -v
LINK_FLAGS += -v
$(info Diplaying link invocation)
endif


ifeq ($(DEBUG),1)                  				        # if debug is defined in the makefile
ifneq (($findstring debug, $(MAKECMDGOALS)), debug)     # but not a goal passed when calling make
ifneq (($findstring release, $(MAKECMDGOALS)), release) # and release is not a make goal
CPPFLAGS += -DDEBUG -g  		 				        # add debug flags
CFLAGS += -DDEBUG -g    		 					 	#
endif
endif
endif



# if rebuild is the target,  prevent all from initiating
# compiliation before the object files are destroyed
ifeq ($(findstring rebuild, $(MAKECMDGOALS)), rebuild)
$(info Requiring clean to finish before before objects are compiled)
$(OBJECTS): clean
endif


all: $(PROJNAME)
	@echo -e '\e[1;32m'----- $(PROJNAME) End ------- $(ENDCOLOR)



debug: CPPFLAGS += -DDEBUG -g
debug: CFLAGS   += -DDEBUG -g
debug: all

#release: CPPFLAGS += TODO release flags
#release:   CFLAGS += TODO release flags
release: all

intro:
	@echo -e '\e[1;32m'----- $(PROJNAME) Start ----- $(ENDCOLOR)
	@echo -e $(CYAN) Compilers:$(ENDCOLOR) cc:$(CC)   cxx:$(CXX)
	# $(CC) --version
	$(CXX) --version
	@echo
	$(CXX) -v
	@echo
	@echo
	mkdir -pv $(BINPATH)
	mkdir -pv $(OBJPATH)
	@echo 
	@echo Current Working Dir:
	pwd
	printf "\n"
	@echo Libs: $(LIBS)
	@echo Lib Flags: $(PKG_CFLAGS) $(PKG_LFLAGS)
	printf "\n"
	@echo CFLAGS: $(CFLAGS)
	printf "\n"
	@echo CPPFLAGS: $(CPPFLAGS)
	printf "\n"
	@echo LINK_FLAGS: $(LINK_FLAGS)
	printf "\n"

	# @echo Objs: $(OBJECTS)
	# @echo Srcs: $(SOURCES)
	# @echo Includes: $(_INCLUDES)
	@echo SysIncludes: $(SYSINCLUDES)
	@echo -e '\e[1;32m'-------------------------------- $(ENDCOLOR)
	@echo -e $(CYAN)------ GENERATING DEPENDANCIES ------ $(ENDCOLOR)


rebuild: clean all

deps : $(DEPS)
	@echo -e $(CYAN)--------- COMPILING --------- $(ENDCOLOR)

$(PROJNAME): $(OBJECTS)
	@echo -e $(YELLOW)---------- LINKING $(TARGET_TYPE)  --------- $(ENDCOLOR)

	# For whatever reason putting slashes to force everything on one line
	# is the only way for this to not fail
	if [ "$(TARGET_TYPE)" = 'static' ]; then                 \
		echo "Archiving Target $(BIN_OUT)";                  \
		ar rcsv $(BIN_OUT) $(OBJECTS) ;                      \
	else                                                     \
		echo $(CXX) $(OBJECTS) $(LINK_FLAGS) -o $(BIN_OUT);  \
		printf "\nErrors:\n";                                \
		$(CXX) $(OBJECTS) $(LINK_FLAGS) -o $(BIN_OUT);       \
	fi

	@echo -e $(YELLOW)------------------- $(ENDCOLOR)


clean:
	@echo $(PROJNAME) : Nuking obj folder
	@-\rm -rv $(OBJPATH)/*

clean-deps:
	@echo cleaning dependancies
	@-\rm -rv $(OBJPATH)/*.d


.PHONY: all intro rebuild clean clean-deps

###############################
define BUILD_SHIT

# Compiler -c CFlags -I Includes Sysincludes -o objectname sourcefile
$$(OBJPATH)/%.o: $(SRCPATH)/$(1)/%.c | intro deps
	@echo -e $$(CYAN) $$(CC) $<$$(ENDCOLOR)
	@$$(CC) -c $$(CFLAGS) -I $$(_INCLUDES) $$(_SYSINCLUDES) -o $$@ $$<

$$(OBJPATH)/%.o: $(SRCPATH)/$(1)/%.cc | intro deps
	@echo -e $$(CYAN) $$(CC) $<$(ENDCOLOR)
	@$$(CC) -c $(CFLAGS) -I $$(_INCLUDES) $$(_SYSINCLUDES) -o $$@ $$<

$$(OBJPATH)/%.o: $$(SRCPATH)/$(1)/%.cpp | intro deps
	# $(info ${OBJPATH} :: ${SRCPATH} / ${1})
	@echo -e $$(CYAN) $$(CXX) $$(addprefix $$(PROJNAME)/$(1)/,$$(notdir $$<)) $$(ENDCOLOR)
	# use $(CXX) to compile with $CPPFLAGS $_INCLUDES AND $_SYSINCLUDES
	# output an object file with a name equal to the rule's name (using $@)
	# the file compiled is the name of the first prerequisite (using $<)
	@$$(CXX) -c $$(CPPFLAGS) $$(_INCLUDES) $$(_SYSINCLUDES) -o $$@ $$<

$$(OBJPATH)/%.d: $$(SRCPATH)/$(1)/%.c | clean-deps
	@echo TODO: .c deps

$$(OBJPATH)/%.d: $$(SRCPATH)/$(1)/%.cc | clean-deps
	@echo TODO: .cc deps


$$(OBJPATH)/%.d: $$(SRCPATH)/$(1)/%.cpp | clean-deps
	# @echo "deps for $$@"
	# # generate dep file and put in a temp file (%.d.$)
	# $$(CXX) -MT $$@ -MM $$(CPPFLAGS) $$(_INCLUDES) $$(_SYSINCLUDES) $$< > $$@.$$$$
	# # do some sed voodoo to "makes each ‘.d’ file depend on all the source
	# # and header files that the corresponding ‘.o’ file depends on"
	# sed 's,\($$*\)\.o[ :]*,\1.o $$@ : ,g' < $$@.$$$$ > $$@;
	# rm -f $$@.$$$$


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




include $(objects:.o=.d)