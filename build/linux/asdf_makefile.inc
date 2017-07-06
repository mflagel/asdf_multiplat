#Compiler Stuff

ifndef TRAVIS
CC  := clang
CXX := clang++
# CC  := gcc
# CXX := g++
endif

LINK    := $(CXX)


WARNING_FLAGS_CLANG := -Wall \
				-Weverything \
				-Werror-uninitialized \
				-Wno-c++98-compat-pedantic \
				-Wno-gnu \
				-Wno-padded \
				-Wno-shadow \
				-Wno-comment \
				-Wno-undef \
				-Wno-extra-semi

# gcc pragma warning ignores don't work, so a few of the
# disabled flags are ones I would rather keep on
WARNING_FLAGS_GCC :=  -Wall \
					  -Wextra \
					  -Wno-padded \
					  -Wno-shadow \
					  -Wno-undef \
					  -Wno-format-zero-length \
					  -Wno-format \
					  -Wno-comment \
					  -Wno-write-strings \
					  -Wno-implicit-fallthrough


# Warning Flags that I think I can safely turn off
WARNING_FLAGS_CLANG += -Wno-weak-vtables


# WARNING_FLAGS_THAT_I_SHOULDNT_TURN_OFF
WARNING_FLAGS_CLANG += -Wno-microsoft \
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

# # Promote some warnings to errors
WARNING_FLAGS += -Werror=return-type


ifeq ($(CC),clang)
WARNING_FLAGS := $(WARNING_FLAGS_CLANG)
else ifeq ($$CC),gcc)
WARNING_FLAGS := $(WARNING_FLAGS_GCC)
endif



CLANG_EXTENSIONS := -fms-extensions
# DAS_FLAGS        := -g -pedantic -fmacro-backtrace-limit=20
DAS_FLAGS        := -g
CFLAGS  		 := $(DAS_FLAGS) $(WARNING_FLAGS) $(CLANG_EXTENSIONS)
CPP_SPECIFIC_FLAGS := -std=c++1z
# use defered assignment for CPPFLAGS so it will include anything CFLAGS gets later
CPPFLAGS 		  = $(CFLAGS) $(CPP_SPECIFIC_FLAGS)
LINK_FLAGS       += -L/usr/lib

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


