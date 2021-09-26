/*
 * This file is part of the Aaru Data Preservation Suite.
 * Copyright (c) 2019-2021 Natalia Portillo.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <stdlib.h>

#include "library.h"
#include "crc64.h"
#include "simd.h"

AARU_EXPORT crc64_ctx* AARU_CALL crc64_init(void)
{
    int        i, slice;
    crc64_ctx* ctx = (crc64_ctx*)malloc(sizeof(crc64_ctx));

    if(!ctx) return NULL;

    ctx->crc = CRC64_ECMA_SEED;

    return ctx;
}

AARU_EXPORT int AARU_CALL crc64_update(crc64_ctx* ctx, const uint8_t* data, uint32_t len)
{
#if defined(__x86_64__) || defined(__amd64) || defined(_M_AMD64) || defined(_M_X64) || defined(__I386__) ||            \
    defined(__i386__) || defined(__THW_INTEL) || defined(_M_IX86)
    if(have_clmul())
    {
        ctx->crc = ~crc64_clmul(~ctx->crc, data, len);
        return 0;
    }
#endif

    // Unroll according to Intel slicing by uint8_t
    // http://www.intel.com/technology/comms/perfnet/download/CRC_generators.pdf
    // http://sourceforge.net/projects/slicing-by-8/

    if(!ctx || !data) return -1;

    uint64_t crc = ctx->crc;

    if(len > 4)
    {
        const uint8_t* limit;

        while((uintptr_t)(data)&3)
        {
            crc = crc64_table[0][*data++ ^ ((crc)&0xFF)] ^ ((crc) >> 8);
            --len;
        }

        limit = data + (len & ~(uint32_t)(3));
        len &= (uint32_t)(3);

        while(data < limit)
        {
            const uint32_t tmp = crc ^ *(const uint32_t*)(data);
            data += 4;

            crc = crc64_table[3][((tmp)&0xFF)] ^ crc64_table[2][(((tmp) >> 8) & 0xFF)] ^ ((crc) >> 32) ^
                  crc64_table[1][(((tmp) >> 16) & 0xFF)] ^ crc64_table[0][((tmp) >> 24)];
        }
    }

    while(len-- != 0) crc = crc64_table[0][*data++ ^ ((crc)&0xFF)] ^ ((crc) >> 8);

    ctx->crc = crc;
    return 0;
}

AARU_EXPORT int AARU_CALL crc64_final(crc64_ctx* ctx, uint64_t* crc)
{
    if(!ctx) return -1;

    *crc = ctx->crc ^ CRC64_ECMA_SEED;

    return 0;
}

AARU_EXPORT void AARU_CALL crc64_free(crc64_ctx* ctx)
{
    if(ctx) free(ctx);
}