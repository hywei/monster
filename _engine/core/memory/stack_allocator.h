#ifndef __MONSTER_LINEAR_ALLOCATOR_H__
#define __MONSTER_LINEAR_ALLOCATOR_H__

#include "core/memory/allocator.h"

namespace monster
{
	class LinearAllocator :
		public Allocator
	{
	private:
		uintptr_t _buffer;
		size_t _buffer_size;

		size_t _allocate_offset;
		size_t _allocate_size;

	protected:
		virtual void* allocate(size_t size, size_t align) override;
		virtual void deallocate(void* p) override;

	public:
		LinearAllocator();
		~LinearAllocator(); 

		void initialize(void* buffer, size_t size);
		void release();



	};

	LinearAllocator::LinearAllocator() : _buffer(0), _buffer_size(0), _allocate_offset(0), _allocate_size(0)
	{}

	LinearAllocator::LinearAllocator() { release(); }

	void LinearAllocator::initialize(void* buffer, size_t size)
	{
		if (buffer == nullptr ||
			size == 0)
		{
			return;
		}

		_buffer = reinterpret_cast<uintptr_t> (buffer);
		_buffer_size = size;

		_allocate_offset = 0;
		_allocate_size = 0;
		
	}

	void LinearAllocator::release()
	{
		_buffer = 0;
		_buffer_size = 0;
	}
	
	void* LinearAllocator::allocate(size_t size, size_t alignment)
	{
		if (size == 0)
		{
			return;
		}

		size_t allocated_size = _allocate_size;
		uintptr_t buffer_head = _buffer + allocated_size;
		uintptr_t alloc_start = reinterpret_cast<uintptr_t>(std::align(alignment, size, (void*&)buffer_head, allocated_size));

		if (alloc_start == 0 || allocated_size == 0)
		{
			return;
		}

	}

}

#endif