/**
 * @file os_semaphore.c
 * @author XRADIO IOT WLAN Team
 */

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

#include "kernel/os/os_semaphore.h"
#include "os_util.h"
// #include "semphr.h"
#include <rtthread.h>

OS_Status OS_SemaphoreCreate(OS_Semaphore_t *sem, uint32_t initCount, uint32_t maxCount)
{
	// OS_HANDLE_ASSERT(!OS_SemaphoreIsValid(sem), sem->handle);

	// sem->handle = xSemaphoreCreateCounting(maxCount, initCount);
	sem->handle = rt_sem_create("NULL", initCount, RT_IPC_FLAG_PRIO);

	if (sem->handle == NULL) {
		OS_ERR("err %"OS_HANDLE_F"\n", sem->handle);
		return OS_FAIL;
	}

	return OS_OK;
}

OS_Status OS_SemaphoreCreateBinary(OS_Semaphore_t *sem)
{
//	OS_HANDLE_ASSERT(!OS_SemaphoreIsValid(sem), sem->handle);

	// sem->handle = xSemaphoreCreateBinary();
	sem->handle = rt_sem_create(NULL, 0, RT_IPC_FLAG_PRIO);
	if (sem->handle == NULL) {
		OS_ERR("err %"OS_HANDLE_F"\n", sem->handle);
		return OS_FAIL;
	}

	return OS_OK;
}

OS_Status OS_SemaphoreDelete(OS_Semaphore_t *sem)
{
	OS_HANDLE_ASSERT(OS_SemaphoreIsValid(sem), sem->handle);

	// vSemaphoreDelete(sem->handle);
	rt_sem_delete(sem->handle);
	OS_SemaphoreSetInvalid(sem);
	return OS_OK;
}

OS_Status OS_SemaphoreWait(OS_Semaphore_t *sem, OS_Time_t waitMS)
{
	// BaseType_t ret;
	// BaseType_t taskWoken;
	rt_err_t ret;
	OS_Time_t ms = OS_IsISRContext() ? 0 : waitMS;

	OS_HANDLE_ASSERT(OS_SemaphoreIsValid(sem), sem->handle);

	ret = rt_sem_take(sem->handle, ms);
	if (ret != RT_EOK) {
		OS_DBG("%s() fail @ %d, %"OS_TIME_F" ms\n", __func__, __LINE__, waitMS);
		return OS_E_TIMEOUT;
	}

	return OS_OK;
}

OS_Status OS_SemaphoreRelease(OS_Semaphore_t *sem)
{
	// BaseType_t ret;
	// BaseType_t taskWoken;
	rt_err_t ret;

	OS_HANDLE_ASSERT(OS_SemaphoreIsValid(sem), sem->handle);

	ret = rt_sem_release(sem->handle);
	if (ret != RT_EOK) {
		OS_DBG("%s() fail @ %d\n", __func__, __LINE__);
		return OS_FAIL;
	}

	return OS_OK;
}
