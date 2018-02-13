###############################
SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)/hexmap_common.mk

# INVOCATION = 1

SO_NAME = libhexmap.so
BIN_OUT = $(LIBPATH)/$(SO_NAME)
###############################

############ LIBS #############
# no non-common libs
# see hexmap_common.mk
###############################

########## INCLUDES ###########
# no non-common includes
# see hexmap_common.mk
###############################

########### SOURCES ###########
# no non-common source files
# see hexmap_common.mk
###############################

$(info self-dir ${SELF_DIR})
include $(SELF_DIR)/../build/linux/asdf_makefile.mk