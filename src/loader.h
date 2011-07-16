/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader.h
 * - Description
 *   : common includes and macro definitions
 */

#include <windows.h>
#include <stdio.h>

//#define DEBUG_MODE 1

#ifndef _LOADER_H
#define _LOADER_H
#include "_begin.h"

#ifndef DEBUG
#ifdef DEBUG_MODE
	#define DEBUG ::printf
#else
	#define DEBUG(...) ;
#endif
#endif

#define LOADER_FRAMEWORK_AUTHOR        "t3RRa"
#define LOADER_FRAMEWORK_VERSION_MAJOR 0
#define LOADER_FRAMEWORK_VERSION_MINOR 1
#define LOADER_FRAMEWORK_VERSION_FULL  "0.1 Final"
#define LOADER_FRAMEWORK_BUILD_DATE    "20110716"
#define LOADER_FRAMEWORK_LICENSE       "2-clause BSD"
#include "_end.h"
#endif

//EOF
