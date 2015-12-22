#include "framework.h"
#include "bgfx.h"

extern "C" int _main_(int _argc, char** _argv);

namespace monster
{
	const uint8_t FrameWork::k_max_window_count = 8;

	static uint32_t s_debug = 0;
	static uint32_t s_reset = 0;
	static bool s_exit = false;

	bool FrameWork::processEvents(uint32_t& width, uint32_t& height, uint32_t& debug, uint32_t& reset, MouseState* mouse)
	{
		WindowHandle handle = { UINT16_MAX };

		bool mouseLock = inputIsMouseLocked();

		const Event* ev;
		do
		{
			struct SE { const Event* _ev; SE() : _ev(poll()) {} ~SE() { if (nullptr != _ev) { release(_ev); } } } scopeEvent;
			ev = scopeEvent._ev;

			if ( nullptr != ev)
			{
				switch (ev->_type)
				{
					case Event::Type::Char:
					{
						const CharEvent* chev = static_cast<const CharEvent*>(ev);
						inputChar(chev->_len, chev->_char);
					}
					break;

					case Event::Type::Exit:	return true;

					case Event::Type::Mouse:
					{
						const MouseEvent* mouse_ev = static_cast<const MouseEvent*>(ev);
						handle = mouse_ev->_handle;

						if (mouse_ev->_move)
						{
							inputSetMousePos(mouse_ev->_mx, mouse_ev->_my, mouse_ev->_mz);
						}
						else
						{
							inputSetMouseButtonState(mouse_ev->_button, mouse_ev->_down);
						}

						if (nullptr != mouse
							&& !mouseLock)
						{
							if (mouse_ev->_move)
							{
								mouse->_mx = mouse_ev->_mx;
								mouse->_my = mouse_ev->_my;
								mouse->_mz = mouse_ev->_mz;
							}
							else
							{
								mouse->_buttons[(uint32_t)mouse_ev->_button] = mouse_ev->_down;
							}
						}
					}
					break;

					case Event::Type::Key:
					{
						const KeyEvent* key = static_cast<const KeyEvent*>(ev);
						handle = key->_handle;

						inputSetKeyState(key->_key, key->_modifiers, key->_down);
					}
					break;

					case Event::Type::Size:
					{
						const SizeEvent* size_ev = static_cast<const SizeEvent*>(ev);
						handle = size_ev->_handle;
						width = size_ev->_width;
						height = size_ev->_height;
						reset = !s_reset; // force reset
					}
					break;

					case Event::Type::Window:
					break;

					default:
					break;
				}
			}

			inputProcess();

		} while (nullptr != ev);

		if (handle.idx == 0
			&& reset != s_reset)
		{
			reset = s_reset;
			bgfx::reset(width, height, reset);
			inputSetMouseResolution(width, height);
		}

		return s_exit;
	}

	FrameWork::WindowState s_window[FrameWork::k_max_window_count];

	bool FrameWork::processWindowEvents(WindowState& state, uint32_t& debug, uint32_t& reset)
	{
		s_debug = debug;
		s_reset = reset;

		WindowHandle handle = { UINT16_MAX };

		bool is_mouse_lock = inputIsMouseLocked();

		const Event* ev;
		do
		{
			struct SE { const Event* _ev; SE() : _ev(poll()) {} ~SE() { if (nullptr != _ev) { release(_ev); } } } scopeEvent;
			ev = scopeEvent._ev;

			if (nullptr != ev)
			{
				handle = ev->_handle;
				WindowState& win = s_window[handle.idx];

				switch (ev->_type)
				{
					case Event::Type::Char:
					{
						const CharEvent* chev = static_cast<const CharEvent*>(ev);
						win._handle = chev->_handle;
						inputChar(chev->_len, chev->_char);
					}
					break;
	
					case Event::Type::Mouse:
					{
						const MouseEvent* mouse = static_cast<const MouseEvent*>(ev);
						win._handle = mouse->_handle;

						if (mouse->_move)
						{
							inputSetMousePos(mouse->_mx, mouse->_my, mouse->_mz);
						}
						else
						{
							inputSetMouseButtonState(mouse->_button, mouse->_down);
						}

						if (!is_mouse_lock)
						{
							if (mouse->_move)
							{
								win._mouse._mx = mouse->_mx;
								win._mouse._my = mouse->_my;
								win._mouse._mz = mouse->_mz;
							}
							else
							{
								win._mouse._buttons[(uint32_t)mouse->_button] = mouse->_down;
							}
						}
					}
					break;

					case Event::Type::Key:
					{
						const KeyEvent* key = static_cast<const KeyEvent*>(ev);
						win._handle = key->_handle;

						inputSetKeyState(key->_key, key->_modifiers, key->_down);
					}
					break;

					case Event::Type::Size:
					{
						const SizeEvent* size = static_cast<const SizeEvent*>(ev);
						win._handle = size->_handle;
						win._width = size->_width;
						win._height = size->_height;
						reset = win._handle.idx == 0 ? !s_reset : reset;
					}
					break;

					case Event::Type::Window:
					{
						const WindowEvent* window = static_cast<const WindowEvent*>(ev);
						win._handle = window->_handle;
						win._nwh = window->_nwh;
						ev = nullptr;
					}
				break;

				case Event::Type::Exit:return true;

				default:
					break;
				}
			}

			inputProcess();

		} while (nullptr != ev);

		if (isValid(handle))
		{
			const WindowState& win = s_window[handle.idx];
			state = win;

			if (handle.idx == 0)
			{
				inputSetMouseResolution(win._width, win._height);
			}
		}

		if (reset != s_reset)
		{
			reset = s_reset;
			bgfx::reset(s_window[0]._width, s_window[0]._height, reset);
			inputSetMouseResolution(s_window[0]._width, s_window[0]._height);
		}

		debug = s_debug;

		return s_exit;
	}

	int FrameWork::entry(int _argc, char** _argv)
	{
		//DBG(BX_COMPILER_NAME " / " BX_CPU_NAME " / " BX_ARCH_NAME " / " BX_PLATFORM_NAME);

		inputInit();

		FrameWork::WindowHandle defaultWindow = { 0 };
		FrameWork::setWindowTitle(defaultWindow, "test");

		int result = ::_main_(_argc, _argv);

		inputRemoveBindings("bindings");
		inputShutdown();

		return result;
	}

}