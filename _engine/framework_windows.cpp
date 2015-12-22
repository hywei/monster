#include "framework.h"
#include "bgfx.h"
#include "bgfxplatform.h"
#include "core/mutex.h"
#include "core/thread.h"
#include "core/memory/handle_allocator.h"

#include <string>
#include <algorithm>
#include <Windows.h>
#include <windowsx.h>

int main(int argc, char** argv);

namespace monster
{
	template<typename T, size_t N> char(&COUNTOF_REQUIRES_ARRAY_ARGUMENT(const T(&)[N]))[N];
#define MONSTER_COUNTOF(_x) sizeof(COUNTOF_REQUIRES_ARRAY_ARGUMENT(_x) )

	enum
	{
		WM_USER_WINDOW_CREATE = WM_USER,
		WM_USER_WINDOW_DESTROY,
		WM_USER_WINDOW_SET_TITLE,
		WM_USER_WINDOW_SET_POS,
		WM_USER_WINDOW_SET_SIZE,
		WM_USER_WINDOW_TOGGLE_FRAME,
		WM_USER_WINDOW_MOUSE_LOCK,
	};

	struct TranslateKeyModifiers
	{
		int _vk;
		Modifier _modifier;
	};

	static const uint8_t k_translate_key_modifier_count = 8;

	static const TranslateKeyModifiers s_translate_key_modifiers[k_translate_key_modifier_count] =
	{
		{ VK_LMENU,    Modifier::LeftAlt },
		{ VK_RMENU,    Modifier::RightAlt },
		{ VK_LCONTROL, Modifier::LeftCtrl },
		{ VK_RCONTROL, Modifier::RightCtrl },
		{ VK_LSHIFT,   Modifier::LeftShift },
		{ VK_RSHIFT,   Modifier::RightShift },
		{ VK_LWIN,     Modifier::LeftMeta },
		{ VK_RWIN,     Modifier::RightMeta },
	};

	static uint8_t translateKeyModifiers()
	{
		uint8_t modifiers = 0;
		for (uint32_t ii = 0; ii < k_translate_key_modifier_count; ++ii)
		{
			const TranslateKeyModifiers& tkm = s_translate_key_modifiers[ii];
			modifiers |= (0 > GetKeyState(tkm._vk)) ? (uint8_t)tkm._modifier : (uint8_t)Modifier::None;
		}
		return modifiers;
	}

	static Key s_translate_key[256];

	static Key translateKey(WPARAM wparam)
	{
		return s_translate_key[wparam & 0xff];
	}

	class MainThreadEntry
	{
	public:
		int _argc;
		char** _argv;

		static int32_t threadFunc(void* _user_data);
	};

	class Msg
	{
	public:
		Msg(): _x(0), _y(0), _width(0), _height(0), _flags(0){}

		int32_t  _x;
		int32_t  _y;
		uint32_t _width;
		uint32_t _height;
		uint32_t _flags;
		std::string _title;
	};

	static void mouseCapture(HWND hwnd, bool capture)
	{
		if (capture)
		{
			SetCapture(hwnd);
		}
		else
		{
			ReleaseCapture();
		}
	}

	const static uint32_t k_max_config_window_count = 8;
	const static uint32_t k_window_default_width = 1280;
	const static uint32_t k_window_default_height = 720;

	class Context
	{
	public:
		FrameWork::EventQueue _event_queue;
		Mutex _lock;

		HandleAllocT<k_max_config_window_count> _window_alloc;

		HWND _hwnd[k_max_config_window_count];
		uint32_t _flags[k_max_config_window_count];
		RECT _rect;
		DWORD _style;
		uint32_t _width;
		uint32_t _height;
		uint32_t _old_width;
		uint32_t _old_height;
		uint32_t _frame_width;
		uint32_t _frame_height;
		float _aspect_ratio;

		int32_t _mx;
		int32_t _my;
		int32_t _mz;

		bool _is_frame;
		HWND _mouse_lock;
		bool _is_init;
		bool _is_exit;


