/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_windows.cpp
 * - Description
 *   : implementation of Loader::Window
 *     This implements Window similar to Loader::Engine.
 *     Loader::Splash requires this to run. 
 */

//#define DEBUG_MODE 1

#include "loader.h"
#include "loader_window.h"
#include "_begin.h"

Window::Window(IActor *actor, window_settings_type *settings)
{
	_actor = actor;
	_dwExStyle = settings->window_exStyle;
	_dwStyle = settings->window_style;
	_title = TEXT(settings->window_title);

	initWindowClassInformation(settings);
	setWindowPosition(0, 0);
	setWindowSize(0, 0);
}

Window::~Window()
{
}

HWND
Window::getHandle(void)
{
	return _window_handle;
}

void
Window::setWindowPosition(int x, int y)
{
	_x = x;
	_y = y;
}

void
Window::setWindowSize(int width, int height)
{
	_width = width;
	_height = height;
}

int
Window::run(void)
{
	MSG msg;

	DEBUG("Loader::Window::run()\n");

	if (!_actor->onLoad(this))
	{
		return UNKNOWN_ERROR;
	}

	if (!::RegisterClassEx(&WCI))
	{
		return CLASS_REGISTRATION_ERROR;
	}

	_window_handle = ::CreateWindowEx(
		_dwExStyle, WCI.lpszClassName, _title, _dwStyle,
		_x, _y, _width, _height, NULL, NULL, NULL, NULL);

	if (!_window_handle)
	{
		return WINDOW_CREATION_ERROR;
	}

	DEBUG("\t::SetWindowLongPtr(...)\n");
	::SetWindowLongPtr(_window_handle, GWLP_USERDATA, (LONG_PTR) this);

	::ShowWindow(_window_handle, SW_SHOW);
	::UpdateWindow(_window_handle);

	_actor->beforeLoop(this);

	while (_actor->running())
	{
		if (::PeekMessage(&msg, _window_handle, 0, 0, PM_REMOVE) != -1)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		_actor->inLoop(this);
	}

	//_actor->afterLoop(this); ?

        ::DestroyWindow(_window_handle);
        _window_handle = 0;
        ::UnregisterClass(WCI.lpszClassName, GetModuleHandle(NULL));

	return OK;
}

LRESULT CALLBACK
Window::processMessage(PROC_ARGS)
{
	Window *instance;

	DEBUG("Loader::Window::processMessage()\n");

	instance = (Window*) ::GetWindowLongPtr(hwnd, GWLP_USERDATA);

	return instance->__processMessage(hwnd, uMsg, wParam, lParam);
}

LRESULT
Window::__processMessage(PROC_ARGS)
{
	DEBUG("Loader::Window::__processMessage()\n");

	switch (uMsg)
	{
		case WM_CREATE:
			//_actor->onCreate(this);
			break;

/*
		case WM_TIMER:
			break;
*/

/*
		case WM_COMMAND:
			break;
*/

		case WM_DESTROY:
			_actor->onDestroy(this);
			PostQuitMessage(0);
			break;

/*
		case WM_ERASEBKGND:
			return 1;
*/

/*
		case WM_SIZE:
			break;
*/

		case WM_PAINT:
			return _actor->onPaint(this);

		default:
			//DEBUG("uMsg: %u\n", uMsg);
			return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return 0;
}

void
Window::initWindowClassInformation(window_settings_type *settings)
{
	WCI.cbSize = sizeof(WNDCLASSEX);
	WCI.style = settings->class_style;
	WCI.lpfnWndProc = Window::processMessage;
	WCI.cbClsExtra = 0;
	WCI.cbWndExtra = DLGWINDOWEXTRA;
	WCI.hInstance = ::GetModuleHandle(NULL);
	WCI.hIcon = settings->class_icon;
	WCI.hCursor = settings->class_cursor;
	WCI.hbrBackground = settings->class_background;
	WCI.lpszMenuName = NULL;
	WCI.lpszClassName = TEXT(settings->class_name);
	WCI.hIconSm = settings->class_small_icon;
}

#include "_end.h"
//EOF
