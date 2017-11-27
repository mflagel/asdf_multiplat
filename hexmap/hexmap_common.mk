###############################
PROJNAME = hexmap

# SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
ifndef SELF_DIR
$(error SELF_DIR is not defined)
endif

BINPATH = $(SELF_DIR)/bin/linux
LIBPATH := $(SELF_DIR)/../lib/linux
OBJPATH := $(SELF_DIR)/obj

INCLUDEPATH := $(SELF_DIR)/../include
EXTPATH := $(SELF_DIR)/../ext
SRCPATH := $(SELF_DIR)/src

include $(SELF_DIR)../build/linux/asdf_makefile.inc
###############################

############ LIBS #############
LIBS += sdl2 gl glew

LINK_FLAGS += -L$(LIBPATH) -Wl,-rpath,$(LIBPATH) -lasdfm -lSOIL -lstdc++fs
###############################

########## INCLUDES ###########
SYSINCLUDES += $(EXTPATH)       \
               $(EXTPATH)/glm   \
               $(EXTPATH)/cJSON \

SYSINCLUDES += $(INCLUDEPATH)      \
               $(INCLUDEPATH)/SOIL \

SYSINCLUDES += $(INCLUDEPATH)/asdfm/           \
               $(INCLUDEPATH)/asdfm/data       \
               $(INCLUDEPATH)/asdfm/main       \
               $(INCLUDEPATH)/asdfm/ui         \
               $(INCLUDEPATH)/asdfm/utilities  \

INCLUDES   += $(SRCPATH)           \
              $(SRCPATH)/data      \
              $(SRCPATH)/main      \
              $(SRCPATH)/ui        \
              $(SRCPATH)/utilities \

###############################

########### SOURCES ###########
SOURCES = $(SRCPATH)/main/stdafx.cpp \
          $(SRCPATH)/main/hexmap.cpp \
          \
          $(SRCPATH)/data/_hex_grid.cpp \
          $(SRCPATH)/data/_hex_map.cpp \
          $(SRCPATH)/data/hex_util.cpp \
          $(SRCPATH)/data/spline.cpp \
          $(SRCPATH)/data/terrain_bank.cpp \
          $(SRCPATH)/data/terrain_brush.cpp \
          \
          $(SRCPATH)/ui/hex_grid.cpp \
          $(SRCPATH)/ui/hex_map.cpp \
          $(SRCPATH)/ui/hex_tile.cpp \
          $(SRCPATH)/ui/spline_renderer.cpp \
          $(SRCPATH)/ui/terrain_brush_renderer.cpp \
          $(SRCPATH)/ui/minimap.cpp \
          \
          $(SRCPATH)/editor/main/editor.cpp \
          $(SRCPATH)/editor/main/editor_workspace.cpp \
          $(SRCPATH)/editor/main/input.cpp \
          $(SRCPATH)/editor/command_actions/command_actions.cpp \
          \
          $(EXTPATH)/cJSON/cJSON.c

SRC_FOLDERS += main data ui editor/main editor/command_actions ../../ext/cJSON 
###############################