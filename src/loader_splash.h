/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_splash.h
 * - Description
 *   : 
 */

#include "loader_window.h"

#ifndef _LOADER_SPLASH_H
#define _LOADER_SPLASH_H
#include "_begin.h"

class Splash : public IActor
{
protected:
	HBITMAP _image;
	HBITMAP old_bitmap;
	int _image_id;
	int _width;
	int _height;
	DWORD _duration;
	DWORD _start_time;
	DWORD _elapsed_time;
	bool finished;

public:
	Splash(int image_id, DWORD duration);
	virtual ~Splash();
	LRESULT onCreate(Window *lw) { return 0; };
	LRESULT onPaint(Window *lw);
	LRESULT onDestroy(Window *lw);
	bool onLoad(Window *lw);
	void beforeLoop(Window *lw);
	void inLoop(Window *lw);
	bool running(void);

	window_settings_type getDefaultSettings(void);
	static void quickRun(int image_id, DWORD duration);
};

#include "_end.h"
#endif

//EOF
