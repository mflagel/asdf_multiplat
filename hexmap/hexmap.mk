###############################
SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
include $(SELF_DIR)/hexmap_common.mk

# INVOCATION = 1
###############################

########## INCLUDES ###########
# no non-common includes
###############################

########### SOURCES ###########
SOURCES += $(SRCPATH)/main/main.cpp
###############################


include $(PROJPATH)/build/linux/asdf_makefile.mk