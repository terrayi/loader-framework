/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_application.cpp
 * - Description   
 *   : implementation of Loader::Application.
 *     Every loader should inherit this and implement appropriate methods.
 */

#include "loader_application.h"
#include "loader_engine.h"
#include "_begin.h"

Application::Application()
{
}

Application::~Application()
{
	DEBUG("Loader::Application freed\n");
}

int
Application::run(__PARAMS__)
{
	Engine *engine = NULL;

	hInstance = _hInstance;
	(void) _hPrevInstance;
	lpCmdLine = _lpCmdLine;
	nCmdShow = _nCmdShow;

	engine = new Engine();
	engine->run(this);
	delete engine;
	engine = NULL;

	return 0;
}

#include "_end.h"
//EOF
