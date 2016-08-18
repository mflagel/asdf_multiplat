###############################
#       ASDF  MULTIPLAT       #
###############################
PROJNAME = asdf_multiplat

# INVOCATION = 1

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))

PROJPATH = .

include $(SELF_DIR)/../build/linux/asdf_makefile.inc
###############################

############ LIBS #############
LIBS += sdl2 gl glew ftgl

PKG_CFLAGS := -Iusr/include/SOIL
PKG_LFLAGS += -L$(LIBPATH) -lSOIL
###############################

########## INCLUDES ###########
INCLUDEPATH = $(PROJPATH)/include

SYSINCLUDES += $(EXTPATH)           \
			   $(EXTPATH)/glm		\

SYSINCLUDES += $(INCLUDEPATH)

INCLUDES   += $(SRCPATH)/data 	  \
			  $(SRCPATH)/main 	  \
			  $(SRCPATH)/ui 	  \
			  $(SRCPATH)/utilities \
			  $(SRCPATH)
###############################

########### SOURCES ###########
EXT_SOURCES = $(EXTPATH)/cJSON/cJSON.c

SRCPATH		= $(PROJPATH)/$(PROJNAME)

MAIN_SOURCES = $(SRCPATH)/main/main.cpp 		\
			   $(SRCPATH)/stdafx.cpp 		\
			   $(SRCPATH)/main/asdf_multiplat.cpp 		\
			   $(SRCPATH)/main/input_controller.cpp 		\
			   $(SRCPATH)/main/keyboard.cpp 		\
			   $(SRCPATH)/main/mouse.cpp 		\

DATA_SOURCES = $(SRCPATH)/data/shader.cpp 	    \
			   $(SRCPATH)/data/content_manager.cpp 	    \
			   $(SRCPATH)/data/texture.cpp 	    \
			   $(SRCPATH)/data/gl_resources.cpp 	    \
			   $(SRCPATH)/data/gl_state.cpp 	    \
			   $(SRCPATH)/data/texture_bank.cpp 	    \
			   $(SRCPATH)/data/texture_atlas.cpp 	    \

UTILITIES_SOURCES = $(UTILITIES_SRC_PATH)/spritebatch.cpp 		\
					$(UTILITIES_SRC_PATH)/thread_pool.cpp 	\
					$(UTILITIES_SRC_PATH)/camera.cpp 	\
					$(UTILITIES_SRC_PATH)/utilities.cpp 	\

UI_SOURCES = $(SRCPATH)/ui/ui_base.cpp \
			 $(SRCPATH)/ui/ui_button.cpp \
             $(SRCPATH)/ui/ui_label.cpp \
             $(SRCPATH)/ui/text.cpp \
             $(SRCPATH)/ui/polygon.cpp \

SOURCES += $(MAIN_SOURCES) $(DATA_SOURCES) $(UTILITIES_SOURCES) $(UI_SOURCES) $(EXT_SOURCES)

SRC_FOLDERS += data main utilities ui . ../ext/cJSON
###############################


include $(PROJPATH)/build/linux/asdf_makefile.mk