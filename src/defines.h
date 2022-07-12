#pragma once

// Unsigned int types.
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

// Signed int types.
typedef signed char      i8;
typedef signed short     i16;
typedef signed int       i32;
typedef signed long long i64;

// Floating point types
typedef float  f32;
typedef double f64;

#include <float.h>
#define F32Max FLT_MAX
#define F32Min FLT_MIN

#define printfl(x) printf("%s\n", x)
#define DebugLog(...) fprintf(stderr, __VA_ARGS__)
