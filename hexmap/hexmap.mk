###############################
PROJNAME = hexmap
# BIN_OUT = $(BINPATH)/$(PROJNAME)

# INVOCATION = 1

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))

PROJPATH = .

include $(SELF_DIR)../build/linux/asdf_makefile.inc

BINPATH = $(PROJPATH)/$(PROJNAME)/bin/linux/x86
###############################

############ LIBS #############
LIBS += sdl2 gl glew ftgl

LINK_FLAGS += -L$(LIBPATH)

ASDFDIR = $(PROJPATH)/obj/asdf_multiplat/linux

# this feels super janky
ASDFASDF = $(ASDFDIR)/asdf_multiplat.o          \
		   $(ASDFDIR)/spritebatch.o             \
		   $(ASDFDIR)/ui_base.o                 \
		   $(ASDFDIR)/content_manager.o         \
		   $(ASDFDIR)/shader.o                  \
		   $(ASDFDIR)/polygon.o                 \
		   $(ASDFDIR)/gl_state.o                \
		   $(ASDFDIR)/gl_resources.o            \
		   $(ASDFDIR)/utilities.o               \
		   $(ASDFDIR)/camera.o                  \
		   $(ASDFDIR)/texture.o                 \
		   $(ASDFDIR)/input_controller.o        \
		   $(ASDFDIR)/keyboard.o                \
		   $(ASDFDIR)/mouse.o                   \
		   $(ASDFDIR)/texture_bank.o            \
		   $(ASDFDIR)/texture_atlas.o           \
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
			   $(EXTPATH)/cJSON		\

SYSINCLUDES += $(INCLUDEPATH)			\
			   $(INCLUDEPATH)/SOIL

ADSF_MULTIPLAT_SRC = $(PROJPATH)/asdf_multiplat
ASDF_MULTIPLAT_INCLUDES = $(ADSF_MULTIPLAT_SRC)/data 	  \
			  $(ADSF_MULTIPLAT_SRC)/main 	  \
			  $(ADSF_MULTIPLAT_SRC)/ui 	  \
			  $(ADSF_MULTIPLAT_SRC)/utilities

SYSINCLUDES += $(PROJPATH) \
			   $(ADSF_MULTIPLAT_SRC) \
		       $(ASDF_MULTIPLAT_INCLUDES)

INCLUDES   += $(SRCPATH)          \
			  $(SRCPATH)/data 	  \
			  $(SRCPATH)/main 	  \
			  $(SRCPATH)/ui 	  \
			  $(SRCPATH)/utilities
###############################

########### SOURCES ###########
SRCPATH		= $(PROJPATH)/$(PROJNAME)/src

SOURCES = $(SRCPATH)/main/main.cpp \
		  $(SRCPATH)/main/stdafx.cpp \
		  $(SRCPATH)/main/hexmap.cpp \
		  $(SRCPATH)/main/hexagon_input.cpp \
          $(SRCPATH)/data/hex_grid.cpp \
          $(SRCPATH)/ui/hex_map.cpp \
          $(SRCPATH)/ui/hex_tile.cpp \
          $(SRCPATH)/editor/main/editor.cpp \
          $(SRCPATH)/editor/main/input.cpp

EXT_SOURCES = $(EXTPATH)/cJSON/cJSON.c

SOURCES += $(EXT_SOURCES)

SRC_FOLDERS += main data ui editor/main ../../ext/cJSON
###############################

# SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(PROJPATH)/build/linux/asdf_makefile.mk