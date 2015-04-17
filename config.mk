
# set current path as TOP
TOP := $(shell pwd)

# set text base
TEXT_BASE := 0x30000000


#do not need set build source & header files
#always build $(TOP)/src file contents


# 定义交叉编译工具链前缀，对比x86平台则不用修改
# cross compile tools defined 
CROSS_COMPILE := arm-linux-


# define build options
CFLAGS := -fno-common -ffixed-r8 -fno-builtin -ffreestanding -nostdinc -pipe
CFLAGS += -marm  -mabi=aapcs-linux -mno-thumb-interwork -march=armv4 -Wall -Wstrict-prototypes -fno-stack-protector 

LDFLAGS := -Bstatic -nostdlib -Ttext $(TEXT_BASE) -T $(TOP)/src/arch/arm/build.lds

# 用到了除法，需要连接lgcc
# need link  libgcc.a for software division
GCC_LIB := $(dir $(shell which $(CROSS_COMPILE)gcc))../lib/gcc/arm-none-linux-gnueabi/4.3.2/armv4t
LDFLAGS += -L $(GCC_LIB) -lgcc

