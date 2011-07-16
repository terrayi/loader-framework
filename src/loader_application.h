/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_application.h
 * - Description
 *   : header file for Loader::Application
 */

#ifndef _LOADER_APPLICATION_H
#define _LOADER_APPLICATION_H
#include "loader.h"
#define __PARAMS__ HINSTANCE _hInstance, \
			HINSTANCE _hPrevInstance, \
			LPSTR _lpCmdLine, \
			int _nCmdShow
#define MAIN int CALLBACK WinMain(__PARAMS__)
#define RUN run(_hInstance, _hPrevInstance, _lpCmdLine, _nCmdShow)
#include "_begin.h"

class Engine;

class Application
{
protected:
	HINSTANCE hInstance;
	LPSTR lpCmdLine;
	int nCmdShow;

public:
	Application();
	virtual ~Application();
	virtual int onStartUp(Engine *engine) = 0;
	virtual int beforePatching(Engine *engine) { return 0; };
	virtual int afterPatching(Engine *engine) { return 0; };
	virtual int onClose(Engine *engine) { return 0; };
	virtual int onError(int code, bool critical = false) { return 1; };
	int run(__PARAMS__);
};

#include "_end.h"
#endif

//EOF
