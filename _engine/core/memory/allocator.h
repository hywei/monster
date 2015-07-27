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
		void make_delete(T* p) { if (p) { p->~T(); dellocate(p); })

		virtual size_t allocatedSize(void* p) = 0;
	};
}

#endif