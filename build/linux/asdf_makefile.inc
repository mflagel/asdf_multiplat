#Compiler Stuff
CC 	    := clang
CPP 	:= clang++
LINK 	:= $(CPP)


WARNING_FLAGS := -Wall \
				-Weverything \
				-Wno-c++98-compat-pedantic \
				-Wno-gnu \
				-Wno-padded \
				-Wno-shadow \
				-Wno-comment \
				-Wno-undef \
				-Werror-uninitialized

# WARNING_FLAGS_THAT_I_SHOULDNT_TURN_OFF
WARNING_FLAGS += -Wno-microsoft \
				 -Wno-invalid-token-paste \
				 -Wno-shorten-64-to-32 \
				 -Wno-sign-conversion \
				 -Wno-conversion \
				 -Wno-float-equal \
				 -Wno-unused-variable \
				 -Wno-format-security \
				 -Wno-global-constructors \
				 -Wno-exit-time-destructors \
				 -Wno-extra-tokens \
				 -Wno-reorder \
				 -Wno-missing-prototypes \
				 -Wno-newline-eof \
				 -Wno-weak-vtables \
				 -Wno-unused-parameter \
				 -Wno-switch-enum \
				 -Wno-missing-braces

#

#-Wno-weak-vtables

CLANG_EXTENSIONS := -fms-extensions
DAS_FLAGS 		 := $(WARNING_FLAGS) $(CLANG_EXTENSIONS) -g -pedantic -fmacro-backtrace-limit=20
CFLAGS  		 := $(DAS_FLAGS) -fPIC
# use deffered assignment for CPPFLAGS so it will include anything CFLAGS gets later
CPPFLAGS 		  = $(CFLAGS) -fPIC -stdlib=libstdc++ -std=c++11
LINK_FLAGS 		 := $(DAS_FLAGS) -L/usr/lib

ifndef PROJNAME
$(error PROJNAME not defined)
endif

#Useful Paths
PROJPATH 	:= ~/Programming/AsdfMultiplat
BINPATH		:= $(PROJPATH)/bin/linux
LIBPATH		:= $(PROJPATH)/lib
EXTPATH		:= $(PROJPATH)/ext
OBJPATH		:= $(PROJPATH)/obj/$(PROJNAME)/linux

SHELL	= /bin/sh

.SILENT :


