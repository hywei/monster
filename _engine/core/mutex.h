/*
* Copyright 2010-2015 Branimir Karadzic. All rights reserved.
* License: http://www.opensource.org/licenses/BSD-2-Clause
*/

#ifndef BX_MUTEX_H_HEADER_GUARD
#define BX_MUTEX_H_HEADER_GUARD

#include "hardware.h"

#define MONSTER_PLATFORM_WINDOWS 1

namespace monster
{
#if MONSTER_PLATFORM_WINDOWS || MONSTER_PLATFORM_WINRT
	typedef CRITICAL_SECTION thread_mutex_t;
	typedef unsigned thread_mutexattr_t;

	inline int thread_mutex_lock(thread_mutex_t* mutex)
	{
		EnterCriticalSection(mutex);
		return 0;
	}

	inline int thread_mutex_unlock(thread_mutex_t* mutex)
	{
		LeaveCriticalSection(mutex);
		return 0;
	}

	inline int thread_mutex_trylock(thread_mutex_t* mutex)
	{
		return TryEnterCriticalSection(mutex) ? 0 : EBUSY;
	}

	inline int thread_mutex_init(thread_mutex_t* mutex, thread_mutexattr_t* /*attr*/)
	{
#if MONSTER_PLATFORM_WINRT
		InitializeCriticalSectionEx(mutex, 4000, 0);   // docs recommend 4000 spincount as sane default
#else
		InitializeCriticalSection(mutex);
#endif
		return 0;
	}

	inline int thread_mutex_destroy(thread_mutex_t* mutex)
	{
		DeleteCriticalSection(mutex);
		return 0;
	}
#else
	typedef pthread_mutex_t thread_mutex_t;
	typedef unsigned thread_mutexattr_t;

	inline int thread_mutex_lock(thread_mutex_t* mutex)
	{
		return pthread_mutex_lock(mutex);
	}

	inline int thread_mutex_unlock(thread_mutex_t* mutex)
	{
		return pthread_mutex_unlock(mutex);
	}

	inline int thread_mutex_trylock(thread_mutex_t* mutex)
	{
		return pthread_mutex_trylock(mutex);
	}

	inline int thread_mutex_init(thread_mutex_t* mutex, thread_mutexattr_t* /*attr*/)
	{
		return pthread_mutex_init(mutex);
	}

	inline int thread_mutex_destroy(thread_mutex_t* mutex)
	{
		return pthread_mutex_destroy(mutex);
	}

#endif 

	class Mutex
	{
	private:
		thread_mutex_t _handle;

	public:
		Mutex() { thread_mutex_init(&_handle, NULL); }
		~Mutex() { thread_mutex_destroy(&_handle); }

		Mutex(const Mutex&) = delete;
		Mutex& operator = (const Mutex&) = delete;

		void lock() { thread_mutex_lock(&_handle); }

		void unlock() { thread_mutex_unlock(&_handle); }
	};

	class MutexScope
	{
	private:
		Mutex& _mutex;

	public:
		MutexScope(Mutex& mutex) : _mutex(mutex) { _mutex.lock(); }
		~MutexScope() { _mutex.unlock(); }

		MutexScope(const MutexScope&) = delete;
		MutexScope& operator = (const MutexScope&) = delete;
	};

} 
#endif 