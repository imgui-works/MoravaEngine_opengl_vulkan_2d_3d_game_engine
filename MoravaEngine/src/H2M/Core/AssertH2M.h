#pragma once

#include "H2M/Core/LogH2M.h"

#include <signal.h>


#ifdef HZ_PLATFORM_WINDOWS
#define HZ_DEBUGBREAK __debugbreak()
#elif HZ_PLATFORM_LINUX
#define HZ_DEBUGBREAK raise(SIGTRAP)
#endif

#ifdef HZ_DEBUG
	#define HZ_ENABLE_ASSERTS
#endif

#ifdef HZ_ENABLE_ASSERTS
	#define HZ_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { HZ_ERROR("Assertion Failed"); HZ_DEBUGBREAK; } } // __debugbreak() not supported by GCC
	#define HZ_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { HZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); HZ_DEBUGBREAK; } } // __debugbreak() not supported by GCC

	#define HZ_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
	#define HZ_GET_ASSERT_MACRO(...) HZ_EXPAND_VARGS(HZ_ASSERT_RESOLVE(__VA_ARGS__, HZ_ASSERT_MESSAGE, HZ_ASSERT_NO_MESSAGE))

	#define HZ_ASSERT(...) HZ_EXPAND_VARGS( HZ_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
	#define HZ_CORE_ASSERT(...) HZ_EXPAND_VARGS( HZ_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
	#define HZ_ASSERT(...)
	#define HZ_CORE_ASSERT(...)
#endif

#ifdef HZ_ENABLE_VERIFY
#define HZ_VERIFY_NO_MESSAGE(condition) { if(!(condition)) { HZ_ERROR("Verify Failed"); __debugbreak(); } }
#define HZ_VERIFY_MESSAGE(condition, ...) { if(!(condition)) { HZ_ERROR("Verify Failed: {0}", __VA_ARGS__); __debugbreak(); } }

#define HZ_VERIFY_RESOLVE(arg1, arg2, macro, ...) macro
#define HZ_GET_VERIFY_MACRO(...) HZ_EXPAND_VARGS(HZ_VERIFY_RESOLVE(__VA_ARGS__, HZ_VERIFY_MESSAGE, HZ_VERIFY_NO_MESSAGE))

#define HZ_VERIFY(...) HZ_EXPAND_VARGS( HZ_GET_VERIFY_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#define HZ_CORE_VERIFY(...) HZ_EXPAND_VARGS( HZ_GET_VERIFY_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#define HZ_VERIFY(...)
#define HZ_CORE_VERIFY(...)
#endif