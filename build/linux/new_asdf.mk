###################
### DEFINITIONS ###
###################
THIS_FILE := $(lastword $(MAKEFILE_LIST))
SELF_DIR := $(dir $(THIS_FILE))

#Fancy Colours
ENDCOLOR := '\e[0m'
BLACK    := '\e[1;30m'
RED      := '\e[1;31m'
GREEN    := '\e[1;32m'
YELLOW   := '\e[1;33m'
BLUE     := '\e[1;34m'
MAGENTA  := '\e[1;35m'
CYAN     := '\e[1;36m'
WHITE    := '\e[1;37m'


# $@	name of whichever target caused rule's recipe to run.
# $%	target's member name
# $<	name of first prerequisite
# $^	all prerequisites
# %     wildcard


######################
### ERROR CHECKING ###
# Check for PROJNAME variable
# Check for Compiler variables CC, CXX, and LINK
######################
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


###################
### TARGET TYPE ###
# Set a default BIN_OUT variable
# For static and shared_object builds:
#    add filename suffixes (.a or .so)
#    add relevant flags
# Set the TARGET_TYPE variable, which is used in the link step 
###################
ifndef BIN_OUT
BIN_OUT = $(BINPATH)/$(PROJNAME)
#$(info No output filename specified, using $(BINPATH)/$(PROJNAME))
endif


ifndef TARGET_TYPE

ifeq ($(suffix $(BIN_OUT)),.so)
CFLAGS += -fpic -g
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


##################################
### INCLUDES and LIBRARY FLAGS ###
##################################

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


####################
### OBJECT FILES ###
# strip out the $SRCPATH from each item in $SOURCES and replace
# all slashes with underscores to get the object base name
# also strip off suffix and append '.o'

OBJECTS_SUFFIXED := $(addsuffix .o, $(basename $(SOURCES)))
# OBJECTS_BASE_PATH := $(subst /,_,$(subst $(SRCPATH),,$(OBJECTS_SUFFIXED)))
# OBJECTS_TO_LINK := $(addprefix $(OBJPATH)/, $(OBJECTS_BASE_PATH))


# OBJECT_NO_PATH := $(notdir $(addsuffix .o, $(basename $(SOURCES))))
OBJECTS_PARTIAL_PATH := $(subst $(SRCPATH),,$(OBJECTS_SUFFIXED))
OBJECTS := $(addprefix $(OBJPATH)/, $(OBJECTS_PARTIAL_PATH))


#############
### STUFF ###
#############
# if rebuild is the target,  prevent all from initiating
# compiliation before the object files are destroyed
ifeq ($(MAKECMDGOALS), rebuild)
$(info Requiring clean to finish before before objects are compiled)
$(OBJECTS): clean
endif


all: $(PROJNAME)
	@echo -e '\e[1;32m'----- $(PROJNAME) End ------- $(ENDCOLOR)


intro:
	@echo -e '\e[1;32m'----- $(PROJNAME) Start ----- $(ENDCOLOR)
	@echo -e $(CYAN) Compilers:$(ENDCOLOR) cc:$(CC)   cxx:$(CXX)
	# $(CC) --version
	$(CXX) --version
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
	# @echo LINK_FLAGS: $(LINK_FLAGS)

	@echo -e '\e[1;32m'-------------------------------- $(ENDCOLOR)
	@echo -e $(CYAN)--------- COMPILING --------- $(ENDCOLOR)


rebuild: clean all

objects:
	@echo $(OBJECTS)

sources:
	@echo $(SOURCES)

includes:
	@echo $(SYSINCLUDES)
	@echo 
	@echo 
	@echo $(_INCLUDES)


# $(info $(OBJECTS))
$(PROJNAME): $(OBJECTS)
	@echo -e $(YELLOW)---------- LINKING $(TARGET_TYPE)  --------- $(ENDCOLOR)

	# For whatever reason putting slashes to force everything on one line
	# is the only way for this to not fail
	if [ "$(TARGET_TYPE)" = 'static' ]; then                 \
	    echo "Archiving Target $(BIN_OUT)";                  \
	    ar rcsv $(BIN_OUT) $(OBJECTS) ;                      \
	else                                                     \
	    echo $(CXX) $(LINK_FLAGS) $(OBJECTS) -o $(BIN_OUT);  \
	    printf "\nErrors:\n";                \
	    $(CXX) $(LINK_FLAGS) $(OBJECTS) -o $(BIN_OUT);       \
	fi

	@echo -e $(YELLOW)------------------- $(ENDCOLOR)


clean:
	@echo $(PROJNAME) : Nuking obj folder
	@-\rm $(OBJPATH)/*


.PHONY: all intro rebuild clean objects sources includes


# ## TEST ##
# asdf_multiplat//obj/linux//main/main.o: asdf_multiplat//main/main.cpp
# 	@echo -e $(CYAN) $(CXX) $< $(ENDCOLOR)

# asdf_multiplat//obj/linux//%.o: asdf_multiplat/%.cpp
# 	@echo -e $(CYAN) $(CXX) $< $(ENDCOLOR)	

# asdf_multiplat//obj/linux//main/%.o: asdf_multiplat/main/%.cpp
# 	@echo -e $(CYAN) $(CXX) $< $(ENDCOLOR)	
# ##

###################################
### WILDCARDED COMPILE FUNCTION ###
###################################

# $$(addprefix $$(addprefix $$(OBJPATH)/,$$(subst /,_,$(1))), %.o): $$(SRCPATH)/$(1)/%.cpp | intro

define COMPILE_FILES =

# $(1) represents a src_folder passed in from the 
# foreach in COMPILE_FILES_IN_FOLDERS

# Compiler -c CFlags -I Includes Sysincludes -o objectname sourcefile
# $(OBJPATH)/$(1)/%.o: $(SRCPATH)/$(1)/%.c | intro
# 	@echo -e $$(CYAN) $$(CC) $<$$(ENDCOLOR)
# 	@$$(CC) -c $$(CFLAGS) -I $$(_INCLUDES) $$(_SYSINCLUDES) -o $$@ $$<

# $(OBJPATH)/$(1)/%.o: $(SRCPATH)/$(1)/%.cc | intro
# 	@echo -e $$(CYAN) $$(CC) $<$(ENDCOLOR)
# 	@$$(CC) -c $(CFLAGS) -I $$(_INCLUDES) $$(_SYSINCLUDES) -o $$@ $$<

$$(OBJPATH)/$(1)/%.o: $$(SRCPATH)/$(1)/%.cpp | intro
	@echo -e $$(CYAN) $$(CXX) $$< $$(ENDCOLOR)
	#long convoluted way of adding a src_folder prefix to the object name
	#@echo $$(addprefix $$(dir $$@),$(addprefix $$(subst /,_,$(1))_, $$(notdir $$@)))
	@$$(CXX) -c $$(CPPFLAGS) $$(_INCLUDES) $$(_SYSINCLUDES) $$< -o $$(addprefix $$(dir $$@),$(addprefix $$(subst /,_,$(1))_, $$(notdir $$@)))

endef

#for each src_folder in a given list $(1), call COMPILE_FILES with that src_folder
# define COMPILE_FILES_IN_FOLDERS
# $(foreach src_folder,$(1),$(eval $(call COMPILE_FILES,$(src_folder))))
# endef

#$(call COMPILE_FILES_IN_FOLDERS, $(SRC_FOLDERS))

define COMPILE_FILES_BETTER =
$$(OBJPATH)/$(1)/%.o: $(1)/%.cpp
endef

$(foreach src_folder,$(SRC_FOLDERS),$(eval $(call COMPILE_FILES,$(src_folder))))