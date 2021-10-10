#pragma once

#include <loguru.hpp>

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

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#ifdef _MSC_VER
#	define START_EXTERNAL_INCLUDE __pragma(warning(push)) __pragma(warning(disable : 4201))
#	define END_EXTERNAL_INCLUDE __pragma(warning(pop))
#else
#	define START_EXTERNAL_INCLUDE
#	define END_EXTERNAL_INCLUDE
#endif

#define UNUSED(x) (void)(x)

#define NONCOPYABLE(T)                                                                                                 \
	T(const T&)  = delete;                                                                                             \
	void operator=(const T&) = delete
#define NONMOVEABLE(T)                                                                                                 \
	T(T&&) noexcept = delete;                                                                                          \
	void operator=(T&&) noexcept = delete

#define DEFAULT_COPYABLE(T)                                                                                            \
	T(const T&) = default;                                                                                             \
	T& operator=(const T&) = default

#define DEFAULT_MOVEABLE(T)                                                                                            \
	T(T&&) noexcept = default;                                                                                         \
	T& operator=(T&&) noexcept = default

#define DEFAULT_CTORS(T)                                                                                               \
	DEFAULT_COPYABLE(T);                                                                                               \
	DEFAULT_MOVEABLE(T)

#ifdef _MSC_VER
#	define DEBUGBREAK __debugbreak()
#else
// TODO: Handle other cases
#	define DEBUGBREAK                                                                                                 \
		int* trap = reinterpret_cast<int*>(3L);                                                                        \
		*trap     = 3
#endif

#define NOOP(...)

#ifdef _DEBUG
#	define Assert(x, ...)                                                                                             \
		do                                                                                                             \
		{                                                                                                              \
			if (!(x))                                                                                                  \
			{                                                                                                          \
				LOG_F(ERROR, "Assertion `%s` failed (%s:%d): %s", #x, __FILE__, __LINE__, __VA_ARGS__);                \
				DEBUGBREAK;                                                                                            \
			}                                                                                                          \
		} while (false)

#	define AssertUnreachable() Assert(false, "Unreachable path")
#else
#	define Assert(x) NOOP(x)
#	define AssertUnreachable() NOOP()
#endif

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