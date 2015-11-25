
#include "input.h"
#include "framework.h"

namespace monster
{
	static Input* s_input;
	void inputInit()
	{
		s_input = new (Input);
	}

	void inputShutdown()
	{
		delete s_input;
	}

	void inputAddBindings(const char* _name, const InputBinding* _bindings)
	{
		s_input->addBindings(_name, _bindings);
	}

	void inputRemoveBindings(const char* _name)
	{
		s_input->removeBindings(_name);
	}

	void inputProcess()
	{
		s_input->process();
	}

	void inputSetMouseResolution(uint16_t width, uint16_t height)
	{
		s_input->_mouse.setResolution(width, height);
	}

	void inputSetKeyState(Key key, uint8_t modifiers, bool down)
	{
		s_input->_keyboard.setKeyState(key, modifiers, down);
	}

	void inputSetMousePos(int32_t mx, int32_t my, int32_t mz)
	{
		s_input->_mouse.setPos(mx, my, mz);
	}

	void inputSetMouseButtonState(MouseButton button, uint8_t state)
	{
		s_input->_mouse.setButtonState(button, state);
	}

	void inputGetMouse(float mouse[3])
	{
		mouse[0] = s_input->_mouse._norm[0];
		mouse[1] = s_input->_mouse._norm[1];
		mouse[2] = s_input->_mouse._norm[2];
		s_input->_mouse._norm[0] = 0.0f;
		s_input->_mouse._norm[1] = 0.0f;
		s_input->_mouse._norm[2] = 0.0f;
	}

	bool inputIsMouseLocked()
	{
		return s_input->_mouse._is_lock;
	}

	void inputSetMouseLock(bool lock)
	{
		if (s_input->_mouse._is_lock != lock)
		{
			s_input->_mouse._is_lock = lock;
			FrameWork::WindowHandle default_window = { 0 };
			FrameWork::setMouseLock(default_window, lock);
			if (lock)
			{
				s_input->_mouse._norm[0] = 0.0f;
				s_input->_mouse._norm[1] = 0.0f;
				s_input->_mouse._norm[2] = 0.0f;
			}
		}
	}

	void inputChar(uint8_t len, const uint8_t ch[4])
	{
		s_input->_keyboard.pushChar(len, ch);
	}

	const uint8_t* inputGetChar()
	{
		return s_input->_keyboard.popChar();
	}

	void inputCharFlush()
	{
		s_input->_keyboard.charFlush();
	}
}
