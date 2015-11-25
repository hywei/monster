#ifndef __MONSTER_HARDWARE_H__
#define __MONSTER_HARDWARE_H__

#include <cstdint>

#define MONSTER_COMPILER_MSVC 1

#if MONSTER_COMPILER_MSVC
#include <intrin.h>
#include <windows.h>

#pragma intrinsic(_ReadBarrier)
#pragma intrinsic(_WriteBarrier)
#pragma intrinsic(_ReadWriteBarrier)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedCompareExchange)
#endif

namespace monster
{
	inline void readBarrier()
	{
#if MONSTER_COMPILER_MSVC
		_ReadBarrier();
#else
		asm volatile("":::"memory");
#endif
	}

	inline void writeBarrier()
	{
#if MONSTER_COMPILER_MSVC
		_WriteBarrier();
#else
		asm volatile("":::"memory");
#endif
	}

	inline void readWriteBarrier()
	{
#if MONSTER_COMPILER_MSVC
		_ReadWriteBarrier();
#else
		asm volatile("":::"memory");
#endif
	}

	inline void memoryBarrier()
	{
#if MONSTER_COMPILER_MSVC
		_mm_mfence();
#else
		__sync_synchronize();
#endif
	}

	inline int32_t atomicInc(volatile void* ptr)
	{
#if MONSTER_COMPILER_MSVC
		return _InterlockedIncrement((volatile LONG*)ptr);
#else
		return __sync_add_and_fetch((volatile int32_t*)ptr, 1);
#endif
	}

	inline int32_t atomicDec(volatile void* ptr)
	{
#if MONSTER_COMPILER_MSVC
		return _InterlockedDecrement((volatile LONG*)ptr);
#else
		return __sync_sub_and_fetch((volatile int32_t*)ptr, 1);
#endif
	}

	inline int32_t atomicCompareAndSwap(volatile void* ptr, int32_t old_value, int32_t new_value)
	{
#if MONSTER_COMPILER_MSVC
		return _InterlockedCompareExchange((volatile LONG*)ptr, new_value, old_value);
#else
		return __sync_val_compare_and_swap((volatile int32_t*)ptr, old_value, new_value);
#endif // BX_COMPILER
	}

	inline void* atomicExchangePtr(void** ptr, void* new_valude)
	{
#if MONSTER_COMPILER_MSVC
		return InterlockedExchangePointer(ptr, new_valude); /* VS2012 no intrinsics */
#else
		return __sync_lock_test_and_set(ptr, new_valude);
#endif // BX_COMPILER
	}


}

#endif