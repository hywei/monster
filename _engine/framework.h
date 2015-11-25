#ifndef __MONSTER_FRAMEWORK_H__
#define __MONSTER_FRAMEWORK_H__

#include <stdint.h>
#include "input.h"
#include "core/utility/spscqueue.h"

#define ENTRY_WINDOW_FLAG_NONE         UINT32_C(0x00000000)
#define ENTRY_WINDOW_FLAG_ASPECT_RATIO UINT32_C(0x00000001)
#define ENTRY_WINDOW_FLAG_FRAME        UINT32_C(0x00000002)

#define ENTRY_IMPLEMENT_EVENT(_class, _type) \
			_class(WindowHandle _handle) : Event(_type, _handle) {}

namespace monster
{
	class FrameWork
	{
	public:
		struct WindowHandle { uint16_t idx; };
		static bool isValid(WindowHandle handle) { return UINT16_MAX != handle.idx; }

		const static uint8_t k_max_window_count;

		class WindowState
		{
		public:
			WindowState() : _nwh (nullptr)
			{
				_handle.idx = UINT16_MAX;
			}

			WindowHandle _handle;
			uint32_t     _width;
			uint32_t     _height;
			MouseState   _mouse;
			void*        _nwh;
		};

		class Event
		{
		public:
			enum class Type
			{
				Char,
				Exit,
				Key,
				Mouse,
				Size,
				Window,
			};

			Type _type;
			WindowHandle _handle;

		public:
			Event(Type type)
				: _type(type)
			{
				_handle.idx = UINT16_MAX;
			}

			Event(Type type, WindowHandle handle) : _type(type), _handle(handle) {}
		};

		class CharEvent : public Event
		{
		public:
			ENTRY_IMPLEMENT_EVENT(CharEvent, Event::Type::Char);

			uint8_t _len;
			uint8_t _char[4];
		};

		class KeyEvent : public Event
		{
		public:
			ENTRY_IMPLEMENT_EVENT(KeyEvent, Event::Type::Key);

			Key _key;
			uint8_t _modifiers;
			bool _down;
		};

		class MouseEvent : public Event
		{
		public:
			ENTRY_IMPLEMENT_EVENT(MouseEvent, Event::Type::Mouse);

			int32_t _mx;
			int32_t _my;
			int32_t _mz;
			MouseButton _button;
			bool _down;
			bool _move;
		};

		class SizeEvent : public Event
		{
		public:
			ENTRY_IMPLEMENT_EVENT(SizeEvent, Event::Type::Size);

			uint32_t _width;
			uint32_t _height;
		};

		class WindowEvent : public Event
		{
		public:
			ENTRY_IMPLEMENT_EVENT(WindowEvent, Event::Type::Window);

			void* _nwh;
		};

		static const Event* poll();
		static const Event* poll(WindowHandle _handle);
		static void release(const Event* _event);

		class EventQueue
		{
		private:
			LockFreeSpScUnboundedQueue<Event> _queue;

		public:
			~EventQueue()
			{
				for (const Event* ev = poll(); nullptr != ev; ev = poll())
				{
					release(ev);
				}
			}

			void postCharEvent(WindowHandle handle, uint8_t len, const uint8_t ch[4])
			{
				CharEvent* ev = new CharEvent(handle);
				ev->_len = len;
				memcpy(ev->_char, ch, 4);
				_queue.push(ev);
			}

			void postExitEvent()
			{
				Event* ev = new Event(Event::Type::Exit);
				_queue.push(ev);
			}

			void postKeyEvent(WindowHandle handle, Key key, uint8_t modifiers, bool down)
			{
				KeyEvent* ev = new KeyEvent(handle);
				ev->_key = key;
				ev->_modifiers = modifiers;
				ev->_down = down;
				_queue.push(ev);
			}

			void postMouseEvent(WindowHandle handle, int32_t mx, int32_t my, int32_t mz)
			{
				MouseEvent* ev = new MouseEvent(handle);
				ev->_mx = mx;
				ev->_my = my;
				ev->_mz = mz;
				ev->_button = MouseButton::None;
				ev->_down = false;
				ev->_move = true;
				_queue.push(ev);
			}

			void postMouseEvent(WindowHandle handle, int32_t mx, int32_t my, int32_t mz, MouseButton button, bool down)
			{
				MouseEvent* ev = new MouseEvent(handle);
				ev->_mx = mx;
				ev->_my = my;
				ev->_mz = mz;
				ev->_button = button;
				ev->_down = down;
				ev->_move = false;
				_queue.push(ev);
			}

			void postSizeEvent(WindowHandle handle, uint32_t width, uint32_t height)
			{
				SizeEvent* ev = new SizeEvent(handle);
				ev->_width = width;
				ev->_height = height;
				_queue.push(ev);
			}

			void postWindowEvent(WindowHandle handle, void* nwh = nullptr)
			{
				WindowEvent* ev = new WindowEvent(handle);
				ev->_nwh = nwh;
				_queue.push(ev);
			}

			const Event* poll()
			{
				return _queue.pop();
			}

			const Event* poll(WindowHandle handle)
			{
				if (isValid(handle))
				{
					Event* ev = _queue.peek();
					if (NULL == ev
						|| ev->_handle.idx != handle.idx)
					{
						return NULL;
					}
				}

				return poll();
			}

			void release(const Event* _event) const
			{
				delete _event;
			}
		};


	public:
		static bool processEvents(uint32_t& width, uint32_t& height, uint32_t& debug, uint32_t& reset, MouseState* mouse = NULL);
		static WindowHandle createWindow(int32_t x, int32_t y, uint32_t width, uint32_t height, uint32_t flags = ENTRY_WINDOW_FLAG_NONE, const char* title = "");
		static void destroyWindow(WindowHandle handle);
		static void setWindowPos(WindowHandle handle, int32_t x, int32_t y);
		static void setWindowSize(WindowHandle handle, uint32_t width, uint32_t height);
		static void setWindowTitle(WindowHandle handle, const char* title);
		static void toggleWindowFrame(WindowHandle handle);
		static void toggleFullscreen(WindowHandle handle);
		static void setMouseLock(WindowHandle handle, bool is_lock);

		static bool processWindowEvents(WindowState& state, uint32_t& debug, uint32_t& reset);
	};
}

#endif