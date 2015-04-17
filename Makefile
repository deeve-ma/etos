#
# This file is a(part of) free software; you can redistribute it 
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2 of 
# the License,, or (at your option) any later version.
#
# this file is encoded UTF-8 no bom
#
# 这个Makefile会将当前src目录下(包括子目录)所有的.c 和 .S 编成.o，
# 并依原始文件的目录结构将.o存放在build目录中
# 最后生成目标文件放于当前目录
#
# build all *.S & *.c in src/, make corresponding object file below build/,
# and build target file in current directory
#
# history:
#     vision  modification                author
#     v1.0    Create                      deeve.ma@gmail.com
#     v1.1    add some comments           deeve.ma@gmail.com


include config.mk

#最终生成的文件的命名，一般不需要修改
#target name
TARGET_ELF := $(notdir $(shell pwd)).elf


# the variable below is defined in config.mk
ifeq ($(TEXT_BASE),)
	$(error "TEXT_BASE is not defined!")
endif
ifeq ($(CROSS_COMPILE),)
	$(error "CROSS_COMPILE is not defined!")
endif

TOP ?= $(shell pwd)


#编译过程中用到的各种编译选项可以在这里定义
#-I头文件不用手动放在CFLAGS中，会自动添加进来
#除非需要添加不位于src/下的头文件，不过在config.mk中添加比较好
#special build flag for app/lib
#do not need add -Iincludes options in CFLAGS manually
CFLAGS    +=  -DTEXT_BASE=$(TEXT_BASE)
CPPFLAGS  += 
LDFLAGS   +=  -Map $(TARGET_ELF).map
LIBS      += 

LIB_DIR   +=

#是否编译debug版本的设定
#debug vision setting
## DBG_EN = y , use -g O0
DBG_EN	?=n

#文件关键位置的定义
#global directory defined

SRCS_DIR_TOP = $(TOP)/src
OBJS_DIR_TOP = $(TOP)/build

BUILD_TIME_STR := $(shell date "+%F %T")
BUILD_TIME_HEADER_FILE := $(SRCS_DIR_TOP)/include/common/build_time.h

###########################################################################

.PHONY: all install clean distclean show build_time

#定义交叉编译环境变量，当需要编译arm/mips等平台应用程序/库的时候修改它
#cross compile tools defined 

CROSS_COMPILE ?=

AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
CC      = $(CROSS_COMPILE)gcc
CPP     = $(CC) -E
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
STRIP   = $(CROSS_COMPILE)strip
RANLIB  = $(CROSS_COMPILE)ranlib
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump


#本机相关的命令，一般无需修改
#local host tools defined
CP    := cp
RM    := rm
MKDIR := mkdir
SED   := sed
FIND  := find
MKDIR := mkdir
XARGS := xargs


#定义需要编译的源文件和头文件
#define source files & header files
ALL_BUILD_C_FILE = $(shell $(FIND) $(SRCS_DIR_TOP) -name "*.c")
ALL_BUILD_H_FILE = $(shell $(FIND) $(SRCS_DIR_TOP) -name "*.h")
ALL_BUILD_S_FILE = $(shell $(FIND) $(SRCS_DIR_TOP) -name "*.S")

#header file folders
SEPARATE_INC_DIRS := $(foreach each_h, $(ALL_BUILD_H_FILE), $(dir $(each_h)))
INC_DIRS := $(sort $(SEPARATE_INC_DIRS))

#source files
SRCS := $(ALL_BUILD_C_FILE) $(ALL_BUILD_S_FILE)

#objects files
ABS_OBJS_ASM := $(patsubst %.S,%.o,$(ALL_BUILD_S_FILE))
ABS_OBJS_C := $(patsubst %.c,%.o,$(ALL_BUILD_C_FILE))

OBJS_ASM := $(foreach sub,$(ABS_OBJS_ASM),$(subst $(SRCS_DIR_TOP), $(OBJS_DIR_TOP),$(sub)))
OBJS_C := $(foreach sub,$(ABS_OBJS_C),$(subst $(SRCS_DIR_TOP), $(OBJS_DIR_TOP),$(sub)))

OBJS  := $(OBJS_ASM) $(OBJS_C)

