/******************************************************************************
File    :  board.c

This file is part of the ETOS distribution
Copyright (c) 2013, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		ARM 平台支持文件
		porting etos on arm

History:

Date           Author       Notes
----------     -------      -------------------------
2013-11-17     deeve        Create
2015-4-12      deeve        Add some comments

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "board.h"
/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/

/*
 * 开发板将OM[3:2]固定接地，即clock来源是外部晶振:12M
 * S3C2440有两个PLL:
 *   MPLL:  用于产生FCLK(CPU使用)， HCLK(AHB)， PCLK(APB)三种频率
 *   UPLL:  专门用于驱动USB host/Device, 且必须为48MHz
 * 在系统复位之后，如果没有设定PLL，则采用外部晶振的频率作为FCLK，同时FCLK:HCLK:PCLK的比例关系为1:1:1
 * 通过改变CLKDIVN可以改变FCLK,HCLK,PCLK的分频比
 *
 * MPLL = (2 * m * Fin) / (p * 2^s)   => FCLK
 * m = (MDIV + 8), p = (PDIV + 2), s = SDIV
 * MDIV/PDIV/SDIV is in  register MPLLCON(0x4C000004)
 *
 * UPLL = (m * Fin) / (p * 2)
 * m = (MDIV + 8), p = (PDIV + 2), s = SDIV
 */

/*use 405Mhz for boot fast*/
#define S3C2440_MPLL_405MHZ     ((0x7f<<12)|(2<<4)|(0x01))
#define S3C2440_UPLL_48MHZ      ((0x38<<12)|(0x02<<4)|(0x02))
#define S3C2440_405M_CLKDIV     (0x05) /* FCLK:HCLK:PCLK = 1:4:8 */



#define NAND_OP_BUSY            (1)



/*for nand Samsung K9K8G08 */
#define NAND_PAGE_SIZE      (2048)
#define NAND_BLOCK_SIZE     (128*1024)
#define NAND_BLOCK_MASK     (NAND_BLOCK_SIZE - 1)


/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_RESET		0xff

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

/* Extended commands for AG-AND device */
/*
 * Note: the command for NAND_CMD_DEPLETE1 is really 0x00 but
 *       there is no way to distinguish that from NAND_CMD_READ0
 *       until the remaining sequence of commands has been completed
 *       so add a high order bit and mask it off in the command.
 */
#define NAND_CMD_DEPLETE1	0x100
#define NAND_CMD_DEPLETE2	0x38
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_STATUS_ERROR	0x72
/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	0x73
#define NAND_CMD_STATUS_ERROR1	0x74
#define NAND_CMD_STATUS_ERROR2	0x75
#define NAND_CMD_STATUS_ERROR3	0x76
#define NAND_CMD_STATUS_RESET	0x7f
#define NAND_CMD_STATUS_CLEAR	0xff


/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/

static inline void delay(unsigned long loops)
{
    __asm__ volatile ("1:\n"
                      "subs %0, %1, #1\n"
                      "bne 1b":"=r" (loops):"0" (loops));
}


/* 等待NAND Flash就绪 */
static void s3c2440_wait_idle(void)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;

    while (!(*p & NAND_OP_BUSY)) {
        delay(10);
    }
}


/* 等待NAND Flash就绪 */
static void s3c2440_wait_reset_done(void)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;

    while ((*p & 0x4) == 0) {
        delay(10);
    }
}


/* 发出片选信号 */
static void s3c2440_nand_select_chip(void)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;

    s3c2440nand->NFCONT &= ~(1 << 1);
    delay(10);
}

/* 取消片选信号 */
static void s3c2440_nand_deselect_chip(void)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;

    s3c2440nand->NFCONT |= (1 << 1);
}


/* clear RnB */
static void s3c2440_nand_clear_RnB(void)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;

    s3c2440nand->NFSTAT |= (1 << 2);
}


/* 发出命令 */
static void s3c2440_write_cmd(int cmd)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;

    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFCMD;
    *p = cmd;
}

/* 发出地址 */
static void s3c2440_write_addr(unsigned int addr)
{
    unsigned int page_num;
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;
    volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
#if 0
    *p = addr & 0xff;
    delay(10);
    *p = (addr >> 9) & 0xff;
    delay(10);
    *p = (addr >> 17) & 0xff;
    delay(10);
    *p = (addr >> 25) & 0xff;
    delay(10);
#else
    page_num = addr >> 11; /* addr / 2048 */
    *p = 0;
    delay(10);
    *p = 0;
    delay(10);
    *p = page_num & 0xff;
    delay(10);
    *p = (page_num >> 8) & 0xff;
    delay(10);
    *p = (page_num >> 16) & 0xff;
#endif
}

