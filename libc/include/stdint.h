#ifndef _STDINT_H_
#define _STDINT_H_

/* Exact types */
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long   uint64_t;

typedef signed char     int8_t;
typedef signed short    int16_t;
typedef signed int      int32_t;
typedef signed long     int64_t;

/* "least" types */
typedef uint8_t         uint_least8_t;
typedef uint16_t        uint_least16_t;
typedef uint32_t        uint_least32_t;
typedef uint64_t        uint_least64_t;

typedef int8_t          int_least8_t;
typedef int16_t         int_least16_t;
typedef int32_t         int_least32_t;
typedef int64_t         int_least64_t;

/* "fast" types */
typedef uint8_t         uint_fast8_t;
typedef uint32_t        uint_fast16_t;
typedef uint32_t        uint_fast32_t;
typedef uint64_t        uint_fast64_t;

typedef int8_t          int_fast8_t;
typedef int32_t         int_fast16_t;
typedef int32_t         int_fast32_t;
typedef int64_t         int_fast64_t;

/* "ptr" types */
typedef int32_t         intptr_t;
typedef uint32_t        uintptr_t;

/* "max" types */
typedef int64_t         intmax_t;
typedef uint64_t        uintmax_t;

/* MAX and MIN macros */
#define INT8_MIN          (-0x7f - 1)
#define INT16_MIN         (-0x7fff - 1)
#define INT32_MIN         (-0x7fffffff - 1)
#define INT64_MIN         (-0x7fffffffffffffff - 1)

#define INT8_MAX          0x7f
#define INT16_MAX         0x7fff
#define INT32_MAX         0x7fffffff
#define INT64_MAX         0x7fffffffffffffff

#define UINT8_MAX         0xff
#define UINT16_MAX        0xffff
#define UINT32_MAX        0xffffffff
#define UINT64_MAX        0xffffffffffffffff

#define INT_LEAST8_MIN    (-0x7f - 1)
#define INT_LEAST16_MIN   (-0x7fff - 1)
#define INT_LEAST32_MIN   (-0x7fffffff - 1)
#define INT_LEAST64_MIN   (-0x7fffffffffffffff - 1)

#define INT_LEAST8_MAX    0x7f
#define INT_LEAST16_MAX   0x7fff
#define INT_LEAST32_MAX   0x7fffffff
#define INT_LEAST64_MAX   0x7fffffffffffffff

#define UINT_LEAST8_MAX   0xff
#define UINT_LEAST16_MAX  0xffff
#define UINT_LEAST32_MAX  0xffffffff
#define UINT_LEAST64_MAX  0xffffffffffffffff

#define INT_FAST8_MIN     (-0x7f - 1)
#define INT_FAST16_MIN    (-0x7fff - 1)
#define INT_FAST32_MIN    (-0x7fffffff - 1)
#define INT_FAST64_MIN    (-0x7fffffffffffffff - 1)

#define INT_FAST8_MAX     0x7f
#define INT_FAST16_MAX    0x7fff
#define INT_FAST32_MAX    0x7fffffff
#define INT_FAST64_MAX    0x7fffffffffffffff

#define UINT_FAST8_MAX    0xff
#define UINT_FAST16_MAX   0xffff
#define UINT_FAST32_MAX   0xffffffff
#define UINT_FAST64_MAX   0xffffffffffffffff

#define INTPTR_MIN        (-0x7fffffff - 1)
#define INTPTR_MAX        0x7fffffff
#define UINTPTR_MAX       0xffffffff

#define PTRDIFF_MIN       INT32_MIN
#define PTRDIFF_MAX       INT32_MAX

#define SIZE_MAX          UINT32_MAX

/* "_C" macros*/
#define INT8_C(value)     (value)
#define INT16_C(value)    (value)
#define INT32_C(value)    (value)
#define INT64_C(value)    (value##LL)

#define UINT8_C(value)    (value##U)
#define UINT16_C(value)   (value##U)
#define UINT32_C(value)   (value##U)
#define UINT64_C(value)   (value##ULL)

/*
  TODO : Add missing stuff
*/

#endif