	public:
		Context(): _mz(0), _is_frame(true), _mouse_lock(NULL), _is_init(false), _is_exit(false)
		{
			memset(s_translate_key, 0, sizeof(s_translate_key));
			s_translate_key[VK_ESCAPE] = Key::Esc;
			s_translate_key[VK_RETURN] = Key::Return;
			s_translate_key[VK_TAB] = Key::Tab;
			s_translate_key[VK_BACK] = Key::Backspace;
			s_translate_key[VK_SPACE] = Key::Space;
			s_translate_key[VK_UP] = Key::Up;
			s_translate_key[VK_DOWN] = Key::Down;
			s_translate_key[VK_LEFT] = Key::Left;
			s_translate_key[VK_RIGHT] = Key::Right;
			s_translate_key[VK_PRIOR] = Key::PageUp;
			s_translate_key[VK_NEXT] = Key::PageUp;
			s_translate_key[VK_HOME] = Key::Home;
			s_translate_key[VK_END] = Key::End;
			s_translate_key[VK_SNAPSHOT] = Key::Print;
			s_translate_key[VK_OEM_PLUS] = Key::Plus;
			s_translate_key[VK_OEM_MINUS] = Key::Minus;
			s_translate_key[VK_F1] = Key::F1;
			s_translate_key[VK_F2] = Key::F2;
			s_translate_key[VK_F3] = Key::F3;
			s_translate_key[VK_F4] = Key::F4;
			s_translate_key[VK_F5] = Key::F5;
			s_translate_key[VK_F6] = Key::F6;
			s_translate_key[VK_F7] = Key::F7;
			s_translate_key[VK_F8] = Key::F8;
			s_translate_key[VK_F9] = Key::F9;
			s_translate_key[VK_F10] = Key::F10;
			s_translate_key[VK_F11] = Key::F11;
			s_translate_key[VK_F12] = Key::F12;
			s_translate_key[VK_NUMPAD0] = Key::NumPad0;
			s_translate_key[VK_NUMPAD1] = Key::NumPad1;
			s_translate_key[VK_NUMPAD2] = Key::NumPad2;
			s_translate_key[VK_NUMPAD3] = Key::NumPad3;
			s_translate_key[VK_NUMPAD4] = Key::NumPad4;
			s_translate_key[VK_NUMPAD5] = Key::NumPad5;
			s_translate_key[VK_NUMPAD6] = Key::NumPad6;
			s_translate_key[VK_NUMPAD7] = Key::NumPad7;
			s_translate_key[VK_NUMPAD8] = Key::NumPad8;
			s_translate_key[VK_NUMPAD9] = Key::NumPad9;
			s_translate_key[uint8_t('0')] = Key::Key0;
			s_translate_key[uint8_t('1')] = Key::Key1;
			s_translate_key[uint8_t('2')] = Key::Key2;
			s_translate_key[uint8_t('3')] = Key::Key3;
			s_translate_key[uint8_t('4')] = Key::Key4;
			s_translate_key[uint8_t('5')] = Key::Key5;
			s_translate_key[uint8_t('6')] = Key::Key6;
			s_translate_key[uint8_t('7')] = Key::Key7;
			s_translate_key[uint8_t('8')] = Key::Key8;
			s_translate_key[uint8_t('9')] = Key::Key9;
			s_translate_key[uint8_t('A')] = Key::KeyA;
			s_translate_key[uint8_t('B')] = Key::KeyB;
			s_translate_key[uint8_t('C')] = Key::KeyC;
			s_translate_key[uint8_t('D')] = Key::KeyD;
			s_translate_key[uint8_t('E')] = Key::KeyE;
			s_translate_key[uint8_t('F')] = Key::KeyF;
			s_translate_key[uint8_t('G')] = Key::KeyG;
			s_translate_key[uint8_t('H')] = Key::KeyH;
			s_translate_key[uint8_t('I')] = Key::KeyI;
			s_translate_key[uint8_t('J')] = Key::KeyJ;
			s_translate_key[uint8_t('K')] = Key::KeyK;
			s_translate_key[uint8_t('L')] = Key::KeyL;
			s_translate_key[uint8_t('M')] = Key::KeyM;
			s_translate_key[uint8_t('N')] = Key::KeyN;
			s_translate_key[uint8_t('O')] = Key::KeyO;
			s_translate_key[uint8_t('P')] = Key::KeyP;
			s_translate_key[uint8_t('Q')] = Key::KeyQ;
			s_translate_key[uint8_t('R')] = Key::KeyR;
			s_translate_key[uint8_t('S')] = Key::KeyS;
			s_translate_key[uint8_t('T')] = Key::KeyT;
			s_translate_key[uint8_t('U')] = Key::KeyU;
			s_translate_key[uint8_t('V')] = Key::KeyV;
			s_translate_key[uint8_t('W')] = Key::KeyW;
			s_translate_key[uint8_t('X')] = Key::KeyX;
			s_translate_key[uint8_t('Y')] = Key::KeyY;
			s_translate_key[uint8_t('Z')] = Key::KeyZ;
		}

