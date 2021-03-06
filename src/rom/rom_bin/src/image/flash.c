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

#include "rom/driver/chip/hal_flash.h"
#include "rom/image/flash.h"

#include "image_debug.h"

#define FLASH_OPEN_TIMEOUT	(5000)

#define BLOCK_SIZE_4K  (4096)  //  4 * 1024
#define BLOCK_SIZE_32K (32768) // 32 * 1024
#define BLOCK_SIZE_64K (65536) // 64 * 1024

typedef struct flash_erase_param {
	int32_t         block_size;
	FlashEraseMode	erase_mode;
} flash_erase_param_t;

static const flash_erase_param_t s_flash_erase_param[] = {
	{ (64 * 1024), FLASH_ERASE_64KB },
	{ (32 * 1024), FLASH_ERASE_32KB },
	{ ( 4 * 1024), FLASH_ERASE_4KB  },
};

#define FLASH_ERASE_PARAM_CNT \
	(sizeof(s_flash_erase_param) / sizeof(s_flash_erase_param[0]))

/**
 * @brief Read/write an amount of data from/to flash
 * @param[in] flash Flash device number
 * @param[in/out] addr flash address to be read/written
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be read/written
 * @param[in] do_write Read or write
 * @return Number of bytes read/written
 */
uint32_t rom_flash_rw(uint32_t flash, uint32_t addr,
                      void *buf, uint32_t size, int do_write)
{
	HAL_Status status;

	status = HAL_Flash_Open(flash, FLASH_OPEN_TIMEOUT);
	if (status != HAL_OK) {
		FLASH_ERR("open %u fail\n", flash);
		return 0;
	}

	if (do_write) {
		status = HAL_Flash_Write(flash, addr, buf, size);
	} else {
		status = HAL_Flash_Read(flash, addr, buf, size);
	}

	HAL_Flash_Close(flash);
	if (status != HAL_OK) {
		FLASH_ERR("%s fail, (%u, %#x, %u), %p\n", do_write ? "write" : "read",
				  flash, addr, size, buf);
		return 0;
	}
	return size;
}

/**
 * @brief Get erase block size for the specified area
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the specified area
 * @param[in] size Size of the specified area
 * @return Erase block size, -1 on misaligned
 */
int32_t rom_flash_get_erase_block(uint32_t flash, uint32_t addr, uint32_t size)
{
	int i;
	int32_t block_size;
	FlashEraseMode erase_mode;
	uint32_t start;

	for (i = 0; i < FLASH_ERASE_PARAM_CNT; ++i) {
		block_size = s_flash_erase_param[i].block_size;
		erase_mode = s_flash_erase_param[i].erase_mode;
		if ((size >= block_size) &&
		    ((size & (block_size - 1)) == 0) &&
		    (HAL_Flash_MemoryOf(flash, erase_mode, addr, &start) == HAL_OK) &&
		    (addr == start)) {
		    break;
		}
	}

	if (i >= FLASH_ERASE_PARAM_CNT) {
		FLASH_ERR("(%u, %#x, %u) misaligned\n", flash, addr, size);
		return -1;
	}
	return block_size;
}

/**
 * @brief Erase a specified area in flash
 * @param[in] flash Flash device number
 * @param[in] addr Start address of the specified area
 * @param[in] size Size of the specified area
 * @return 0 on success, -1 on failure
 */
