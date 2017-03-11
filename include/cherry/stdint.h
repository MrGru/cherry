/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __CHERRY_STDINT_H__
#define __CHERRY_STDINT_H__

#include <stdint.h>

typedef int8_t          i8;
typedef int16_t         i16;
typedef int32_t         i32;
typedef int64_t         i64;

typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;
typedef uint64_t        u64;

#define for_i(i, n) for(i = 0; i < n; ++i)

#define for_i_step(i, s, n) for(i = 0; i < n; i += s)

#define for_i_from_step(i, f, s, n) for(i = f; i < n; i += s)

#define back_i(i, n) for(i = n - 1; i >= 0; i--)

#define for_i_from(i, f, n) for(i = f; i < n; ++i)

#define back_i_to(i, n, t) for(i = n - 1; i >= t; i--)

#endif