		int32_t run(int argc, char** argv)
		{
			SetDllDirectory(".");

			HINSTANCE instance = (HINSTANCE)GetModuleHandle(NULL);

			WNDCLASSEX wnd;
			memset(&wnd, 0, sizeof(wnd));
			wnd.cbSize = sizeof(wnd);
			wnd.style = CS_HREDRAW | CS_VREDRAW;
			wnd.lpfnWndProc = wndProc;
			wnd.hInstance = instance;
			wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
			wnd.lpszClassName = "bgfx";
			wnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			RegisterClassExA(&wnd);

			_window_alloc.alloc();
			_hwnd[0] = CreateWindowA(
				"bgfx", 
				"BGFX", 
				WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
				0, 
				0, 
				k_window_default_width, 
				k_window_default_height, 
				NULL, 
				NULL, 
				instance, 
				0);

			_flags[0] = 0 | ENTRY_WINDOW_FLAG_ASPECT_RATIO | ENTRY_WINDOW_FLAG_FRAME;

			bgfx::winSetHwnd(_hwnd[0]);

			adjust(_hwnd[0], k_window_default_width, k_window_default_height, true);
			clear(_hwnd[0]);

			_width = k_window_default_width;
			_height = k_window_default_height;
			_old_width = k_window_default_width;
			_old_height = k_window_default_height;

			MainThreadEntry mte;
			mte._argc = argc;
			mte._argv = argv;

			Thread thread;
			thread.init(mte.threadFunc, &mte);
			_is_init = true;

			_event_queue.postSizeEvent(findHandle(_hwnd[0]), _width, _height);

			MSG msg;
			msg.message = WM_NULL;

			while (_is_exit == false)
			{
				WaitForInputIdle(GetCurrentProcess(), 16);

				while (0 != PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			thread.shutdown();

			DestroyWindow(_hwnd[0]);

			return 0;
		}

		LRESULT process(HWND hwnd, UINT id, WPARAM wparam, LPARAM lparam)
		{
			if (_is_init)
			{
				switch (id)
				{
				case WM_USER_WINDOW_CREATE:
				{
					Msg* msg = (Msg*)lparam;
					HWND hwnd = CreateWindowA("bgfx"
						, msg->_title.c_str()
						, WS_OVERLAPPEDWINDOW | WS_VISIBLE
						, msg->_x
						, msg->_y
						, msg->_width
						, msg->_height
						, _hwnd[0]
						, NULL
						, (HINSTANCE)GetModuleHandle(NULL)
						, 0);
					clear(hwnd);

					_hwnd[wparam] = hwnd;
					_flags[wparam] = msg->_flags;
					FrameWork::WindowHandle handle = { (uint16_t)wparam };
					_event_queue.postSizeEvent(handle, msg->_width, msg->_height);
					_event_queue.postWindowEvent(handle, hwnd);

					delete msg;
				}
				break;

				case WM_USER_WINDOW_DESTROY:
				{
					FrameWork::WindowHandle handle = { (uint16_t)wparam };
					PostMessageA(_hwnd[wparam], WM_CLOSE, 0, 0);
					_event_queue.postWindowEvent(handle);
					DestroyWindow(_hwnd[wparam]);
					_hwnd[wparam] = 0;
				}
				break;

				case WM_USER_WINDOW_SET_TITLE:
				{
					Msg* msg = (Msg*)lparam;
					SetWindowTextA(_hwnd[wparam], msg->_title.c_str());
					delete msg;
				}
				break;

				case WM_USER_WINDOW_SET_POS:
				{
					Msg* msg = (Msg*)lparam;
					SetWindowPos(_hwnd[wparam], 0, msg->_x, msg->_y, 0, 0
						, SWP_NOACTIVATE
						| SWP_NOOWNERZORDER
						| SWP_NOSIZE
						);
					delete msg;
				}
				break;

				case WM_USER_WINDOW_SET_SIZE:
				{
					uint32_t width = GET_X_LPARAM(lparam);
					uint32_t height = GET_Y_LPARAM(lparam);
					adjust(_hwnd[wparam], width, height, true);
				}
				break;

				case WM_USER_WINDOW_TOGGLE_FRAME:
				{
					if (_is_frame)
					{
						_old_width = _width;
						_old_height = _height;
					}
					adjust(_hwnd[wparam], _old_width, _old_height, !_is_frame);
				}
				break;

				case WM_USER_WINDOW_MOUSE_LOCK:
					setMouseLock(_hwnd[wparam], !!lparam);
					break;

				case WM_DESTROY:
					break;

				case WM_QUIT:
				case WM_CLOSE:
					if (hwnd == _hwnd[0])
					{
						_is_exit = true;
						_event_queue.postExitEvent();
					}
					else
					{
						FrameWork::destroyWindow(findHandle(hwnd));
					}
					// Don't process message. Window will be destroyed later.
					return 0;

				case WM_SIZING:
				{
					FrameWork::WindowHandle handle = findHandle(hwnd);

					if (FrameWork::isValid(handle)
						&&  (1 & _flags[handle.idx]))
					{
						RECT& rect = *(RECT*)lparam;
						uint32_t width = rect.right - rect.left - _frame_width;
						uint32_t height = rect.bottom - rect.top - _frame_height;

						// Recalculate size according to aspect ratio
						switch (wparam)
						{
						case WMSZ_LEFT:
						case WMSZ_RIGHT:
						{
							float aspectRatio = 1.0f / _aspect_ratio;
							width = std::max(k_window_default_width / 4, width);
							height = uint32_t(float(width)*aspectRatio);
						}
						break;

						default:
						{
							float aspectRatio = _aspect_ratio;
							height = std::max(k_window_default_height / 4, height);
							width = uint32_t(float(height)*aspectRatio);
						}
						break;
						}

						// Recalculate position using different anchor points
						switch (wparam)
						{
						case WMSZ_LEFT:
						case WMSZ_TOPLEFT:
						case WMSZ_BOTTOMLEFT:
							rect.left = rect.right - width - _frame_width;
							rect.bottom = rect.top + height + _frame_height;
							break;

						default:
							rect.right = rect.left + width + _frame_width;
							rect.bottom = rect.top + height + _frame_height;
							break;
						}

						_event_queue.postSizeEvent(findHandle(hwnd), width, height);
					}
				}
				return 0;

				case WM_SIZE:
				{
					FrameWork::WindowHandle handle = findHandle(hwnd);
					if (FrameWork::isValid(handle))
					{
						uint32_t width = GET_X_LPARAM(lparam);
						uint32_t height = GET_Y_LPARAM(lparam);

						_width = width;
						_height = height;
						_event_queue.postSizeEvent(handle, _width, _height);
					}
				}
				break;

				case WM_SYSCOMMAND:
					switch (wparam)
					{
					case SC_MINIMIZE:
					case SC_RESTORE:
					{
						HWND parent = GetWindow(hwnd, GW_OWNER);
						if (NULL != parent)
						{
							PostMessage(parent, id, wparam, lparam);
						}
					}
					}
					break;

				case WM_MOUSEMOVE:
				{
					int32_t mx = GET_X_LPARAM(lparam);
					int32_t my = GET_Y_LPARAM(lparam);

					if (hwnd == _mouse_lock)
					{
						mx -= _mx;
						my -= _my;

						if (0 == mx
							&& 0 == my)
						{
							break;
						}

						setMousePos(hwnd, _mx, _my);
					}

					_event_queue.postMouseEvent(findHandle(hwnd), mx, my, _mz);
				}
				break;

				case WM_MOUSEWHEEL:
				{
					POINT pt = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
					ScreenToClient(hwnd, &pt);
					int32_t mx = pt.x;
					int32_t my = pt.y;
					_mz += GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA;
					_event_queue.postMouseEvent(findHandle(hwnd), mx, my, _mz);
				}
				break;

				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_LBUTTONDBLCLK:
				{
					mouseCapture(hwnd, id == WM_LBUTTONDOWN);
					int32_t mx = GET_X_LPARAM(lparam);
					int32_t my = GET_Y_LPARAM(lparam);
					_event_queue.postMouseEvent(findHandle(hwnd), mx, my, _mz, MouseButton::Left, id == WM_LBUTTONDOWN);
				}
				break;

				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				case WM_MBUTTONDBLCLK:
				{
					mouseCapture(hwnd, id == WM_MBUTTONDOWN);
					int32_t mx = GET_X_LPARAM(lparam);
					int32_t my = GET_Y_LPARAM(lparam);
					_event_queue.postMouseEvent(findHandle(hwnd), mx, my, _mz, MouseButton::Middle, id == WM_MBUTTONDOWN);
				}
				break;

				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_RBUTTONDBLCLK:
				{
					mouseCapture(hwnd, id == WM_RBUTTONDOWN);
					int32_t mx = GET_X_LPARAM(lparam);
					int32_t my = GET_Y_LPARAM(lparam);
					_event_queue.postMouseEvent(findHandle(hwnd), mx, my, _mz, MouseButton::Right, id == WM_RBUTTONDOWN);
				}
				break;

				case WM_KEYDOWN:
				case WM_SYSKEYDOWN:
				case WM_KEYUP:
				case WM_SYSKEYUP:
				{
					uint8_t modifiers = translateKeyModifiers();
					Key key = translateKey(wparam);

					FrameWork::WindowHandle handle = findHandle(hwnd);

					if (Key::Print == key
						&& 0x3 == ((uint32_t)(lparam) >> 30))
					{
						// VK_SNAPSHOT doesn't generate keydown event. Fire on down event when previous
						// key state bit is set to 1 and transition state bit is set to 1.
						//
						// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646280%28v=vs.85%29.aspx
						_event_queue.postKeyEvent(handle, key, modifiers, true);
					}

					_event_queue.postKeyEvent(handle, key, modifiers, id == WM_KEYDOWN || id == WM_SYSKEYDOWN);
				}
				break;

				case WM_CHAR:
				{
					uint8_t utf8[4] = {};
					uint8_t len = (uint8_t)WideCharToMultiByte(CP_UTF8
						, 0
						, (LPCWSTR)&wparam
						, 1
						, (LPSTR)utf8
						, MONSTER_COUNTOF(utf8)
						, NULL
						, NULL
						);
					if (0 != len)
					{
						FrameWork::WindowHandle handle = findHandle(hwnd);
						_event_queue.postCharEvent(handle, len, utf8);
					}
				}
				break;

				default:
					break;
				}
			}

			return DefWindowProc(hwnd, id, wparam, lparam);
		}

		FrameWork::WindowHandle findHandle(HWND hwnd)
		{
			MutexScope scope(_lock);
			for (uint32_t ii = 0, num = _window_alloc.getNumHandles(); ii < num; ++ii)
			{
				uint16_t idx = _window_alloc.getHandleAt(ii);
				if (hwnd == _hwnd[idx])
				{
					FrameWork::WindowHandle handle = { idx };
					return handle;
				}
			}

			FrameWork::WindowHandle invalid = { UINT16_MAX };
			return invalid;
		}

		void clear(HWND hwnd)
		{
			RECT rect;
			GetWindowRect(hwnd, &rect);
			HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
			HDC hdc = GetDC(hwnd);
			SelectObject(hdc, brush);
			FillRect(hdc, &rect, brush);
		}

		void adjust(HWND hwnd, uint32_t width, uint32_t height, bool is_frame)
		{
			_width = width;
			_height = height;
			_aspect_ratio = float(width) / float(height);

			ShowWindow(hwnd, SW_SHOWNORMAL);
			RECT rect;
			RECT newrect = { 0, 0, (LONG)_width, (LONG)_height };
			DWORD style = WS_POPUP | WS_SYSMENU;

			if (_is_frame)
			{
				GetWindowRect(hwnd, &_rect);
				_style = GetWindowLong(hwnd, GWL_STYLE);
			}

			if (is_frame)
			{
				rect = _rect;
				style = _style;
			}
			else
			{
				HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO mi;
				mi.cbSize = sizeof(mi);
				GetMonitorInfo(monitor, &mi);
				newrect = mi.rcMonitor;
				rect = mi.rcMonitor;
			}

			SetWindowLong(hwnd, GWL_STYLE, style);
			uint32_t prewidth = newrect.right - newrect.left;
			uint32_t preheight = newrect.bottom - newrect.top;
			AdjustWindowRect(&newrect, style, FALSE);
			_frame_width = (newrect.right - newrect.left) - prewidth;
			_frame_height = (newrect.bottom - newrect.top) - preheight;
			UpdateWindow(hwnd);

			if (rect.left == -32000
				|| rect.top == -32000)
			{
				rect.left = 0;
				rect.top = 0;
			}

			int32_t left = rect.left;
			int32_t top = rect.top;
			int32_t new_width = (newrect.right - newrect.left);
			int32_t new_height = (newrect.bottom - newrect.top);

			if (!is_frame)
			{
				float aspectRatio = 1.0f / _aspect_ratio;
				new_width = std::max(k_window_default_width / 4, (uint32_t)new_width);
				new_height = uint32_t(float(new_width)*aspectRatio);

				left = newrect.left + (newrect.right - newrect.left - new_width) / 2;
				top = newrect.top + (newrect.bottom - newrect.top - new_height) / 2;
			}

			HWND parent = GetWindow(hwnd, GW_OWNER);
			if (NULL != parent)
			{
				if (is_frame)
				{
					SetWindowPos(parent
						, HWND_TOP
						, -32000
						, -32000
						, 0
						, 0
						, SWP_SHOWWINDOW
						);
				}
				else
				{
					SetWindowPos(parent
						, HWND_TOP
						, newrect.left
						, newrect.top
						, newrect.right - newrect.left
						, newrect.bottom - newrect.top
						, SWP_SHOWWINDOW
						);
				}
			}

			SetWindowPos(hwnd
				, HWND_TOP
				, left
				, top
				, width
				, height
				, SWP_SHOWWINDOW
				);

			ShowWindow(hwnd, SW_RESTORE);

			_is_frame = is_frame;
		}

		void setMousePos(HWND hwnd, int32_t mx, int32_t my)
		{
			POINT pt = { mx, my };
			ClientToScreen(hwnd, &pt);
			SetCursorPos(pt.x, pt.y);
		}

		void setMouseLock(HWND hwnd, bool is_lock)
		{
			if (hwnd != _mouse_lock)
			{
				if (is_lock)
				{
					_mx = _width / 2;
					_my = _height / 2;
					ShowCursor(false);
					setMousePos(hwnd, _mx, _my);
				}
				else
				{
					setMousePos(hwnd, _mx, _my);
					ShowCursor(true);
				}

				_mouse_lock = hwnd;
			}
		}

		static LRESULT CALLBACK wndProc(HWND _hwnd, UINT _id, WPARAM _wparam, LPARAM _lparam);
	};

	static Context s_ctx;

	LRESULT CALLBACK Context::wndProc(HWND _hwnd, UINT _id, WPARAM _wparam, LPARAM _lparam)
	{
		return s_ctx.process(_hwnd, _id, _wparam, _lparam);
	}

	const FrameWork::Event* FrameWork::poll()
	{
		return s_ctx._event_queue.poll();
	}

	const FrameWork::Event* FrameWork::poll(WindowHandle handle)
	{
		return s_ctx._event_queue.poll(handle);
	}

	void FrameWork::release(const FrameWork::Event* _event)
	{
		s_ctx._event_queue.release(_event);
	}

	FrameWork::WindowHandle FrameWork::createWindow(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, uint32_t _flags, const char* _title)
	{
		MutexScope scope(s_ctx._lock);
		FrameWork::WindowHandle handle = { s_ctx._window_alloc.alloc() };

		if (UINT16_MAX != handle.idx)
		{
			Msg* msg = new Msg;
			msg->_x = _x;
			msg->_y = _y;
			msg->_width = _width;
			msg->_height = _height;
			msg->_title = _title;
			msg->_flags = _flags;
			PostMessage(s_ctx._hwnd[0], WM_USER_WINDOW_CREATE, handle.idx, (LPARAM)msg);
		}

		return handle;
	}

	void FrameWork::destroyWindow(FrameWork::WindowHandle handle)
	{
		if (UINT16_MAX != handle.idx)
		{
			PostMessage(s_ctx._hwnd[0], WM_USER_WINDOW_DESTROY, handle.idx, 0);

			MutexScope scope(s_ctx._lock);
			s_ctx._window_alloc.free(handle.idx);
		}
	}

	void FrameWork::setWindowPos(FrameWork::WindowHandle handle, int32_t _x, int32_t _y)
	{
		Msg* msg = new Msg;
		msg->_x = _x;
		msg->_y = _y;
		PostMessage(s_ctx._hwnd[0], WM_USER_WINDOW_SET_POS, handle.idx, (LPARAM)msg);
	}

	void FrameWork::setWindowSize(FrameWork::WindowHandle _handle, uint32_t _width, uint32_t _height)
	{
		PostMessage(s_ctx._hwnd[0], WM_USER_WINDOW_SET_SIZE, _handle.idx, (_height << 16) | (_width & 0xffff));
	}

	void FrameWork::setWindowTitle(FrameWork::WindowHandle handle, const char* _title)
	{
		Msg* msg = new Msg;
		msg->_title = _title;
		PostMessage(s_ctx._hwnd[0], WM_USER_WINDOW_SET_TITLE, handle.idx, (LPARAM)msg);
	}

	void FrameWork::toggleWindowFrame(FrameWork::WindowHandle handle)
	{
		PostMessage(s_ctx._hwnd[0], WM_USER_WINDOW_TOGGLE_FRAME, handle.idx, 0);
	}

	void FrameWork::toggleFullscreen(FrameWork::WindowHandle handle)
	{
	}

	void FrameWork::setMouseLock(FrameWork::WindowHandle handle, bool is_lock)
	{
		PostMessage(s_ctx._hwnd[0], WM_USER_WINDOW_MOUSE_LOCK, handle.idx, is_lock);
	}

	int32_t MainThreadEntry::threadFunc(void* _userData)
	{
		MainThreadEntry* self = (MainThreadEntry*)_userData;
		int32_t result = FrameWork::entry(self->_argc, self->_argv);
		PostMessage(s_ctx._hwnd[0], WM_QUIT, 0, 0);
		return result;
	}

} // namespace entry

int main(int argc, char** argv)
{
	return monster::s_ctx.run(argc, argv);
}

