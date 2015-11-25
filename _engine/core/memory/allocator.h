#ifndef __MONSTER_ALLOCATOR_H__
#define __MONSTER_ALLOCATOR_H__

#include <cstdint>
#include <memory>

namespace monster
{
	class Allocator
	{
	protected:
		virtual void* allocate(size_t size, size_t align) = 0;
		virtual void deallocate(void* p) = 0;

	public:
		template <class T, class V>
		T* make_new(const V& v) { return new(allocate(sizeof(T), alignof(T))) T(v); }

		template <class T>
		void make_delete(T* p) { if (p) { p->~T(); dellocate(p); } }

		virtual size_t allocatedSize(void* p) = 0;
	};

	class AllocatorI
	{
	public:
		virtual ~AllocatorI() = 0;
		virtual void* alloc(size_t _size, size_t _align, const char* _file, uint32_t _line) = 0;
		virtual void free(void* _ptr, size_t _align, const char* _file, uint32_t _line) = 0;
	};

	inline AllocatorI::~AllocatorI()
	{
	}

	inline void* alloc(AllocatorI* allocator, size_t size, size_t align = 0, const char* file = NULL, uint32_t line = 0)
	{
		return allocator->alloc(size, align, file, line);
	}

	inline void free(AllocatorI* allocator, void* ptr, size_t align = 0, const char* file = NULL, uint32_t line = 0)
	{
		allocator->free(ptr, align, file, line);
	}
}

#endif