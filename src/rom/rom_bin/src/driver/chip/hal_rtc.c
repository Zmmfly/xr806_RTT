/**
  * @file  hal_rtc.c
  * @author  XRADIO IOT WLAN Team
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

#include "rom/pm/pm.h"
#include "rom/driver/chip/hal_rtc.h"

#include "hal_base.h"

typedef struct {
	RTC_AlarmIRQCallback    callback;
	void                    *arg;
} RTC_AlarmPrivate;

static RTC_AlarmPrivate gRtcSecAlarmPriv;
static RTC_AlarmPrivate gRtcWDayAlarmPriv;

static __inline int RTC_WDayAlarmIsHHMMSSReadable(void)
{
	return !HAL_GET_BIT(RTC->CTRL, RTC_WDAY_ALARM_HHMMSS_ACCESS_BIT);
}

static __inline int RTC_IsDDHHMMSSReadable(void)
{
	return !HAL_GET_BIT(RTC->CTRL, RTC_DDHHMMSS_ACCESS_BIT);
}

static __inline int RTC_IsYYMMDDReadable(void)
{
	return !HAL_GET_BIT(RTC->CTRL, RTC_YYMMDD_ACCESS_BIT);
}

static __inline void RTC_SecAlarmSetAlarmTime(uint32_t sec)
{
	RTC->SEC_ALARM_LOAD_VAL = sec;
}

static __inline uint32_t RTC_SecAlarmGetCurrentTime(void)
{
	return RTC->SEC_ALARM_CUR_VAL;
}

static __inline void RTC_SecAlarmEnableIRQ(void)
{
	HAL_SET_BIT(RTC->SEC_ALARM_IRQ_EN, RTC_SEC_ALARM_IRQ_EN_BIT);
}

static __inline void RTC_SecAlarmDisableIRQ(void)
{
	HAL_CLR_BIT(RTC->SEC_ALARM_IRQ_EN, RTC_SEC_ALARM_IRQ_EN_BIT);
}

__nonxip_text
static __inline uint32_t RTC_SecAlarmIsEnableIRQ(void)
{
	return HAL_GET_BIT(RTC->SEC_ALARM_IRQ_EN, RTC_SEC_ALARM_IRQ_EN_BIT);
}

__nonxip_text
static __inline int RTC_SecAlarmIsPendingIRQ(void)
{
	return HAL_GET_BIT(RTC->SEC_ALARM_IRQ_STATUS, RTC_SEC_ALARM_IRQ_PENDING_BIT);
}

__nonxip_text
static __inline void RTC_SecAlarmClearPendingIRQ(void)
{
	HAL_SET_BIT(RTC->SEC_ALARM_IRQ_STATUS, RTC_SEC_ALARM_IRQ_PENDING_BIT);
}

static __inline void RTC_SecAlarmStart(void)
{
	HAL_SET_BIT(RTC->SEC_ALARM_EN, RTC_SEC_ALARM_EN_BIT);
}

__nonxip_text
static __inline void RTC_SecAlarmStop(void)
{
	HAL_CLR_BIT(RTC->SEC_ALARM_EN, RTC_SEC_ALARM_EN_BIT);
}

__nonxip_text
static __inline void RTC_SecAlarmEnableWakeup(void)
{
	HAL_SET_BIT(RTC->ALARM_WAKEUP_EN, RTC_SEC_ALARM_WAKEUP_EN_BIT);
}

__nonxip_text
static __inline void RTC_SecAlarmDisableWakeup(void)
{
	HAL_CLR_BIT(RTC->ALARM_WAKEUP_EN, RTC_SEC_ALARM_WAKEUP_EN_BIT);
}

static __inline void RTC_WDayAlarmSetAlarmTime(uint8_t hour,
                                                      uint8_t minute,
                                                      uint8_t second)
{
	HAL_ASSERT_PARAM(hour >= RTC_HOUR_MIN && hour <= RTC_HOUR_MAX);
	HAL_ASSERT_PARAM(minute >= RTC_MINUTE_MIN && minute <= RTC_MINUTE_MAX);
	HAL_ASSERT_PARAM(second >= RTC_SECOND_MIN && second <= RTC_SECOND_MAX);

	RTC->WDAY_ALARM_HHMMSS =
		(((uint32_t)hour & RTC_WDAY_ALARM_HOUR_VMASK) << RTC_WDAY_ALARM_HOUR_SHIFT) |
		(((uint32_t)minute & RTC_WDAY_ALARM_MINUTE_VMASK) << RTC_WDAY_ALARM_MINUTE_SHIFT) |
		(((uint32_t)second & RTC_WDAY_ALARM_SECOND_VMASK) << RTC_WDAY_ALARM_SECOND_SHIFT);
}

static __inline void RTC_WDayAlarmSetAlarmDay(uint8_t wdayMask)
{
	RTC->WDAY_ALARM_WDAY_EN = wdayMask & RTC_WDAY_ALARM_EN_MASK;
}

static __inline void RTC_WDayAlarmEnableIRQ(void)
{
	HAL_SET_BIT(RTC->WDAY_ALARM_IRQ_EN, RTC_WDAY_ALARM_IRQ_EN_BIT);
}

static __inline void RTC_WDayAlarmDisableIRQ(void)
{
	HAL_CLR_BIT(RTC->WDAY_ALARM_IRQ_EN, RTC_WDAY_ALARM_IRQ_EN_BIT);
}

__nonxip_text
static __inline uint32_t RTC_WDayAlarmIsEnableIRQ(void)
{
	return HAL_GET_BIT(RTC->WDAY_ALARM_IRQ_EN, RTC_WDAY_ALARM_IRQ_EN_BIT);
}

__nonxip_text
static int RTC_WDayAlarmIsPendingIRQ(void)
{
	return HAL_GET_BIT(RTC->WDAY_ALARM_IRQ_STATUS, RTC_WDAY_ALARM_IRQ_PENDING_BIT);
}

__nonxip_text
static void RTC_WDayAlarmClearPendingIRQ(void)
{
	HAL_SET_BIT(RTC->WDAY_ALARM_IRQ_STATUS, RTC_WDAY_ALARM_IRQ_PENDING_BIT);
}

__nonxip_text
static __inline void RTC_WDayAlarmEnableWakeup(void)
{
	HAL_SET_BIT(RTC->ALARM_WAKEUP_EN, RTC_WDAY_ALARM_WAKEUP_EN_BIT);
}

__nonxip_text
static __inline void RTC_WDayAlarmDisableWakeup(void)
{
	HAL_CLR_BIT(RTC->ALARM_WAKEUP_EN, RTC_WDAY_ALARM_WAKEUP_EN_BIT);
}

__nonxip_text
void RTC_SecAlarm_IRQHandler()
{
	RTC_SecAlarmStop();
	if (RTC_SecAlarmIsPendingIRQ()) {
		RTC_SecAlarmClearPendingIRQ();
		if (gRtcSecAlarmPriv.callback) {
			gRtcSecAlarmPriv.callback(gRtcSecAlarmPriv.arg);
		}
	}
}

__nonxip_text
void RTC_WDayAlarm_IRQHandler()
{
	if (RTC_WDayAlarmIsPendingIRQ()) {
		RTC_WDayAlarmClearPendingIRQ();
		if (gRtcWDayAlarmPriv.callback) {
			gRtcWDayAlarmPriv.callback(gRtcWDayAlarmPriv.arg);
		}
	}
}

/**
 * @brief Set the RTC date, including leaf year flag, year, month and month day
 * @param[in] isLeapYear Leap year flag set to the RTC.
 *     @arg !0 The year is a leap year
 *     @arg  0 The year is not a leap year
 * @param[in] year Year set to the RTC, [0, 255]
 * @param[in] month Month set to the RTC, [1, 12]
 * @param[in] mday Month day set to the RTC, [1, 31]
 * @return None
 *
 * @note The leap year flag is always set by the caller, but never changed after
 *       setting. So the leap year bit of the RTC maybe wrong.
 * @note The value of year is not a real year, but year's offset relative to
 *       the base year defined by the caller.
 * @note The correction of the combination of all the parameters is guaranteed
 *       by the caller.
 */
