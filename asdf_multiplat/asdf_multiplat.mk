###############################
#       ASDF  MULTIPLAT       #
###############################
PROJNAME = asdf_multiplat

# INVOCATION = 1

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)/../build/linux/asdf_makefile.inc
###############################

############ LIBS #############
LIBS += sdl2 gl glu glew ftgl soil

PKG_LFLAGS += -L$(LIBPATH)
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
SRCPATH		= $(PROJPATH)/$(PROJNAME)

MAIN_SOURCES = $(SRCPATH)/main/main.cpp 		\
			   $(SRCPATH)/stdafx.cpp 		\
			   $(SRCPATH)/main/asdf_multiplat.cpp 		\

DATA_SOURCES = $(SRCPATH)/data/shader.cpp 	    \
			   $(SRCPATH)/data/content_manager.cpp 	    \
			   $(SRCPATH)/data/texture.cpp 	    \
			   $(SRCPATH)/data/gl_resources.cpp 	    \

UTILITIES_SOURCES = $(UTILITIES_SRC_PATH)/spritebatch.cpp 		\
					$(UTILITIES_SRC_PATH)/thread_pool.cpp 	\
					$(UTILITIES_SRC_PATH)/camera.cpp 	\
					$(UTILITIES_SRC_PATH)/utilities.cpp 	\
					$(UTILITIES_SRC_PATH)/utilities_openGL.cpp 	\

UI_SOURCES = $(SRCPATH)/ui/ui_base.cpp \
			 $(SRCPATH)/ui/ui_button.cpp \
             $(SRCPATH)/ui/ui_label.cpp \
             $(SRCPATH)/ui/text.cpp \
             $(SRCPATH)/ui/polygon.cpp \

SOURCES += $(MAIN_SOURCES) $(DATA_SOURCES) $(UTILITIES_SOURCES) $(UI_SOURCES)

SRC_FOLDERS += data main utilities ui
###############################


include $(PROJPATH)/build/linux/asdf_makefile.mk