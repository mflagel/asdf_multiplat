###############################
PROJNAME = hexmap
# BIN_OUT = $(BINPATH)/$(PROJNAME)

# INVOCATION = 1
DEBUG = 1

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))

PROJPATH = .

include $(SELF_DIR)../build/linux/asdf_makefile.inc

BINPATH = $(PROJPATH)/$(PROJNAME)/bin/linux/x86
###############################

############ LIBS #############
LIBS += sdl2 gl glew ftgl

LINK_FLAGS += -L$(LIBPATH) -rpath $(LIBPATH) -lasdfm -lSOIL
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
          $(SRCPATH)/data/hex_grid.cpp \
          $(SRCPATH)/data/_hex_map.cpp \
          $(SRCPATH)/data/spline.cpp \
          $(SRCPATH)/data/terrain_bank.cpp \
          $(SRCPATH)/ui/hex_map.cpp \
          $(SRCPATH)/ui/hex_tile.cpp \
          $(SRCPATH)/ui/spline_renderer.cpp \
          $(SRCPATH)/editor/main/editor.cpp \
          $(SRCPATH)/editor/main/input.cpp \
          $(SRCPATH)/editor/command_actions/command_actions.cpp \
          $(EXTPATH)/cJSON/cJSON.c

SRC_FOLDERS += main data ui editor/main editor/command_actions ../../ext/cJSON
###############################

# SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(PROJPATH)/build/linux/asdf_makefile.mk