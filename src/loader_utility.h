/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_utility.h
 * - Description
 *   : header file for Loader::Utility
 */

#ifndef _LOADER_UTILITY_H
#define _LOADER_UTILITY_H
#include "_begin.h"

#ifndef FIRST_PHYSICAL_DRIVE
#define FIRST_PHYSICAL_DRIVE "\\\\.\\PhysicalDrive0"
#endif

class Utility
{
public:
	static OSVERSIONINFO getWindowsVersion(void);
	static DWORD getWindowsMajorVersion(void);
	static DWORD getWindowsMinorVersion(void);
	static bool isWindowsVistaOrUp(void);
	static bool isWindowsXPOrLess(void);
	static int getDiskSerial(char *path, char** serial);
	static int getFirstPhysicalDiskSerial(char** serial);
	static bool isUnderAdministratorCredentialStandard(void);
	static bool isUnderAdministratorCredentialWithUAC(void);
	static bool isElevatedProcess(void);
	//static bool elevatePrivileges(void);
};

#include "_end.h"
#endif

//EOF