void HAL_RTC_SetYYMMDD(uint8_t isLeapYear, uint8_t year, uint8_t month, uint8_t mday)
{
	HAL_ASSERT_PARAM(year <= RTC_YEAR_MAX);
	HAL_ASSERT_PARAM(month >= RTC_MONTH_MIN && month <= RTC_MONTH_MAX);
	HAL_ASSERT_PARAM(mday >= RTC_MDAY_MIN && month <= RTC_MDAY_MAX);

	RTC->YYMMDD = (isLeapYear ? RTC_LEAP_YEAR_BIT : 0) |
				  (((uint32_t)year & RTC_YEAR_VMASK) << RTC_YEAR_SHIFT) |
				  (((uint32_t)month & RTC_MONTH_VMASK) << RTC_MONTH_SHIFT) |
				  (((uint32_t)mday & RTC_MDAY_VMASK) << RTC_MDAY_SHIFT);
}

/**
 * @brief Set the RTC weekday and time including hour, minute and second
 * @param[in] wday Weekday set to the RTC
 * @param[in] hour Hour set to the RTC, [0, 23]
 * @param[in] minute Minute set to the RTC, [0, 59]
 * @param[in] second Second set to the RTC, [0, 59]
 * @return None
 *
 * @note The correction of the weekday is guaranteed by the caller.
 */
