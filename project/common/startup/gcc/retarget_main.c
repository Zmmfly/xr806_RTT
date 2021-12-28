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

#include <stdio.h>
#include "kernel/os/os_thread.h"
#include "kernel/include/los_task.h"
#include "driver/chip/system_chip.h"
#include "common/framework/platform_init.h"
#ifdef CONFIG_TRUSTZONE
#include "trustzone/nsc_table.h"
#endif
#ifdef CONFIG_OS_DEBUG_CPU_USAGE
#include "kernel/os/os_cpuusage.h"
#endif

extern int console_write(uint8_t *buf, int32_t len);
extern void OHOS_SystemInit(void);
static OS_Thread_t g_main_thread;

static void main_task(void *arg)
{
#ifdef CONFIG_TRUSTZONE
	portALLOCATE_SECURE_CONTEXT(configMAXIMAL_SECURE_STACK_SIZE);

	TZ_ExceptionRegister_NSC((tz_remote_call_t *)OS_ThreadList);
#endif

#ifdef CONFIG_OS_DEBUG_CPU_USAGE
	OS_CpuUsageInit(CONFIG_OS_DEBUG_CPU_USAGE_SECONDS);
#endif
#if (!defined CONFIG_BOOTLOADER) && (!defined CONFIG_ETF)
	printf("\
====================================================================\n\
	Hello! OpenHarmony!\n\
	System tag : OpenHarmony 1.1.2_LTS\n\
====================================================================\n\
	\n");
	platform_init();

	OHOS_SystemInit();
	//printf("$>Welcome to Wlan Bluetooth Console Tools ......\n");
	//console_write((uint8_t *)"$>", 2);
#else
	extern int __real_main(void);
	__real_main();
#endif
	OS_ThreadDelete(&g_main_thread);
}

void main_task_start(void)
{
	SystemCoreClockUpdate();
	__asm volatile(
		//"cpsie i \n"
		"cpsie f \n");

	if (OS_ThreadCreate(&g_main_thread, "main", main_task, NULL,
			    PRJCONF_MAIN_THREAD_PRIO,
			    PRJCONF_MAIN_THREAD_STACK_SIZE) != OS_OK) {
		printf("[ERR] create main task failed\n");
	}

	__set_PSPLIM(0);
	OS_ThreadStartScheduler();

	while (1) {
		printf("error\n");
	}
}