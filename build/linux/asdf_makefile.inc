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
				-Werror=return-type \
				-Wno-c++98-compat-pedantic \
				-Wno-microsoft \
				-Wno-gnu \
				-Wno-padded \
				-Wno-undef \
				-Wno-extra-semi \
				-Wno-weak-vtables \
				-Wno-global-constructors

# WARNING_FLAGS_THAT_I_SHOULDNT_TURN_OFF
WARNING_FLAGS_CLANG += \
				 -Wno-invalid-token-paste \
				 -Wno-exit-time-destructors \
				 -Wno-extra-tokens \
				 -Wno-reorder \
				 -Wno-missing-prototypes \
				 -Wno-newline-eof \
				 -Wno-missing-braces


# gcc pragma warning ignores don't work, so a few of the
# disabled flags are ones I would rather keep on
WARNING_FLAGS_GCC :=  -Wall \
					  -Werror-uninitialized \
					  -Werror=return-type \
					  -Wextra \
					  -Wno-padded \
					  -Wno-shadow \
					  -Wno-undef \
					  -Wno-format-zero-length \
					  -Wno-format \
					  -Wno-comment \
					  -Wno-write-strings \
					  -Wno-implicit-fallthrough


# # Promote some warnings to errors
WARNING_FLAGS += 


ifeq ($(CC),clang)
WARNING_FLAGS = $(WARNING_FLAGS_CLANG)
else ifeq ($$CC),gcc)
WARNING_FLAGS = $(WARNING_FLAGS_GCC)
endif



CLANG_EXTENSIONS := -fms-extensions
# DAS_FLAGS        := -g -pedantic -fmacro-backtrace-limit=20
DAS_FLAGS        := -g
CFLAGS  		  = $(DAS_FLAGS) $(WARNING_FLAGS) $(CLANG_EXTENSIONS)
# use deffered assignment for CPPFLAGS so it will include anything CFLAGS gets later
# CPP_SPECIFIC_FLAGS := -stdlib=libstdc++ -std=c++1z
CPP_SPECIFIC_FLAGS := -std=c++1z
CPPFLAGS 		  = $(CFLAGS) $(CPP_SPECIFIC_FLAGS)
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