void HAL_RTC_SetDDHHMMSS(RTC_WeekDay wday, uint8_t hour, uint8_t minute, uint8_t second)
{
	HAL_ASSERT_PARAM(hour >= RTC_HOUR_MIN && hour <= RTC_HOUR_MAX);
	HAL_ASSERT_PARAM(minute >= RTC_MINUTE_MIN && minute <= RTC_MINUTE_MAX);
	HAL_ASSERT_PARAM(second >= RTC_SECOND_MIN && second <= RTC_SECOND_MAX);

	RTC->DDHHMMSS = (((uint32_t)wday & RTC_WDAY_VMASK) << RTC_WDAY_SHIFT) |
				    (((uint32_t)hour & RTC_HOUR_VMASK) << RTC_HOUR_SHIFT) |
				    (((uint32_t)minute & RTC_MINUTE_VMASK) << RTC_MINUTE_SHIFT) |
				    (((uint32_t)second & RTC_SECOND_VMASK) << RTC_SECOND_SHIFT);
}

/**
 * @brief Get the RTC date, including leaf year flag, year, month and month day
 * @param[out] isLeapYear The RTC's leap year flag. Don't use it because it
 *                        maybe wrong.
 *     - 1 means the year is a leap year
 *     - 0 means the year is not a leap year
 * @param[out] year The RTC's Year
 * @param[out] month The RTC's Month
 * @param[out] mday The RTC's Month day
 * @return None
 *
 * @note Don't trust the RTC leap year flag, because it's never changed by the
 *       RTC hardware, and maybe wrong.
 * @note The RTC's year is not a real year, but year offset relative to the
 *       base year defined by the caller.
 */
void HAL_RTC_GetYYMMDD(uint8_t *isLeapYear, uint8_t *year, uint8_t *month, uint8_t *mday)
{
	uint32_t yymmdd;

	while (!RTC_IsYYMMDDReadable()) {
		HAL_MSleep(1);
	}

	yymmdd = RTC->YYMMDD;
	*isLeapYear = HAL_GET_BIT(yymmdd, RTC_LEAP_YEAR_BIT) ? 1 : 0;
	*year = HAL_GET_BIT_VAL(yymmdd, RTC_YEAR_SHIFT, RTC_YEAR_VMASK);
	*month = HAL_GET_BIT_VAL(yymmdd, RTC_MONTH_SHIFT, RTC_MONTH_VMASK);
	*mday = HAL_GET_BIT_VAL(yymmdd, RTC_MDAY_SHIFT, RTC_MDAY_VMASK);
}

/**
 * @brief Get the RTC weekday and time including hour, minute and second
 * @param[out] wday The RTC's Weekday
 * @param[out] hour The RTC's hour
 * @param[out] minute The RTC's minute
 * @param[out] second The RTC's second
 * @return None
 */
void HAL_RTC_GetDDHHMMSS(RTC_WeekDay *wday, uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	uint32_t ddhhmmss;

	while (!RTC_IsDDHHMMSSReadable()) {
		HAL_MSleep(1);
	}

	ddhhmmss = RTC->DDHHMMSS;
	*wday = (RTC_WeekDay)HAL_GET_BIT_VAL(ddhhmmss, RTC_WDAY_SHIFT, RTC_WDAY_VMASK);
	*hour = HAL_GET_BIT_VAL(ddhhmmss, RTC_HOUR_SHIFT, RTC_HOUR_VMASK);
	*minute = HAL_GET_BIT_VAL(ddhhmmss, RTC_MINUTE_SHIFT, RTC_MINUTE_VMASK);
	*second = HAL_GET_BIT_VAL(ddhhmmss, RTC_SECOND_SHIFT, RTC_SECOND_VMASK);
}

