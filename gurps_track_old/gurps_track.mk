###############################
PROJNAME = gurps_track
# BIN_OUT = $(BINPATH)/$(PROJNAME)

# INVOCATION = 1

SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)../build/linux/asdf_makefile.inc
###############################

############ LIBS #############
LIBS += sdl2 gl glu glew ftgl soil glfw3

LINK_FLAGS += -L$(LIBPATH)

LINK_FLAGS += -L/home/mathias/Programming/AsdfMultiplat/lib/linux
LINK_FLAGS +=  -lkiui

ASDFDIR = /home/mathias/Programming/AsdfMultiplat/obj/asdf_multiplat/linux
# this still feels super janky
ASDFASDF = $(ASDFDIR)/asdf_multiplat.o   \
		   $(ASDFDIR)/spritebatch.o      \
		   $(ASDFDIR)/ui_base.o          \
		   $(ASDFDIR)/ui_button.o        \
		   $(ASDFDIR)/ui_label.o          \
		   $(ASDFDIR)/text.o          \
		   $(ASDFDIR)/utilities_openGL.o \
		   $(ASDFDIR)/content_manager.o  \
		   $(ASDFDIR)/shader.o           \
		   $(ASDFDIR)/utilities.o        \
		   $(ASDFDIR)/camera.o        \
		   $(ASDFDIR)/texture.o        \
		   # $(ASDFDIR)/stdafx.o

# ASDFASDF += $(EXTPATH)/cJSON/cJSON.o

LINK_FLAGS += $(ASDFASDF)

# --- WTF ---
# attempt to open ~/Programming/AsdfMultiplat/lib/libasdf.so failed
PKG_LFLAGS += -L$(LIBPATH)
###############################

########## INCLUDES ###########
INCLUDEPATH = $(PROJPATH)/include

SYSINCLUDES += $(EXTPATH)           \
			   $(EXTPATH)/glm		\
			   $(EXTPATH)/cJSON		\

SYSINCLUDES += $(INCLUDEPATH)

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
EXT_SOURCES = $(EXTPATH)/cJSON/cJSON.c

SRCPATH		= $(PROJPATH)/$(PROJNAME)

MAIN_SOURCES = $(SRCPATH)/main/main.cpp 		\
			  $(SRCPATH)/main/stdafx.cpp 		\
			  $(SRCPATH)/main/gurps_track.cpp 		\

DATA_SOURCES = $(SRCPATH)/data/character.cpp    \
			   $(SRCPATH)/data/to_from_json.cpp    \
               $(SRCPATH)/data/skills.cpp    \

UTILITIES_SOURCES = 
# $(UTILITIES_SRC_PATH)/		\

UI_SOURCES = $(SRCPATH)/ui/ui_wrapper.cpp \
			 $(SRCPATH)/ui/character_layout.cpp		\
             $(SRCPATH)/ui/character_main_layout.cpp \
             $(SRCPATH)/ui/character_combat_layout.cpp \
             $(SRCPATH)/ui/character_traits_layout.cpp \
             $(SRCPATH)/ui/character_skills_layout.cpp \
             $(SRCPATH)/ui/character_spells_layout.cpp \
             $(SRCPATH)/ui/skill_list_layout.cpp

SOURCES += $(EXT_SOURCES) $(MAIN_SOURCES) $(DATA_SOURCES) $(UTILITIES_SOURCES) $(UI_SOURCES)

SRC_FOLDERS += data main utilities ui ../ext/cJSON

# DEPENDANCY_MAKEFILES += $(PROJPATH)/asdf_multiplat/asdf_multiplat.mk
###############################

# SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(PROJPATH)/build/linux/asdf_makefile.mk