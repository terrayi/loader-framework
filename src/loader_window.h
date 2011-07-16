/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_windows.h
 * - Description
 *   : header file for Loader::Window
 *     This also defines IActor interface which react to certain
 *     Windows message events. Currently only a few messages are handled
 *     useful for Loader::Splash
 */

#ifndef _LOADER_WINDOW_H
#define _LOADER_WINDOW_H
#include "_begin.h"

#define PROC_ARGS HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
#define WCI window_class_information

class Window;

class IActor
{
public:
	virtual LRESULT onCreate(Window *lw) {
		return 0;
	};
	//virtual LRESULT onClose(...);
	virtual LRESULT onPaint(Window *lw) {
		return 0;
	};
	virtual LRESULT onDestroy(Window *lw) {
		return 0;
	};
	virtual bool onLoad(Window *lw) { return TRUE; };
	virtual void beforeLoop(Window *lw) {};
	virtual void inLoop(Window *lw) = 0;
	virtual bool running(void) = 0;
};

typedef struct 
{
	DWORD window_exStyle;
	DWORD window_style;
	LPCTSTR window_title;
	UINT class_style;
	HICON class_icon;
	HICON class_small_icon;
	HCURSOR class_cursor;
	HBRUSH class_background;
	LPCTSTR class_name;
} window_settings_type;

class Window
{
protected:
	IActor *_actor;
	WNDCLASSEX WCI;
	HWND _window_handle;
	DWORD _dwExStyle;
	DWORD _dwStyle;
	LPCTSTR _title;
	int _x, _y, _width, _height;

	void initWindowClassInformation(window_settings_type *settings);
	LRESULT __processMessage(PROC_ARGS);

public:
	Window(IActor *actor, window_settings_type *settings);
	~Window();

	HWND getHandle(void);
	void setWindowPosition(int x, int y);
	void setWindowSize(int width, int height);
	int run(void);

	static LRESULT CALLBACK processMessage(PROC_ARGS);

	enum RUN_RETURN_VALUE
	{
		OK,
		UNKNOWN_ERROR,
		CLASS_REGISTRATION_ERROR,
		WINDOW_CREATION_ERROR
	};
};

#include "_end.h"
#endif

//EOF
