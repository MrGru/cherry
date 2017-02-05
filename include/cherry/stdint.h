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

#define back_i(i, n) for(i = n - 1; i >= 0; i--)

#endif
