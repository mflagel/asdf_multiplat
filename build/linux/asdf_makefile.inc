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
DAS_FLAGS        := -g
CFLAGS  		 := $(DAS_FLAGS) $(WARNING_FLAGS) $(CLANG_EXTENSIONS)
CPP_SPECIFIC_FLAGS := -std=c++1z
# use defered assignment for CPPFLAGS so it will include anything CFLAGS gets later
CPPFLAGS 		  = $(CFLAGS) $(CPP_SPECIFIC_FLAGS)
LINK_FLAGS       += -L/usr/lib

ifndef PROJNAME
$(error PROJNAME not defined)
endif


define ERROR_NOT_DEFINED
$(error $(1) is not defined)
endef


ifndef PROJNAME
$(call ERROR_NOT_DEFINED, PROJNAME)
endif
ifndef BINPATH
$(call ERROR_NOT_DEFINED, BINPATH)
endif
ifndef LIBPATH
$(call ERROR_NOT_DEFINED, LIBPATH)
endif
ifndef OBJPATH
$(call ERROR_NOT_DEFINED, OBJPATH)
endif
ifndef INCLUDEPATH
$(call ERROR_NOT_DEFINED, INCLUDEPATH)
endif
ifndef SRCPATH
$(call ERROR_NOT_DEFINED, SRCPATH)
endif



SHELL	= /bin/sh

.SILENT :


