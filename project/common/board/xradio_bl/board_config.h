/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include "driver/chip/hal_chip.h"
#include "driver/hal_board.h"
#include "driver/hal_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * board base minimal configuration for bootloader
 */

/* chip clock */
#define BOARD_LOSC_EXTERNAL     0   /* 0: inter 32k, 1: external 32k */
#define BOARD_CPU_CLK_SRC       PRCM_CPU_CLK_SRC_SYSCLK
#if defined(CONFIG_CHIP_XR872)
#define BOARD_CPU_CLK_FACTOR    PRCM_SYS_CLK_FACTOR_240M
#elif (defined(CONFIG_CHIP_XR808) || defined(CONFIG_CHIP_XR806))
#define BOARD_CPU_CLK_FACTOR    PRCM_SYS_CLK_FACTOR_160M
#endif
#define BOARD_DEV_CLK_FACTOR    PRCM_DEV_CLK_FACTOR_192M
#define BOARD_AHB2_CLK_DIV      CCM_AHB2_CLK_DIV_2
#define BOARD_APB_CLK_SRC       CCM_APB_CLK_SRC_HFCLK
#define BOARD_APB_CLK_DIV       CCM_APB_CLK_DIV_1

#define BOARD_APBS_CLK_SRC      CCM_APBS_CLK_SRC_DEVCLK
#define BOARD_APBS_CLK_FACTOR   CCM_APBS_CLK_DEV_FACTOR_48M

/* uart */
#define BOARD_MAIN_UART_ID      UART0_ID    /* debug and console */

#define BOARD_UART_BAUD_RATE    115200
#define BOARD_UART_PARITY       UART_PARITY_NONE
#define BOARD_UART_STOP_BITS    UART_STOP_BITS_1
#define BOARD_UART_DATA_BITS    UART_DATA_BITS_8
#define BOARD_UART_HW_FLOW_CTRL 0

/* flash */
#if 1 /* flash or psram or sip psram + no sip flash or sip flash + no sip psram */
#define BOARD_FLASH_PSRAM_PIN_MAP0  SYSCTL_FLASH_PSRAM_PIN_MAP0_ONLY_CS0
#define BOARD_FLASH_PSRAM_PIN_MAP1  SYSCTL_FLASH_PSRAM_PIN_MAP1_ONLY_CS1
#elif 0 /* no sip flash + no sip psram */
#define BOARD_FLASH_PSRAM_PIN_MAP0  SYSCTL_FLASH_PSRAM_PIN_MAP0_BOTH_CS0_1
#define BOARD_FLASH_PSRAM_PIN_MAP1  SYSCTL_FLASH_PSRAM_PIN_MAP1_RESERVED
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BOARD_CONFIG_H_ */
