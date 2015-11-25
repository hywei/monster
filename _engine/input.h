#ifndef __MONSTER_INPUT_H__
#define __MONSTER_INPUT_H__

#include <cstdint>
#include <cstring>
#include <cassert>
#include <utility>
#include <unordered_map>

#include "core/utility/ringbuffer.h"

namespace monster
{
	enum class MouseButton
	{
		None,
		Left,
		Middle,
		Right,

		k_count
	};

	enum class Modifier
	{
		None		= 0,
		LeftAlt		= 0x01,
		RightAlt	= 0x02,
		LeftCtrl	= 0x04,
		RightCtrl	= 0x08,
		LeftShift	= 0x10,
		RightShift	= 0x20,
		LeftMeta	= 0x40,
		RightMeta	= 0x80,

		k_count
	};

	enum class Key
	{
		None = 0,
		Esc,
		Return,
		Tab,
		Space,
		Backspace,
		Up,
		Down,
		Left,
		Right,
		PageUp,
		PageDown,
		Home,
		End,
		Print,
		Plus,
		Minus,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		NumPad0,
		NumPad1,
		NumPad2,
		NumPad3,
		NumPad4,
		NumPad5,
		NumPad6,
		NumPad7,
		NumPad8,
		NumPad9,
		Key0,
		Key1,
		Key2,
		Key3,
		Key4,
		Key5,
		Key6,
		Key7,
		Key8,
		Key9,
		KeyA,
		KeyB,
		KeyC,
		KeyD,
		KeyE,
		KeyF,
		KeyG,
		KeyH,
		KeyI,
		KeyJ,
		KeyK,
		KeyL,
		KeyM,
		KeyN,
		KeyO,
		KeyP,
		KeyQ,
		KeyR,
		KeyS,
		KeyT,
		KeyU,
		KeyV,
		KeyW,
		KeyX,
		KeyY,
		KeyZ,

		k_count
	};

	class Mouse
	{
	public:
		int32_t _absolute[3];
		float _norm[3];
		int32_t _wheel;
		uint8_t _buttons[(uint32_t)MouseButton::k_count];
		uint16_t _width;
		uint16_t _height;
		uint16_t _wheel_delta;
		bool _is_lock;

	public:
		Mouse(): _width(1280), _height(720), _wheel_delta(120), _is_lock(false) {}

		void reset()
		{
			if (_is_lock) { _norm[0] = _norm[1] = _norm[2] = 0.f; }
			std::memset(_buttons, 0, sizeof(_buttons));
		}

		void setResolution(uint16_t _width, uint16_t _height)
		{
			_width = _width;
			_height = _height;
		}

		void setPos(int32_t mx, int32_t my, int32_t mz)
		{
			_absolute[0] = mx;
			_absolute[1] = my;
			_absolute[2] = mz;
			assert(_width != 0.f);
			assert(_height != 0.f);
			assert(_wheel_delta != 0.f);

			_norm[0] = float(mx) / float(_width);
			_norm[1] = float(my) / float(_height);
			_norm[2] = float(mz) / float(_wheel_delta);
		}

		void setButtonState(MouseButton _button, uint8_t _state)
		{
			_buttons[(uint8_t)_button] = _state;
		}


	};

	class MouseState
	{
	public:
		int32_t _mx;
		int32_t _my;
		int32_t _mz;
		uint8_t _buttons[(uint32_t)MouseButton::k_count];

	public:
		MouseState() : _mx(0), _my(0), _mz(0)
		{
			for (uint32_t i = 0; i < (uint32_t)MouseButton::k_count; i++)
			{
				_buttons[i] = (uint8_t)MouseButton::None;
			}
		}
	};

	class Keyboard
	{
	public:
		uint32_t _key[256];
		bool _once[256];

		RingBufferControl _ring;
		uint8_t _char[256];

	public:
		Keyboard() : _ring(256) {}

		void reset()
		{
			memset(_key, 0, sizeof(_key));
			memset(_once, 0xff, sizeof(_once));
		}

		static uint32_t encodeKeyState(uint8_t modifiers, bool down)
		{
			uint32_t state = 0;
			state |= uint32_t(modifiers) << 16;
			state |= uint32_t(down) << 8;
			return state;
		}

