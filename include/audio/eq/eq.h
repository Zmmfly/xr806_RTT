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

#ifndef _EQ_H_
#define _EQ_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* EQ filter type definition */
typedef enum {
	/* low pass shelving filter */
	LOWPASS_SHELVING,
	/* band pass peak filter */
	BANDPASS_PEAK,
	/* high pass shelving filter */
	HIHPASS_SHELVING,
	LOWPASS,
	HIGHPASS
} eq_ftype_t;

/* equalizer parameters */
typedef struct {
	/* boost/cut gain in dB */
	float G;
	/* cutoff/center frequency in Hz */
	int fc;
	/* quality factor */
	float Q;
	/* filter type */
	eq_ftype_t type;
} eq_core_prms_t;

typedef struct {
	/*num of items(biquad)*/
	int biq_num;
	/* sampling rate */
	int sampling_rate;
	/* channel num */
	int chan;
	/* eq parameters for generate IIR coeff*/
	eq_core_prms_t *core_prms;
} eq_prms_t;

typedef struct {
	int biq_num; /*num of items(biquad)*/
	eq_core_prms_t *core_params;
} eq_user_params_t;

/**
 * @brief Create the equalizer object
 * @param[in] params Desired frequency response array
 * @retval The equalizer handle
 */
void *eq_create(void *params);

/**
 * @brief Equalizer processing function
 * @handle[in] handle Equalzier handle
 * @handle[in,out] x Input signal
 * @handle[in] len Input length(in samples)
 * @retval None
 */
void eq_process(void *handle, short *x, int len);

/**
 * @brief Destroy the equalizer object
 * @param[in] handle Equalizer handle
 * @retval None
 */
void eq_destroy(void *handle);

#ifdef __cplusplus
}
#endif

#endif

