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

#ifndef _IMAGE_FDCM_H_
#define _IMAGE_FDCM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FDCM handle definition
 */
typedef struct fdcm_handle {
	uint32_t    flash;
	uint32_t    addr;
	uint32_t    size;
} fdcm_handle_t;

/**
 * @brief Open an area in a flash to be managed by FDCM module
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the area
 * @param[in] size Size of the area
 * @retval Pointer to the FDCM handle, NULL on failure
 *
 * @note The area must be aligned to the flash erase block
 */
fdcm_handle_t *fdcm_open(uint32_t flash, uint32_t addr, uint32_t size);

/**
 * @brief Read a data chunk from the specified area
 * @param[in] hdl Pointer to the FDCM handle
 * @param[in] data Pointer to the data
 * @param[in] data_size Size of the data
 * @return Number of bytes read
 */
uint32_t fdcm_read(fdcm_handle_t *hdl, void *data, uint16_t data_size);

/**
 * @brief Write a data chunk to the specified area
 * @param[in] hdl Pointer to the FDCM handle
 * @param[in] data Pointer to the data
 * @param[in] data_size Size of the data
 * @return Number of bytes written
 */
uint32_t fdcm_write(fdcm_handle_t *hdl, const void *data, uint16_t data_size);

/**
 * @brief Erase the whole FDCM area
 * @param[in] hdl Pointer to the FDCM handle
 * @return 0 on success, -1 on failure
 */
int fdcm_erase(fdcm_handle_t *hdl);

/**
 * @brief Close the area managed by FDCM module
 * @param[in] hdl Pointer to the FDCM handle
 * @return None
 */
void fdcm_close(fdcm_handle_t *hdl);

#ifdef __cplusplus
}
#endif

#endif /* _IMAGE_FDCM_H_ */