		static void decodeKeyState(uint32_t state, uint8_t& modifiers, bool& down)
		{
			modifiers = (state >> 16) & 0xff;
			down = 0 != ((state >> 8) & 0xff);
		}

		void setKeyState(Key key, uint8_t modifiers, bool down)
		{
			_key[(uint32_t)key] = encodeKeyState(modifiers, down);
			_once[(uint32_t)key] = false;
		}

		void pushChar(uint8_t len, const uint8_t ch[4])
		{
			for (uint32_t i = _ring.reserve(4); i < len; i = _ring.reserve(4))
			{
				popChar();
			}

			memcpy(&_char[_ring.m_current], ch, 4);
			_ring.commit(4);
		}

		const uint8_t* popChar()
		{
			if (0 < _ring.available())
			{
				uint8_t* utf8 = &_char[_ring.m_read];
				_ring.consume(4);
				return utf8;
			}

			return NULL;
		}

		void charFlush()
		{
			_ring.m_current = 0;
			_ring.m_write = 0;
			_ring.m_read = 0;
		}
	};

	typedef void(*InputBindingFn)(const void* _user_data);

	struct InputBinding
	{
		Key _key;
		uint8_t _modifiers;
		uint8_t _flags;
		InputBindingFn _fn;
		const void* _user_data;
	};


	class Input
	{
	public:
		typedef std::unordered_map<const char*, const InputBinding*> InputBindingMap;
		InputBindingMap _input_bindings_map;
		Mouse _mouse;
		Keyboard _keyboard;

	public:
		Input() { reset(); }

		~Input() {}

		void addBindings(const char* name, const InputBinding* bindings)
		{
			_input_bindings_map.insert(std::make_pair(name, bindings));
		}

		void removeBindings(const char* name)
		{
			InputBindingMap::iterator it = _input_bindings_map.find(name);
			if (it != _input_bindings_map.end())
			{
				_input_bindings_map.erase(it);
			}
		}

		void process(const InputBinding* bindings)
		{
			for (const InputBinding* binding = bindings; binding->_key != Key::None; ++binding)
			{
				uint8_t modifiers;
				bool down;
				Keyboard::decodeKeyState(_keyboard._key[(uint32_t)binding->_key], modifiers, down);

				if (binding->_flags == 1)
				{
					if (down)
					{
						if (modifiers == binding->_modifiers
							&&  !_keyboard._once[(uint32_t)binding->_key])
						{
							binding->_fn(binding->_user_data);
							_keyboard._once[(uint32_t)binding->_key] = true;
						}
					}
					else
					{
						_keyboard._once[(uint32_t)binding->_key] = false;
					}
				}
				else
				{
					if (down
						&&  modifiers == binding->_modifiers)
					{
						binding->_fn(binding->_user_data);
					}
				}
			}
		}

		void process()
		{
			for (InputBindingMap::const_iterator it = _input_bindings_map.begin(); it != _input_bindings_map.end(); ++it)
			{
				process(it->second);
			}
		}

		void reset()
		{
			_mouse.reset();
			_keyboard.reset();
		}
	};

	#define INPUT_BINDING_END { Key::None, Modifier::None, 0, NULL, NULL }

	///
	void inputInit();

	///
	void inputShutdown();

	///
	void inputAddBindings(const char* _name, const InputBinding* _bindings);

	///
	void inputRemoveBindings(const char* _name);

	///
	void inputProcess();

	///
	void inputSetKeyState(Key _key, uint8_t _modifiers, bool _down);

	/// Adds single UTF-8 encoded character into input buffer.
	void inputChar(uint8_t _len, const uint8_t _char[4]);

	/// Returns single UTF-8 encoded character from input buffer.
	const uint8_t* inputGetChar();

	/// Flush internal input buffer.
	void inputCharFlush();

	///
	void inputSetMouseResolution(uint16_t _width, uint16_t _height);

	///
	void inputSetMousePos(int32_t _mx, int32_t _my, int32_t _mz);

	///
	void inputSetMouseButtonState(MouseButton _button, uint8_t _state);

	///
	void inputSetMouseLock(bool _lock);

	///
	void inputGetMouse(float _mouse[3]);

	///
	bool inputIsMouseLocked();
}

#endif