/* 读取数据 */
static unsigned short s3c2440_read_data_16(void)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;
    volatile unsigned short *p = (volatile unsigned short *)&s3c2440nand->NFDATA;
    return *p;
}


/* reset */
static void nand_reset(void)
{
    s3c2440_nand_select_chip();
    s3c2440_write_cmd(0xff);  // 复位命令
    s3c2440_wait_reset_done();
    s3c2440_nand_deselect_chip();
}


/* init */
void nand_init_ll(void)
{
    regs_nand_flash_t *s3c2440nand = (regs_nand_flash_t *)0x4e000000;

#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

    /* 设置时序 */
    //s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
    s3c2440nand->NFCONF = (7 << 12) | (7 << 8) | (7 << 4);

    /* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
    s3c2440nand->NFCONT = (1 << 4) | (1 << 1) | (1 << 0);

    /* 复位NAND Flash */
    nand_reset();
}

/* read page */
static int nand_read_page_ll(unsigned char *buf, unsigned long addr)
{
    unsigned int i;
    unsigned short *ptr16 = (unsigned short *)buf;

    s3c2440_nand_clear_RnB();

    /* 发出READ0命令 */
    s3c2440_write_cmd(NAND_CMD_READ0);

    /* Write Address */
    s3c2440_write_addr(addr);

    s3c2440_write_cmd(NAND_CMD_READSTART);
    s3c2440_wait_idle();

    for (i = 0; i < (NAND_PAGE_SIZE >> 1); i++) {
        *ptr16 = s3c2440_read_data_16();
        ptr16++;
    }

    return NAND_PAGE_SIZE;
}



/* 读函数 */
int nand_read_ll(unsigned char *buf, unsigned long start_addr, int size)
{
    int i, j;

    if ((start_addr & NAND_BLOCK_MASK) /*|| (size & NAND_BLOCK_MASK)*/) {
        return 1;    /* 地址或长度不对齐 */
    }

    if ((size & (NAND_PAGE_SIZE - 1))) {
        size += (NAND_PAGE_SIZE - 1);
        size &= (~(NAND_PAGE_SIZE - 1));
    }

    /* 选中芯片 */
    s3c2440_nand_select_chip();
    s3c2440_nand_clear_RnB();

    delay(10);

    for (i = start_addr; i < (start_addr + size);) {
        j = nand_read_page_ll(buf, i);
        i += j;
        buf += j;
    }

    /* 取消片选信号 */
    s3c2440_nand_deselect_chip();

    return 0;
}


/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/
/**
 * init clock at boot.
 * config mcu clock frequency at boot sequence
 *
 * @param[in]    void
 *
 * @return     none
 *
 * @note  this clock is only used on boot stage
 * @authors    deeve
 * @date       2015/4/12
 */
void clock_init_ll(void)
{
    regs_clock_power_t *clk_power = (regs_clock_power_t *)0x4C000000;

    clk_power->CLKDIVN = S3C2440_405M_CLKDIV;

    /* change to asynchronous bus mod */
    __asm__(    "mrc    p15, 0, r1, c1, c0, 0\n"    /* read ctrl register   */
                "orr    r1, r1, #0xc0000000\n"      /* Asynchronous         */
                "mcr    p15, 0, r1, c1, c0, 0\n"    /* write ctrl register  */
                :::"r1"
           );

    /* to reduce PLL lock time, adjust the LOCKTIME register */
    clk_power->LOCKTIME = 0xFFFFFFFF;

    /* configure UPLL */
    clk_power->UPLLCON = S3C2440_UPLL_48MHZ;

    /* some delay between MPLL and UPLL */
    delay (4000);

    /* configure MPLL */
    clk_power->MPLLCON = S3C2440_MPLL_405MHZ;

    /* some delay for pll stable */
    delay (8000);
}


/**
 * copy code and data to ram.
 * code and data is in nand flash, so we need to copy them to ram at boot stage
 *
 * @param[in]    src_addr    start address in nand flash
 * @param[in]    dest_buf    destination address in ram
 * @param[in]    size        copy length
 *
 * @return
 * @retval 0       success
 * @retval 1       fail
 *
 * @note       pay attention to the define NAND_PAGE_SIZE & NAND_BLOCK_SIZE
 * @authors    deeve
 * @date       2015/4/12
 */
int copy_code_data_to_ram(unsigned long src_addr, unsigned char *dest_buf, int size)
{
    /* 初始化NAND Flash */
    nand_init_ll();
    /* 从 NAND Flash启动 */
    return nand_read_ll(dest_buf, src_addr, (size + NAND_BLOCK_MASK)&~(NAND_BLOCK_MASK));
}

/* EOF */

