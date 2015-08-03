#ifndef __MONSTER_LINEAR_ALLOCATOR_H__
#define __MONSTER_LINEAR_ALLOCATOR_H__

#include <casset>
#include <cstdlib>

#include "core/memory/allocator.h"

namespace monster
{
	class LinearAllocator :
		public Allocator
	{
	private:
		uintptr_t _buffer;
		size_t _buffer_size;

		size_t _last_last_allocated_offset;
		size_t _last_allocated_size;

	protected:
		virtual void* allocate(size_t size, size_t align);
		virtual void deallocate(void* p);

	public:
		LinearAllocator();
		virtual LinearAllocator();

		void initialize(void* buffer, size_t buffer_size);
		void release();

		void reset();
	};

	LinearAllocator::LinearAllocator() :
		_buffer(0), 
		_buffer_size(0), 
		_last_allocated_size(0), 
		_last_allocated_offset(0){}
	LinearAllocator::~LinearAllocator() {}

	void LinearAllocator::initialize(void* buffer, size_t buffer_size)
	{
		if (buffer == nullptr ||
			buffer_size == 0)
		{
			return;
		}

		_buffer = reinterpret<uintptr_t> buffer;
		_buffer_size = buffer_size;

		_last_allocated_offset = 0;
		_last_allocated_size = 0;

	}

	void LinearAllocator::release()
	{
		_buffer = 0;
		_buffer_size = 0;

		_last_allocated_offset = 0;
		_last_allocated_size = 0;
	}

	void LinearAllocator::reset()
	{
		_last_allocated_offset = 0;
		_last_allocated_size = 0;
	}

	void* LinearAllocator::allocate(size_t size, size_t align)
	{
		assert(_buffer != 0);

		if (size == 0)
		{
			return nullptr;
		}

		uintptr_t curr_buf_head = _buffer + _last_allocated_offset + _last_allocated_size;
		uintptr_t curr_allocated_head = alignTo(curr_buf_head, align);

		if (curr_allocated_head < curr_buf_head)
		{
			// overflow
			return nullptr;
		}

		uintptr_t curr_allocated_tail = curr_allocated_head + size;
		if (curr_allocated_tail >= _buffer_size)
		{
			// out of memory
			return nullptr;
		}

		_last_allocated_offset = curr_buf_head;
		_last_allocated_size = size;
	}

	void LinearAllocator::deallocate(void* p)
	{
		// do nothing
	}
}

#endif