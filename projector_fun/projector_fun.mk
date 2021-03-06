###############################
PROJNAME = projector_fun
# BIN_OUT = $(BINPATH)/$(PROJNAME)

# INVOCATION = 1

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)../build/linux/asdf_makefile.inc
###############################

############ LIBS #############
LIBS += sdl2 gl glu glew ftgl

LINK_FLAGS += -L$(LIBPATH)

ASDFDIR = /home/mathias/Programming/AsdfMultiplat/obj/asdf_multiplat/linux

# this feels super janky
ASDFASDF = $(ASDFDIR)/asdf_multiplat.o   \
		   $(ASDFDIR)/spritebatch.o      \
		   $(ASDFDIR)/ui_base.o          \
		   $(ASDFDIR)/utilities_openGL.o \
		   $(ASDFDIR)/content_manager.o  \
		   $(ASDFDIR)/shader.o           \
		   $(ASDFDIR)/utilities.o        \
		   $(ASDFDIR)/texture.o        \
		   # $(ASDFDIR)/stdafx.o

LINK_FLAGS += $(ASDFASDF)

# --- WTF ---
# attempt to open ~/Programming/AsdfMultiplat/lib/libasdf.so failed
PKG_LFLAGS += -L$(LIBPATH) \
		      -lSOIL
###############################

########## INCLUDES ###########
INCLUDEPATH = $(PROJPATH)/include

SYSINCLUDES += $(EXTPATH)           \
			   $(EXTPATH)/glm		\

SYSINCLUDES += $(INCLUDEPATH)			\
			   $(INCLUDEPATH)/SOIL

ADSF_MULTIPLAT_SRC = $(PROJPATH)/asdf_multiplat
ASDF_MULTIPLAT_INCLUDES = $(ADSF_MULTIPLAT_SRC)/data 	  \
			  $(ADSF_MULTIPLAT_SRC)/main 	  \
			  $(ADSF_MULTIPLAT_SRC)/ui 	  \
			  $(ADSF_MULTIPLAT_SRC)/utilities

SYSINCLUDES += $(ASDF_MULTIPLAT_INCLUDES)

INCLUDES   += $(SRCPATH)/data 	  \
			  $(SRCPATH)/main 	  \
			  $(SRCPATH)/ui 	  \
			  $(SRCPATH)/utilities
###############################

########### SOURCES ###########
SRCPATH		= $(PROJPATH)/$(PROJNAME)

MAIN_SOURCES = $(SRCPATH)/main/main.cpp 		\
			   $(SRCPATH)/main/stdafx.cpp 		\
			   $(SRCPATH)/main/projector_fun.cpp 		\

DATA_SOURCES  = $(SRCPATH)/data/polygon_state_machine.cpp \
			  	$(SRCPATH)/data/polygon_path.cpp		  \
			  	$(SRCPATH)/data/metaballs.cpp		      \


# UTILITIES_SOURCES = $(UTILITIES_SRC_PATH)/		\

UI_SOURCES = $(SRCPATH)/ui/polygon.cpp

SOURCES += $(MAIN_SOURCES) $(DATA_SOURCES) $(UI_SOURCES)
 #  $(UTILITIES_SOURCES) 

SRC_FOLDERS += data main utilities ui

# DEPENDANCY_MAKEFILES += $(PROJPATH)/asdf_multiplat/asdf_multiplat.mk
###############################

# SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(PROJPATH)/build/linux/asdf_makefile.mk