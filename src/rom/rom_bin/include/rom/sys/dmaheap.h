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

#ifndef _ROM_DMAHEAP_H_
#define _ROM_DMAHEAP_H_

#include "sys/dma_heap.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ROM
#undef dma_malloc
#define dma_malloc \
    RAM_TBL_FUN(void *(*)( size_t size , uint32_t flag), dma_malloc)

#undef dma_free
#define dma_free \
    RAM_TBL_FUN(void (*)( void *ptr , uint32_t flag ), dma_free)

#undef dma_calloc
#define dma_calloc \
    RAM_TBL_FUN(void *(*)( size_t nmemb, size_t size, uint32_t flag ), dma_calloc)

#undef dma_realloc
#define dma_realloc \
    RAM_TBL_FUN(void *(*)( void *ptr, size_t size, uint32_t flag ), dma_realloc)

#endif /* CONFIG_ROM */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_DMAHEAP_H_ */