OBJS_ASM_DIRS := $(sort $(foreach sub,$(OBJS_ASM),$(dir $(sub))))
OBJS_C_DIRS := $(sort $(foreach sub,$(OBJS_C),$(dir $(sub))))

#所有.d依赖文件放入DEPS
#all depends file
DEPS  := $(patsubst %.o,%.d,$(OBJS))

ifeq ($(DBG_EN), y)
CFLAGS += -DDBG_EN=1 -g -O0
else
CFLAGS += -DDBG_EN=0 -g -Os 
endif

#注意-MD，是为了生成.d文件后，构造对.h的依赖
#common build options define
CFLAGS    += -Wall -Wno-format-security -MD
CFLAGS    += $(foreach dir,$(INC_DIRS),-I$(dir))
ARFLAGS   := rc

#Xlinker是为了在提供多个.a时，未知它们之间的依赖顺序时，自动查找依赖顺序
#link options define
#CPPFLAGS   += 
#XLDFLAGS   := -Xlinker "-(" $(LDFLAGS) -Xlinker "-)"
ifeq ($(strip $(LIBS)),)
  XLDFLAGS := $(LDFLAGS)
else
  XLDFLAGS := $(LDFLAGS) --start-group $(LIBS) --end-group
endif
  
ifeq ($(strip $(LIB_DIR)),)
LDLIBS     +=
else
LDLIBS     += -L $(LIB_DIR) 
endif


#默认编译可执行文件elf  
#default target
all: build_time $(TARGET_ELF)

#for .h header files dependence
-include $(DEPS)


$(TARGET_ELF) : $(OBJS)
	$(LD) $^ $(XLDFLAGS) $(LDLIBS) -o $@
	$(OBJDUMP) -xdt $@ > $@.dis
ifeq ($(DBG_EN), y)
	$(STRIP) --strip-unneeded $@
endif
	$(OBJCOPY) --gap-fill=0xff -O binary $@ $@.bin

#build rule for all *.S
define BUILD_SUBDIR_TARGET_ASM
$(1)%.o: $$(subst $(OBJS_DIR_TOP),$(SRCS_DIR_TOP),$(1))%.S
	@[ -d $$(dir $$@) ] | $(MKDIR) -p $$(dir $$@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $$@ -c $$<
endef
$(foreach subobj,$(OBJS_ASM_DIRS),$(eval $(call BUILD_SUBDIR_TARGET_ASM,$(subobj))))

#build rule for all *.c
define BUILD_SUBDIR_TARGET_C
$(1)%.o: $$(subst $(OBJS_DIR_TOP),$(SRCS_DIR_TOP),$(1))%.c
	@[ -d $$(dir $$@) ] | $(MKDIR) -p $$(dir $$@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $$@ -c $$<
endef
##all sub dir obj build rule
$(foreach subobj,$(OBJS_C_DIRS),$(eval $(call BUILD_SUBDIR_TARGET_C,$(subobj))))


build_time:
	@echo "build time: $(BUILD_TIME_STR)"
	@echo '#ifndef __ETOS_BUILD_TIME_H__' > $(BUILD_TIME_HEADER_FILE)
	@echo '#define __ETOS_BUILD_TIME_H__' >> $(BUILD_TIME_HEADER_FILE)
	@echo "const char *build_time = \"$(BUILD_TIME_STR)\";"  >> $(BUILD_TIME_HEADER_FILE)
	@echo '#endif /* __ETOS_BUILD_TIME_H__ */' >> $(BUILD_TIME_HEADER_FILE)

install:
	@echo "nothing to do at this moment!"

clean:
	$(FIND) $(OBJS_DIR_TOP) -name "*.o" -o -name "*.d" | $(XARGS) $(RM) -f
	$(RM) -f $(TARGET_ELF)* 

distclean : clean
	$(RM) -rf $(OBJS_DIR_TOP)

show:
	@echo "SRCS: $(SRCS)"
	@echo "OBJS: $(OBJS)"
	@echo "OBJS_C_DIRS: $(OBJS_C_DIRS)"
	@echo "OBJS_ASM_DIRS: $(OBJS_ASM_DIRS)"
	@echo "SEPARATE_INC_DIRS: $(SEPARATE_INC_DIRS)"
