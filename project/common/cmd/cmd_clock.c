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

#include "cmd_util.h"
#include "driver/chip/hal_clock.h"

enum CMD_CLOCK_TYPE {
	CMD_CLOCK_TYPE_HF = 0,
	CMD_CLOCK_TYPE_LF,
	CMD_CLOCK_TYPE_CPU,
	CMD_CLOCK_TYPE_DEV,
	CMD_CLOCK_TYPE_AHB1,
	CMD_CLOCK_TYPE_AHB2,
	CMD_CLOCK_TYPE_APB,
	CMD_CLOCK_TYPE_APBS,
	CMD_CLOCK_TYPE_NUM
};

/* index by enum CMD_CLOCK_TYPE */
static const char *cmd_clock_type_str[CMD_CLOCK_TYPE_NUM] = {
	"HF", "LF", "CPU", "DEV", "AHB1", "AHB2", "APB", "APBS"
};

static const char *cmd_clock32k_type_str[PRCM_LFCLK_SRC_MAX] = {
	"LOSC", "RCOSC", "RCCAL", "DIV32K",
};

static uint32_t cmd_clock_get(enum CMD_CLOCK_TYPE type)
{
	uint32_t clock;

	switch (type) {
	case CMD_CLOCK_TYPE_HF:
		clock = HAL_GetHFClock();
		break;
	case CMD_CLOCK_TYPE_LF:
		clock = HAL_GetLFClock(PRCM_LFCLK_MODULE_SYS);
		break;
	case CMD_CLOCK_TYPE_CPU:
		clock = HAL_GetCPUClock();
		break;
	case CMD_CLOCK_TYPE_DEV:
		clock = HAL_GetDevClock();
		break;
	case CMD_CLOCK_TYPE_AHB1:
		clock = HAL_GetAHB1Clock();
		break;
	case CMD_CLOCK_TYPE_AHB2:
		clock = HAL_GetAHB2Clock();
		break;
	case CMD_CLOCK_TYPE_APB:
		clock = HAL_GetAPBClock();
		break;
#if (CONFIG_CHIP_ARCH_VER > 1)
	case CMD_CLOCK_TYPE_APBS:
		clock = HAL_GetAPBSClock();
		break;
#endif
	default:
		clock = 0;
		break;
	}
	return clock;
}

/*
 * drv clock <type>
 */
enum cmd_status cmd_clock_exec(char *cmd)
{
	int type;
	uint32_t clock = 0;
	uint32_t cnt;
	uint32_t src, module;

	cnt = cmd_sscanf(cmd, "LF get m=%d", &module);
	if (cnt == 1) {
		clock = HAL_GetLFClock(module);
		type = HAL_GetLFClockType(module);
		cmd_write_respond(CMD_STATUS_OK, "%d Hz type:%s (%s clock)", clock,
		                  cmd_clock32k_type_str[type], cmd);
		return CMD_STATUS_ACKED;
	}
	cnt = cmd_sscanf(cmd, "LF set m=%d s=%d", &module, &src);
	if (cnt == 2) {
		HAL_PRCM_SetLFCLKSource(module, src);
		return CMD_STATUS_ACKED;
	}

	for (type = 0; type < CMD_CLOCK_TYPE_NUM; ++type) {
		if (cmd_strcasecmp(cmd, cmd_clock_type_str[type]) == 0) {
			clock = cmd_clock_get((enum CMD_CLOCK_TYPE)type);
			break;
		}
	}

	if (clock != 0) {
		cmd_write_respond(CMD_STATUS_OK, "%d Hz (%s clock)", clock, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_INVALID_ARG;
}
