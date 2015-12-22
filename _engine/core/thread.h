#ifndef __MONSTER_THREAD_H__
#define __MONSTER_THREAD_H__

#include <cstdint>
#include <Windows.h>

namespace monster
{
	class Semaphore
	{
	private:
		HANDLE _handle;

	public:
		Semaphore()
		{
			_handle = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
		}

		~Semaphore()
		{
			CloseHandle(_handle);
		}

		Semaphore(const Semaphore&) = delete;
		Semaphore& operator=(const Semaphore&) = delete;

		void post(uint32_t _count = 1) const
		{
			ReleaseSemaphore(_handle, _count, NULL);
		}

		bool wait(int32_t _msecs = -1) const
		{
			DWORD milliseconds = (0 > _msecs) ? INFINITE : _msecs;
			return WAIT_OBJECT_0 == WaitForSingleObject(_handle, milliseconds);
		}
	};


	typedef int32_t(*ThreadFunc) (void* user_data);
	
	class Thread
	{
	private:
		HANDLE _handle;
		ThreadFunc _thread_fn;
		void* _user_data;
		Semaphore _sem;
		uint32_t _stack_size;
		int32_t _exit_code;
		bool _is_running;

	private:
		int32_t entry()
		{
			_sem.post();
			return _thread_fn(_user_data);
		}

		static DWORD WINAPI threadFunc(LPVOID _arg)
		{
			Thread* thread = (Thread*)_arg;
			int32_t result = thread->entry();
			return result;
		}

	public:
		Thread() : 
			_handle(INVALID_HANDLE_VALUE), 
			_thread_fn(nullptr), 
			_user_data(nullptr),
			_stack_size(0),
			_exit_code(0),
			_is_running(false) {}

		~Thread()
		{
			if (_is_running)
			{
				shutdown();
			}
		}

		Thread(const Thread&) = delete;		
		Thread& operator = (const Thread&) = delete;

		void init(ThreadFunc fn, void* user_data = nullptr, uint32_t stack_size = 0)
		{
			_thread_fn = fn;
			_user_data = user_data;
			_stack_size = stack_size;
			_is_running = true;

			_handle = CreateThread(nullptr, _stack_size, threadFunc, this, 0, nullptr);

			_sem.wait();
		}

		void shutdown()
		{
			WaitForSingleObject(_handle, INFINITE);
			GetExitCodeThread(_handle, (DWORD*)&_exit_code);
			CloseHandle(_handle);
			_handle = INVALID_HANDLE_VALUE;
			_is_running = false;
		}

		bool isRunning() const { return _is_running; }

		int32_t getExitCode() const { return _exit_code; }
	};
}

#endif