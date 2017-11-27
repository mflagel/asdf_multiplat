###############################
#       ASDF  MULTIPLAT       #
###############################
PROJNAME = asdf_multiplat

# INVOCATION = 1

SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

BINPATH := $(SELF_DIR)/bin/linux
LIBPATH := $(SELF_DIR)/../lib/linux
OBJPATH := $(SELF_DIR)/obj

INCLUDEPATH := $(SELF_DIR)/../include
EXTPATH := $(SELF_DIR)/../ext
SRCPATH := $(SELF_DIR)/src

SO_NAME := libasdfm.so
BIN_OUT := $(LIBPATH)/$(SO_NAME)

include $(SELF_DIR)/../build/linux/asdf_makefile.inc
###############################

############ LIBS #############
LIBS += sdl2 gl glew zlib

CFLAGS += -Iusr/include/SOIL
LINK_FLAGS += -L$(LIBPATH) -lSOIL -ltar
###############################

########## INCLUDES ###########
SYSINCLUDES += $(EXTPATH)           \
               $(EXTPATH)/glm       \

SYSINCLUDES += $(INCLUDEPATH)

INCLUDES   += $(SRCPATH)/data      \
              $(SRCPATH)/main      \
              $(SRCPATH)/ui        \
              $(SRCPATH)/utilities \
              $(SRCPATH)
###############################

########### SOURCES ###########
EXT_SOURCES = $(EXTPATH)/cJSON/cJSON.c

MAIN_SOURCES = $(SRCPATH)/main/main.cpp             \
               $(SRCPATH)/stdafx.cpp                \
               $(SRCPATH)/main/asdf_multiplat.cpp   \
               $(SRCPATH)/main/input_controller.cpp \
               $(SRCPATH)/main/keyboard.cpp         \
               $(SRCPATH)/main/mouse.cpp            \
               $(SRCPATH)/main/input_sdl.cpp        \

DATA_SOURCES = $(SRCPATH)/data/shader.cpp          \
               $(SRCPATH)/data/content_manager.cpp \
               $(SRCPATH)/data/texture.cpp         \
               $(SRCPATH)/data/gl_resources.cpp    \
               $(SRCPATH)/data/gl_state.cpp        \
               $(SRCPATH)/data/texture_bank.cpp    \
               $(SRCPATH)/data/texture_atlas.cpp   \

UTILITIES_SOURCES = $(SRCPATH)/utilities/spritebatch.cpp \
                    $(SRCPATH)/utilities/camera.cpp      \
                    $(SRCPATH)/utilities/utilities.cpp   \

UI_SOURCES = $(SRCPATH)/ui/polygon.cpp 

#            $(SRCPATH)/ui/ui_base.cpp \
#            $(SRCPATH)/ui/ui_button.cpp \
#            $(SRCPATH)/ui/ui_label.cpp \
#            $(SRCPATH)/ui/text.cpp \

SOURCES += $(MAIN_SOURCES) $(DATA_SOURCES) $(UTILITIES_SOURCES) $(UI_SOURCES) $(EXT_SOURCES)

SRC_FOLDERS += data main utilities ui . ../../ext/cJSON
###############################


include $(SELF_DIR)/../build/linux/asdf_makefile.mk