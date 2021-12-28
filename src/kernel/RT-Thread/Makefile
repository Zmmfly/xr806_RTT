#
# Rules for building library
#

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------
ROOT_PATH := ../../..

include $(ROOT_PATH)/gcc.mk

# ----------------------------------------------------------------------------
# library and objects
# ----------------------------------------------------------------------------
LIBS := librtthread.a

# ifneq ($(CONFIG_ROM_FREERTOS), y)
# ifeq ($(CONFIG_TRUSTZONE), y)
#   RTOS_DIRS_IGNORE := $(shell find ./Source/portable/GCC/$(CONFIG_OS_FREERTOS_PLATFORM)/secure -type d)
# endif
# RTOS_DIRS := ./Source $(shell find ./Source/portable/GCC/$(CONFIG_OS_FREERTOS_PLATFORM) -type d)
SRC_DIRS := ./src 
SRC_DIRS += ./bsp 
SRC_DIRS += ./components/finsh 
SRC_DIRS += ./libcpu/arm/cortex-m33 

#	# $(shell find ./libcpu/arm/cortex-m33 -type d)

DIRS := SRC_DIRS

SRCS := $(sort $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS]))))

# INCLUDE_PATHS += $(foreach dir, $(DIRS), -I$(dir))

OBJS := $(addsuffix .o,$(SRCS))

# library make rules
include $(LIB_MAKE_RULES)