void HAL_RTC_SetLeapYear(uint8_t isLeapYear)
{
	if (isLeapYear) {
		HAL_SET_BIT(RTC->YYMMDD, RTC_LEAP_YEAR_BIT);
	} else {
		HAL_CLR_BIT(RTC->YYMMDD, RTC_LEAP_YEAR_BIT);
	}
}

/**
 * @brief Start the RTC second alarm once
 *
 * After starting, the RTC second alarm counts down from param->alarmSeconds
 * to zero, and trigger interrupt when it reach zero. After alarming, the
 * alarm stops automatically.
 *
 * @param[in] param Pointer to RTC_SecAlarmStartParam structure
 * @return None
 *
 */
void HAL_RTC_StartSecAlarm(const RTC_SecAlarmStartParam *param)
{
	/* reset the current value and wait it done */
	RTC_SecAlarmStop();
	while (RTC_SecAlarmGetCurrentTime() != 0) {
		HAL_MSleep(10);
	}

	RTC_SecAlarmSetAlarmTime(param->alarmSeconds);
	gRtcSecAlarmPriv.callback = param->callback;
	gRtcSecAlarmPriv.arg = param->arg;
	if (RTC_SecAlarmIsPendingIRQ()) {
		RTC_SecAlarmClearPendingIRQ();
	}
	RTC_SecAlarmEnableIRQ();
	HAL_NVIC_ConfigExtIRQ(RTC_SEC_ALARM_IRQn, RTC_SecAlarm_IRQHandler,
	                      NVIC_PERIPH_PRIO_DEFAULT);
	RTC_SecAlarmStart();
}

/**
 * @brief Stop the RTC second alarm
 * @return None
 */
void HAL_RTC_StopSecAlarm(void)
{
	RTC_SecAlarmStop();
	HAL_NVIC_DisableIRQ(RTC_SEC_ALARM_IRQn);
	RTC_SecAlarmDisableIRQ();
	if (RTC_SecAlarmIsPendingIRQ()) {
		RTC_SecAlarmClearPendingIRQ();
	}
	gRtcSecAlarmPriv.callback = NULL;
	gRtcSecAlarmPriv.arg = NULL;
}

/**
 * @brief Start the RTC weekday alarm
 *
 * After starting, the RTC weekday alarm will trigger interrupt when it reach
 * the configured weekday time. After alarming, the alarm continues to running.
 *
 * @param[in] param Pointer to RTC_WDayAlarmStartParam structure
 * @return None
 */
void HAL_RTC_StartWDayAlarm(const RTC_WDayAlarmStartParam *param)
{
	RTC_WDayAlarmSetAlarmDay(0);
	RTC_WDayAlarmSetAlarmTime(param->alarmHour, param->alarmMinute, param->alarmSecond);

	gRtcWDayAlarmPriv.callback = param->callback;
	gRtcWDayAlarmPriv.arg = param->arg;
	if (RTC_WDayAlarmIsPendingIRQ()) {
		RTC_WDayAlarmClearPendingIRQ();
	}
	RTC_WDayAlarmEnableIRQ();
	HAL_NVIC_ConfigExtIRQ(RTC_WDAY_ALARM_IRQn, RTC_WDayAlarm_IRQHandler,
	                      NVIC_PERIPH_PRIO_DEFAULT);
	RTC_WDayAlarmSetAlarmDay(param->alarmWDayMask);
}

/**
 * @brief Stop the RTC weekday alarm
 * @return None
 */
void HAL_RTC_StopWDayAlarm(void)
{
	RTC_WDayAlarmSetAlarmDay(0);
	HAL_NVIC_DisableIRQ(RTC_WDAY_ALARM_IRQn);
	RTC_WDayAlarmDisableIRQ();
	if (RTC_WDayAlarmIsPendingIRQ()) {
		RTC_WDayAlarmClearPendingIRQ();
	}
	gRtcWDayAlarmPriv.callback = NULL;
	gRtcWDayAlarmPriv.arg = NULL;
}

