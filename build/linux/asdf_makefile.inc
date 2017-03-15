#Compiler Stuff

ifndef TRAVIS
CC 	    := clang
CXX 	:= clang++
endif
LINK    := $(CXX)


WARNING_FLAGS := -Wall \
				-Weverything \
				-Wno-c++98-compat-pedantic \
				-Wno-gnu \
				-Wno-padded \
				-Wno-shadow \
				-Wno-comment \
				-Wno-undef \
				-Wno-extra-semi \
				-Werror-uninitialized

# Warning Flags that I think I can safely turn off
WARNING_FLAGS += -Wno-weak-vtables


# WARNING_FLAGS_THAT_I_SHOULDNT_TURN_OFF
WARNING_FLAGS += -Wno-microsoft \
				 -Wno-invalid-token-paste \
				 -Wno-shorten-64-to-32 \
				 -Wno-sign-conversion \
				 -Wno-float-equal \
				 -Wno-unused-variable \
				 -Wno-format-security \
				 -Wno-global-constructors \
				 -Wno-exit-time-destructors \
				 -Wno-extra-tokens \
				 -Wno-reorder \
				 -Wno-missing-prototypes \
				 -Wno-newline-eof \
				 -Wno-switch-enum \
				 -Wno-missing-braces \
				 -Wno-writable-strings \
				 -Wno-comment

# Promote some warnings to errors
WARNING_FLAGS += -Werror=return-type


CLANG_EXTENSIONS := -fms-extensions
DAS_FLAGS 		 := $(WARNING_FLAGS) $(CLANG_EXTENSIONS) -g -pedantic -fmacro-backtrace-limit=20
CFLAGS  		 := $(DAS_FLAGS)
# use deffered assignment for CPPFLAGS so it will include anything CFLAGS gets later
CPPFLAGS 		  = $(CFLAGS) -stdlib=libstdc++ -std=c++1z
LINK_FLAGS       := -L/usr/lib

ifndef PROJNAME
$(error PROJNAME not defined)
endif

#Useful Paths

ifndef PROJPATH
# PROJPATH 	:= ~/gdrive/Programming/AsdfMultiplat
$(error PROJPATH must be defined);
endif

BINPATH		:= $(PROJPATH)/bin/linux
LIBPATH		:= $(PROJPATH)/lib/linux
EXTPATH		:= $(PROJPATH)/ext
OBJPATH		:= $(PROJPATH)/obj/$(PROJNAME)/linux

SHELL	= /bin/sh

.SILENT :


