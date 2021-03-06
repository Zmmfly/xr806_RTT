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
#include "common/framework/platform_init.h"
#include "common/apps/buttons/buttons.h"
#include "common/apps/buttons/buttons_low_level.h"

/* the buttons in board, need see "board/xradio_storybot/board_config.c" */
#define PLAY   KEY1
#define PREV   KEY2
#define NEXT   KEY3
#define MODE   KEY4
#define AI     KEY5

static button_handle *short_button;
static button_handle *long_button0;
static button_handle *long_button1;
static button_handle *short_long_button;
static button_handle *combined_long_button0;
static button_handle *repeat_long_button;

static void short_button_cb(BUTTON_STATE sta, void *arg)
{
	/* short button only has the RELEASE state */
	if (sta == RELEASE)
		printf("short button for PLAY/PAUSE has release\n");
}

static void long_button0_cb(BUTTON_STATE sta, void *arg)
{
	/* long button has the PRESS/RELEASE state */
	if (sta == PRESS)
		printf("long button for PRE/VOL+ has press\n");
	else if (sta == RELEASE)
		printf("long button for PRE/VOL+ has release\n");
}

static void long_button1_cb(BUTTON_STATE sta, void *arg)
{
	/* long button has the PRESS/RELEASE state */
	if (sta == PRESS)
		printf("long button for NEXT/VOL- has press\n");
	else if (sta == RELEASE)
		printf("long button for NEXT/VOL- has release\n");
}

static void short_long_button_cb(BUTTON_STATE sta, void *arg)
{
	/* short_long button has the PRESS/RELEASE/REPEAT_RELEASE state */
	if (sta == PRESS)
		printf("long button for MODE has press\n");
	else if (sta == RELEASE)
		printf("short button for MODE has release\n");
	else if (sta == REPEAT_RELEASE)
		printf("long button for MODE has release\n");
}

static void combined_long_button0_cb(BUTTON_STATE sta, void *arg)
{
	/* combined button has the PRESS/RELEASE state */
	if (sta == PRESS)
		printf("combined button for NEXT+PREV has press\n");
	else if (sta == RELEASE)
		printf("combined button for NEXT+PREV has release\n");
}

static void repeat_long_button_cb(BUTTON_STATE sta, void *arg)
{
	/* repeat long button has the PRESS/RELEASE/REPEAT_PRESS state */
	if (sta == PRESS)
		printf("repeat long button for AI has press\n");
	else if (sta == RELEASE)
		printf("repeat long button for AI has release\n");
	else if (sta == REPEAT_PRESS)
		printf("repeat long button for AI has repeat release\n");

}

int example_buttons_init(void)
{
	int ret;

	/* register the low level button interface */
	button_impl_t impl = {
		buttons_low_level_init,
		buttons_low_level_get_state,
		buttons_low_level_wait_semaphore,
		buttons_low_level_release_semaphore,
		buttons_low_level_deinit
	};
	/* set buttons thread priority and stack size */
	buttons_set_thread_priority(4);
	buttons_set_thread_stack_size(1024);

	/* init buttons, will init the low level buttons, ad buttons and gpio buttons */
	ret = buttons_init(&impl);

	return ret;
}

void example_buttons_start(void)
{
	/* start buttons object */
	short_button->start(short_button);
	long_button0->start(long_button0);
	long_button1->start(long_button1);
	short_long_button->start(short_long_button);
	combined_long_button0->start(combined_long_button0);
	repeat_long_button->start(repeat_long_button);
}

void example_buttons_stop(void)
{
	/* stop buttons object */
	short_button->stop(short_button);
	long_button0->stop(long_button0);
	long_button1->stop(long_button1);
	short_long_button->stop(short_long_button);
	combined_long_button0->stop(combined_long_button0);
	repeat_long_button->stop(repeat_long_button);
}

void example_buttons_destroy(void)
{
	/* destory buttons */
	if (short_button)
		short_button->destroy(short_button);
	if (long_button0)
		long_button0->destroy(long_button0);
	if (long_button1)
		long_button1->destroy(long_button1);
	if (short_long_button)
		short_long_button->destroy(short_long_button);
	if (combined_long_button0)
		combined_long_button0->destroy(combined_long_button0);
	if (repeat_long_button)
		repeat_long_button->destroy(repeat_long_button);
}

int example_buttons_create(void)
{
	/* create buttons object */

	/* when this button pressed, it will trigger */
	short_button = create_short_button(PLAY);
	/* after a long press of 50ms, it will trigger */
	long_button0 = create_long_button(PREV, 50);
	/* after a long press of 500ms, it will trigger */
	long_button1 = create_long_button(NEXT, 500);

	short_long_button = create_short_long_button(MODE, 500);
	/*
	 * NEXT and PREV are ad buttons, so if we want to create the combination
	 * button of NEXT and PREV, we should set the ad value in boardconfig.c when
	 * NEXT and PREV are pressed, or can not recognize the combination button of NEXT and PREV.
	 */
	combined_long_button0 = create_combined_long_button(NEXT | PREV, 50);

	repeat_long_button = create_repeat_long_button(AI, 500, 300);

	if (!short_button || !long_button0 || !long_button1 || !short_long_button ||
		!combined_long_button0 || !repeat_long_button) {
			printf("at least one button create err\n");
			goto exit;
	}

	/* set buttons callback */
	short_button->cb = short_button_cb;
	long_button0->cb = long_button0_cb;
	long_button1->cb = long_button1_cb;
	short_long_button->cb = short_long_button_cb;
	combined_long_button0->cb = combined_long_button0_cb;
	repeat_long_button->cb = repeat_long_button_cb;

	return 0;

exit:
	example_buttons_destroy();
	return -1;
}

int main(void)
{
	platform_init();

	printf("button demo started.\n");

	example_buttons_init();
	example_buttons_create();
	example_buttons_start();

	return 0;
}