int rom_flash_erase(uint32_t flash, uint32_t addr, uint32_t size)
{
	struct FlashDev *dev;
	HAL_Status status;
	int ret = 0;
	uint32_t flash_erase_mode_support;

	uint32_t addr_4k_ahead_start = 0;
	uint32_t addr_4k_ahead_end = 0;
	uint32_t addr_32k_ahead_start = 0;
	uint32_t addr_32k_ahead_end = 0;
	uint32_t addr_64k_start = 0;

	uint32_t addr_4k_behind_start = 0;
	uint32_t addr_4k_behind_end = 0;
	uint32_t addr_32k_behind_start = 0;
	uint32_t addr_32k_behind_end = 0;
	uint32_t addr_64k_end = 0;

	uint32_t size_4k_ahead = 0;
	uint32_t size_32k_ahead = 0;
	uint32_t size_4k_behind = 0;
	uint32_t size_32k_behind = 0;
	uint32_t size_64k = 0;

	if (addr % BLOCK_SIZE_4K || size % BLOCK_SIZE_4K) {
		FLASH_ERR("addr:0x%x(%dK) or size:0x%x(%dK) is not 4K alignment!!!\n",
					addr, addr / 1024, size, size / 1024);
		return -1;
	}

	if (HAL_Flash_Open(flash, FLASH_OPEN_TIMEOUT) != HAL_OK) {
		FLASH_ERR("open %d fail\n", flash);
		return -1;
	}

	dev = getFlashDev(flash);
	flash_erase_mode_support = dev->chip->cfg.mEraseSizeSupport;

	addr_4k_ahead_start = addr;
	addr_4k_behind_end = addr + size;

	if (size >= BLOCK_SIZE_64K && (flash_erase_mode_support & FLASH_ERASE_64KB)) {
		addr_64k_start = (addr + BLOCK_SIZE_64K -1) & ~(BLOCK_SIZE_64K - 1); // Align up
		addr_64k_end = (addr + size) & ~(BLOCK_SIZE_64K - 1); // Align down
		size_64k = addr_64k_end - addr_64k_start;

		addr_32k_ahead_start = (addr + BLOCK_SIZE_32K -1) & ~(BLOCK_SIZE_32K - 1); // Align up
		addr_32k_ahead_end = addr_64k_start;
		size_32k_ahead = addr_32k_ahead_end - addr_32k_ahead_start;

		addr_32k_behind_start = addr_64k_end;
		addr_32k_behind_end = (addr + size) & ~(BLOCK_SIZE_32K - 1); // Align down
		size_32k_behind = addr_32k_behind_end - addr_32k_behind_start;

		addr_4k_ahead_end = addr_32k_ahead_start;
		size_4k_ahead = addr_4k_ahead_end - addr_4k_ahead_start;

		addr_4k_behind_start = addr_32k_behind_end;
		size_4k_behind = addr_4k_behind_end - addr_4k_behind_start;

	} else if (size >= BLOCK_SIZE_32K && (flash_erase_mode_support & FLASH_ERASE_32KB)) {
		addr_32k_ahead_start = (addr + BLOCK_SIZE_32K -1) & ~(BLOCK_SIZE_32K - 1); // Align up
		addr_32k_ahead_end = (addr + size) & ~(BLOCK_SIZE_32K - 1); // Align down
		size_32k_ahead = addr_32k_ahead_end - addr_32k_ahead_start;

		addr_32k_behind_start = addr_32k_behind_end = addr_32k_ahead_end;
		size_32k_behind = 0;

		addr_4k_ahead_end = addr_32k_ahead_start;
		size_4k_ahead = addr_4k_ahead_end - addr_4k_ahead_start;

		addr_4k_behind_start = addr_32k_ahead_end;
		size_4k_behind = addr_4k_behind_end - addr_4k_behind_start;
	} else if (size >= BLOCK_SIZE_4K && (flash_erase_mode_support & FLASH_ERASE_4KB)) {
		size_4k_ahead = size;
		size_4k_behind = 0;
	}

	FLASH_DBG("start_addr:0x%x(%dK) end_addr:0x%x(%dK) earse_size:0x%x(%dK)\n",
				addr, addr / 1024,addr + size, (addr + size)/1024, size, size / 1024);

	if (size_4k_ahead > 0) {
		FLASH_DBG("erase the ahead 4k area, start_addr:0x%x(%dK) end_addr:0x%x(%dK) size:0x%x(%dK)\n",
						addr_4k_ahead_start, addr_4k_ahead_start / 1024,
						addr_4k_ahead_end, addr_4k_ahead_end / 1024,
						size_4k_ahead, size_4k_ahead / 1024);
		if ((status = HAL_Flash_Erase(flash, FLASH_ERASE_4KB, addr_4k_ahead_start,
						size_4k_ahead / BLOCK_SIZE_4K)) != HAL_OK) {
			FLASH_ERR("earse fail\n");
			ret = -1;
			goto out;
		}
	}
	if (size_4k_behind > 0) {
		FLASH_DBG("erase the behind 4k area, start_addr:0x%x(%dK) end_addr:0x%x(%dK) size:0x%x(%dK)\n",
						addr_4k_behind_start, addr_4k_behind_start / 1024,
						addr_4k_behind_end, addr_4k_behind_end / 1024,
						size_4k_behind, size_4k_behind / 1024);
		if ((status = HAL_Flash_Erase(flash, FLASH_ERASE_4KB, addr_4k_behind_start,
						size_4k_behind / BLOCK_SIZE_4K)) != HAL_OK) {
			FLASH_ERR("earse fail\n");
			ret = -1;
			goto out;
		}
	}

	if (size_32k_ahead > 0) {
		FLASH_DBG("erase the ahead 32k area, start_addr:0x%x(%dK) end_addr:0x%x(%dK) size:0x%x(%dK)\n",
						addr_32k_ahead_start, addr_32k_ahead_start / 1024,
						addr_32k_ahead_end, addr_32k_ahead_end / 1024,
						size_32k_ahead, size_32k_ahead / 1024);
		if ((status = HAL_Flash_Erase(flash, FLASH_ERASE_32KB, addr_32k_ahead_start,
						size_32k_ahead / BLOCK_SIZE_32K)) != HAL_OK) {
			FLASH_ERR("earse fail\n");
			ret = -1;
			goto out;
		}
	}
	if (size_32k_behind > 0) {
		FLASH_DBG("erase the behind 32k area, start_addr:0x%x(%dK) end_addr:0x%x(%dK) size:0x%x(%dK)\n",
				addr_32k_behind_start, addr_32k_behind_start / 1024,
				addr_32k_behind_end, addr_32k_behind_end / 1024,
				size_32k_behind, size_32k_behind / 1024);
		if ((status = HAL_Flash_Erase(flash, FLASH_ERASE_32KB, addr_32k_behind_start,
						size_32k_behind / BLOCK_SIZE_32K)) != HAL_OK) {
			FLASH_ERR("earse fail\n");
			ret = -1;
			goto out;
		}
	}

	if (size_64k > 0) {
		FLASH_DBG("erase the middle 64k area, start_addr:0x%x(%dK) end_addr:0x%x(%dK) size:0x%x(%dK)\n",
				addr_64k_start, addr_64k_start / 1024, addr_64k_end, addr_64k_end / 1024,
				size_64k, size_64k / 1024);
		if ((status = HAL_Flash_Erase(flash, FLASH_ERASE_64KB, addr_64k_start,
						size_64k / BLOCK_SIZE_64K)) != HAL_OK) {
			FLASH_ERR("earse fail\n");
			ret = -1;
			goto out;
		}
	}

out:
	HAL_Flash_Close(flash);

	return ret;
}