#ifdef CONFIG_PM
static int rtc_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		if (RTC_SecAlarmIsEnableIRQ()) {
			RTC_SecAlarmEnableWakeup();
		}
		if (RTC_WDayAlarmIsEnableIRQ()) {
			RTC_WDayAlarmEnableWakeup();
		}
		HAL_DBG("%s ok\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

static int rtc_resume(struct soc_device *dev, enum suspend_state_t state)
{
	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		if (RTC_SecAlarmIsEnableIRQ()) {
			RTC_SecAlarmDisableWakeup();
		}
		if (RTC_WDayAlarmIsEnableIRQ()) {
			RTC_WDayAlarmDisableWakeup();
		}
		HAL_DBG("%s ok\n", __func__);
		break;
	default:
		break;
	}

	return 0;
}

static const struct soc_device_driver rtc_drv = {
	.name = "rtc",
	.suspend_noirq = rtc_suspend,
	.resume_noirq = rtc_resume,
};

static struct soc_device rtc_dev = {
	.name = "rtc",
	.driver = &rtc_drv,
};

#define RTC_DEV (&rtc_dev)
#endif /* CONFIG_PM */

/**
 * @brief Init the RTC module
 * @return None
 */
void HAL_RTC_Init(void)
{
#ifdef CONFIG_PM
	pm_register_ops(RTC_DEV);

	if (RTC_SecAlarmIsEnableIRQ()) {
		RTC_SecAlarmDisableWakeup();
		HAL_NVIC_ConfigExtIRQ(RTC_SEC_ALARM_IRQn, RTC_SecAlarm_IRQHandler,
		                      NVIC_PERIPH_PRIO_DEFAULT);
	}
	if (RTC_WDayAlarmIsEnableIRQ()) {
		RTC_WDayAlarmDisableWakeup();
		HAL_NVIC_ConfigExtIRQ(RTC_WDAY_ALARM_IRQn, RTC_WDayAlarm_IRQHandler,
		                      NVIC_PERIPH_PRIO_DEFAULT);
	}
#endif
}

/**
 * @brief Convert RTC's free running counts to time value (in microsecond)
 * @param[in] cnt RTC's free running counts to be converted
 * @return Time value (in microsecond) after converted
 */
__nonxip_text
uint64_t HAL_RTC_FreeRunCntToTime(uint64_t cnt)
{
	/* convert counts to us, it may overflow at about 17 years */
	return (cnt * 1000000 / HAL_GetLFClock(PRCM_LFCLK_MODULE_RTC));
}

/**
 * @brief Convert time value (in microsecond) to RTC's Free running counts
 * @param[in] us Time value (in microsecond) to be converted
 * @return RTC's Free running counts after converted
 */
__nonxip_text
uint64_t HAL_RTC_FreeRunTimeToCnt(uint64_t us)
{
	return (us * HAL_GetLFClock(PRCM_LFCLK_MODULE_RTC) / 1000000);
}

/**
 * @brief Get The counts of the RTC's Free running counter
 *
 * Free running counter is a 48-bit counter which is driven by LFCLK and starts
 * to count as soon as the system reset is released and the LFCLK is ready.
 *
 * @return The counts of RTC's Free running counter
 */
__nonxip_text
uint64_t HAL_RTC_GetFreeRunCnt(void)
{
	uint64_t cnt;
	uint32_t cntLow1, cntLow2, cntHigh;

	cntLow1 = RTC->FREERUN_CNT_L;
	cntHigh = RTC->FREERUN_CNT_H;
	cntLow2 = RTC->FREERUN_CNT_L;
	if (cntLow2 < cntLow1) {
		/* counter's low 32-bit overflow, get high 16-bit again */
		cntHigh = RTC->FREERUN_CNT_H;
		cnt = ((uint64_t)cntHigh << 32) | cntLow2;
	} else {
		cnt = ((uint64_t)cntHigh << 32) | cntLow1;
	}

	return cnt;
}

/**
 * @brief Get the time value (in microsecond) of the RTC's Free running counter
 *
 * Free running counter is a 48-bit counter which is driven by LFCLK and starts
 * to count as soon as the system reset is released and the LFCLK is ready.
 *
 * The time unit of the counter is: (10^6 / LFCLK) us
 *
 * @return The time value (in microsecond) of the RTC's Free running counter.
 *         Its accuracy is about 32 us.
 */
__nonxip_text
uint64_t HAL_RTC_GetFreeRunTime(void)
{
	return HAL_RTC_FreeRunCntToTime(HAL_RTC_GetFreeRunCnt());
}
