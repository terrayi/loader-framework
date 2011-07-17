/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_splash.cpp
 * - Description
 *   : implementation of Loader::Splash.
 *     For ease of loading up a splash image
 */

//#define DEBUG_MODE 1

#pragma comment(lib, "winmm.lib")

#include "loader.h"
#include "loader_splash.h"
#include "_begin.h"

Splash::Splash(int image_id, DWORD duration)
{
	_image = NULL;
	_image_id = image_id;
	_width = 0;
	_height = 0;
	_duration = duration;
	_start_time = 0;
	_elapsed_time = 0;
	finished = FALSE;
}

Splash::~Splash()
{
	if (_image != NULL)
	{
		DeleteObject(_image);
	}
}

LRESULT
Splash::onPaint(Window *lw)
{
	HWND handle;
	PAINTSTRUCT ps;
	HDC dc = 0;
	HDC memdc = 0;
	HBITMAP old_bitmap = 0;
	//RECT rect;

	DEBUG("Loader::Splash::onPaint()\n");
	handle = lw->getHandle();
	//::GetClientRect(handle, &rect);

	dc = ::BeginPaint(handle, &ps);
	memdc = ::CreateCompatibleDC(dc);
	old_bitmap = (HBITMAP) ::SelectObject(memdc, _image);

	::BitBlt(dc, 0, 0, _width, _height, memdc, 0, 0, SRCCOPY);
	::SelectObject(memdc, old_bitmap);
	::DeleteDC(memdc);
	::EndPaint(handle, &ps);

	return 0;
}

bool
Splash::onLoad(Window *lw)
{
	BITMAP bitmap;
	int x = 0;
	int y = 0;

	DEBUG("Loader::Splash::onLoad()\n");

	_image = ::LoadBitmap(
		::GetModuleHandle(NULL), MAKEINTRESOURCE(_image_id));

	::GetObject(_image, sizeof(BITMAP), &bitmap);
	_width = bitmap.bmWidth;
	_height = bitmap.bmHeight;

	x = (::GetSystemMetrics(SM_CXFULLSCREEN) - _width) / 2;
	y = (::GetSystemMetrics(SM_CYFULLSCREEN) - _height) / 2;

	lw->setWindowPosition(x, y);
	lw->setWindowSize(_width, _height);

	DEBUG("\twindow position: (%d, %d)\n\timage size: %d x %d\n",
		x, y, _width, _height);

	return TRUE;
}

LRESULT
Splash::onDestroy(Window *lw)
{
	::DeleteObject(_image);
	return 0;
}

void
Splash::beforeLoop(Window *lw)
{
	DEBUG("Loader::Splash::beforeLoop()\n");
	_start_time = ::timeGetTime();
}

void
Splash::inLoop(Window *lw)
{
	DEBUG("Loader::Splash::inLoop()\n");
	_elapsed_time = ::timeGetTime() - _start_time;

	if (_elapsed_time >= _duration)
	{
		finished = TRUE;
		::PostMessage(lw->getHandle(), WM_CLOSE, 0, 0);
	}
}

bool
Splash::running(void)
{
	return !finished;
}

window_settings_type
Splash::getDefaultSettings(void)
{
	window_settings_type settings;

	DEBUG("Loader::Splash::getDefaultSettings()\n");
	settings.window_exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
	settings.window_style = WS_POPUP;
	settings.window_title = TEXT("SPLASH");
	settings.class_style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
	settings.class_icon = NULL;
	settings.class_cursor = ::LoadCursor(NULL, IDC_WAIT);
	settings.class_background = 0;
	settings.class_name = TEXT("SPLASH");
	settings.class_small_icon = NULL;

	return settings;
}

void
Splash::quickRun(int image_id, DWORD duration)
{
	Window *w;
	Splash *s;
	window_settings_type settings;

	s = new Splash(image_id, duration);
	settings = s->getDefaultSettings();
	w = new Window((IActor*) s, &settings);

	w->run();

	delete s;
	delete w;
}

#include "_end.h"
//EOF
