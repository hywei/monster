#ifndef __MONSTER_HANDLE_ALLOCATOR_H__
#define __MONSTER_HANDLE_ALLOCATOR_H__

#include <cstdint>
#include "core/memory/allocator.h"

namespace monster
{
	template <uint16_t MaxHandlesCountT>
	class HandleAllocT
	{
	private:
		uint16_t _handles[MaxHandlesCountT * 2];
		uint16_t _handle_count;

	public:
		static const uint16_t invalid = 0xffff;

		HandleAllocT()
			: _handle_count(0)
		{
			for (uint16_t ii = 0; ii < MaxHandlesCountT; ++ii)
			{
				_handles[ii] = ii;
			}
		}

		~HandleAllocT()
		{
		}

		const uint16_t* getHandles() const
		{
			return _handles;
		}

		uint16_t getHandleAt(uint16_t at) const
		{
			return _handles[at];
		}

		uint16_t getNumHandles() const
		{
			return _handle_count;
		}

		uint16_t getMaxHandlesCount() const
		{
			return MaxHandlesCountT;
		}

		uint16_t alloc()
		{
			if (_handle_count < MaxHandlesCountT)
			{
				uint16_t index = _handle_count;
				++_handle_count;

				uint16_t handle = _handles[index];
				uint16_t* sparse = &_handles[MaxHandlesCountT];
				sparse[handle] = index;
				return handle;
			}

			return invalid;
		}

		bool isValid(uint16_t handle)
		{
			uint16_t* sparse = &_handles[MaxHandlesCountT];
			uint16_t index = sparse[handle];

			return index < _handle_count && _handles[index] == handle;
		}

		void free(uint16_t handle)
		{
			uint16_t* sparse = &_handles[MaxHandlesCountT];
			uint16_t index = sparse[handle];
			--_handle_count;
			uint16_t temp = _handles[_handle_count];
			_handles[_handle_count] = handle;
			sparse[temp] = index;
			_handles[index] = temp;
		}
	};

	class HandleAlloc
	{
	private:
		uint16_t* _handles;
		uint16_t _handle_count;
		uint16_t _max_handles_count;

	public:
		static const uint16_t invalid = 0xffff;

		HandleAlloc(uint16_t max_handle_count, void* handles)
			: _handles((uint16_t*)handles)
			, _handle_count(0)
			, _max_handles_count(max_handle_count)
		{
			for (uint16_t ii = 0; ii < max_handle_count; ++ii)
			{
				_handles[ii] = ii;
			}
		}

		~HandleAlloc()
		{
		}

		const uint16_t* getHandles() const
		{
			return _handles;
		}

		uint16_t getHandleAt(uint16_t at) const
		{
			return _handles[at];
		}

		uint16_t getHandleCount() const
		{
			return _handle_count;
		}

		uint16_t getMaxHandlesCount() const
		{
			return _max_handles_count;
		}

		uint16_t alloc()
		{
			if (_handle_count < _max_handles_count)
			{
				uint16_t index = _handle_count;
				++_handle_count;

				uint16_t handle = _handles[index];
				uint16_t* sparse = &_handles[_max_handles_count];
				sparse[handle] = index;
				return handle;
			}

			return invalid;
		}

		bool isValid(uint16_t handle)
		{
			uint16_t* sparse = &_handles[_max_handles_count];
			uint16_t index = sparse[handle];

			return (index < _handle_count && _handles[index] == handle);
		}

		void free(uint16_t handle)
		{
			uint16_t* sparse = &_handles[_max_handles_count];
			uint16_t index = sparse[handle];
			--_handle_count;
			uint16_t temp = _handles[_handle_count];
			_handles[_handle_count] = handle;
			sparse[temp] = index;
			_handles[index] = temp;
		}
	};

	HandleAlloc* createHandleAlloc(AllocatorI* allocator, uint16_t max_handles_count)
	{
		uint8_t* ptr = (uint8_t*)monster::alloc(allocator, sizeof(HandleAlloc) + 2 * max_handles_count*sizeof(uint16_t));
		return ::new (ptr) HandleAlloc(max_handles_count, &ptr[sizeof(HandleAlloc)]);
	}

	inline void destroyHandleAlloc(AllocatorI* allocator, HandleAlloc* handleAlloc)
	{
		handleAlloc->~HandleAlloc();
		monster::free(allocator, handleAlloc);
	}

} // namespace bx

#endif // BX_HANDLE_ALLOC_H_HEADER_GUARD
