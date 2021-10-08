#pragma once

#include <stdint.h>

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using intptr  = intptr_t;
using uintptr = uintptr_t;

using isize = intptr_t;
using usize = uintptr_t;

using f32 = float;
using f64 = double;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

template <typename T>
inline constexpr T Min(T a, T b)
{
	return a < b ? a : b;
}

template <typename T>
inline constexpr T Max(T a, T b)
{
	return a > b ? a : b;
}

template <typename T>
inline constexpr T Clamp(T x, T a, T b)
{
	return Min(b, Max(x, a));
}

template <typename T>
inline constexpr T Saturate(T x)
{
	return Clamp(x, T(0), T(1));
}

template <typename T>
inline constexpr T Abs(T x)
{
	return x < 0 ? -x : x;
}