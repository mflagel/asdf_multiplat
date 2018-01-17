###############################
SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)/hexmap_common.mk

# INVOCATION = 1

BIN_OUT = $(BINPATH)/$(PROJNAME)
###############################

########## INCLUDES ###########
# no non-common includes
###############################

########### SOURCES ###########
SOURCES += $(SRCPATH)/main/main.cpp
###############################


$(info self-dir ${SELF_DIR})
include $(SELF_DIR)/../build/linux/asdf_makefile.mk