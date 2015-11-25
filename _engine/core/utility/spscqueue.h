#ifndef __MONSTER_SPSC_QUEUE_H__
#define __MONSTER_SPSC_QUEUE_H__

#include <list>
#include "core/hardware.h"
#include "core/mutex.h"

namespace monster
{
	template <class T>
	class LockFreeSpScUnboundedQueue
	{
	private:
		class Node 
		{
		public:
			void* _ptr;
			Node* _next;

			Node(void* ptr) : _ptr(ptr), _next(nullptr) {}
		};

		Node* _first;
		Node* _last;
		Node* _divider;

	public:
		LockFreeSpScUnboundedQueue() : _first(new Node(nullptr)), _divider(_first), _last(_first) {}
		~LockFreeSpScUnboundedQueue()
		{
			while (_first != nullptr)
			{
				Node* node = _first;
				_first = _first->_next;
				delete node;
			}
		}

		LockFreeSpScUnboundedQueue(const LockFreeSpScUnboundedQueue&) = delete;
		LockFreeSpScUnboundedQueue& operator=(const LockFreeSpScUnboundedQueue&) = delete;

		void push(T* new_ptr)
		{
			_last->_next = new Node((void*)new_ptr);
			atomicExchangePtr((void**)&_last, _last->_next);
			while (_first != _divider)
			{
				Node* node = _first;
				_first = _first->_next;
				delete node;
			}
		}

		T* peek()
		{
			if (_divider != _last)
			{
				T* ptr = (T*)_divider->_next->_ptr;
				return ptr;
			}

			return nullptr;
		}

		T* pop()
		{
			if (_divider != _last)
			{
				T* ptr = (T*)_divider->_next->_ptr;
				atomicExchangePtr((void**)&_divider, _divider->_next);
				return ptr;
			}

			return nullptr;
		}
	};

	template <class T>
	class MutexSpScUnboundedQueue
	{
	private:
		Mutex _mutex;
		std::list<T*> _queue;

	public:
		MutexSpScUnboundedQueue() {}
		~MutexSpScUnboundedQueue() {}

		MutexSpScUnboundedQueue(const MutexSpScUnboundedQueue&) = delete;
		MutexSpScUnboundedQueue& operator = (const MutexSpScUnboundedQueue&) = delete;

		void push(T* new_item)
		{
			MutexScope lock(_mutex);
			_queue.push_back(new_item);
		}

		T* peek()
		{
			MutexScope lock(_mutex);
			if (!_queue.empty())
			{
				return _queue.front();
			}
			return nullptr;
		}

		T* pop()
		{
			MutexScope lock(_mutex);
			if(!_queue.empty())
			{
				T* p = _queue.front();
				_queue.pop_back();
				return p;
			}

			return nullptr;
		}
	};
}

#endif