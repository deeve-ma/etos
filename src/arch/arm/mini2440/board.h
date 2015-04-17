/******************************************************************************
File    :  board.h

This file is part of the ETOS distribution
Copyright (c) 2014, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		ARM 平台支持头文件
		porting etos on arm

History:

Date           Author       Notes
----------     -------      -------------------------
2014-6-1       deeve        Create
2015-4-12      deeve        Add some comments


*******************************************************************************/
#ifndef __BOARD_H__
#define __BOARD_H__

/******************************************************************************
 *                                 Include Files                              *
 ******************************************************************************/
#include "etos_cfg.h"
#include "etos_hw_op.h"

/******************************************************************************
 *                                 Macros/Defines/Structures                  *
 ******************************************************************************/

typedef volatile u16  S3C2440_REG16;
typedef volatile u32  S3C2440_REG32;


/* CLOCK & POWER MANAGEMENT (see S3C2440 manual chapter 7) */
typedef struct _regs_clock_power_t {
    S3C2440_REG32   LOCKTIME;
    S3C2440_REG32   MPLLCON;
    S3C2440_REG32   UPLLCON;
    S3C2440_REG32   CLKCON;
    S3C2440_REG32   CLKSLOW;
    S3C2440_REG32   CLKDIVN;
    S3C2440_REG32   CAMDIVN;
} /*__attribute__((__packed__))*/ regs_clock_power_t;


/* NAND FLASH (see S3C2440 manual chapter 6) */
typedef struct {
    S3C2440_REG32   NFCONF;
    S3C2440_REG32   NFCONT;
    S3C2440_REG32   NFCMD;
    S3C2440_REG32   NFADDR;
    S3C2440_REG32   NFDATA;
    S3C2440_REG32   NFMECCD0;
    S3C2440_REG32   NFMECCD1;
    S3C2440_REG32   NFSECCD;
    S3C2440_REG32   NFSTAT;
    S3C2440_REG32   NFESTAT0;
    S3C2440_REG32   NFESTAT1;
    S3C2440_REG32   NFMECC0;
    S3C2440_REG32   NFMECC1;
    S3C2440_REG32   NFSECC;
    S3C2440_REG32   NFSBLK;
    S3C2440_REG32   NFEBLK;
} /*__attribute__((__packed__))*/ regs_nand_flash_t;



/******************************************************************************
 *                                 Declar Functions                           *
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
void clock_init_ll(void);


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
int copy_code_data_to_ram(unsigned long src_addr, unsigned char *dest_buf, int size);

#endif  /* __BOARD_H__ */

/* EOF */

