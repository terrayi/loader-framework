/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_utility.cpp
 * - Description
 *   : implementation of Loader::Utility
 *     This implements some useful utility features, ease as a static method.
 */

// Under MinGW32 to use CheckTokenMembership() in winbase.h
// _WIN32_WINNT must be >= 0x0500
#ifdef __MINGW32__
#define _WIN32_WINNT 0x0500
#endif

//#define DEBUG_MODE 1

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>
#include <winnt.h>
#include "loader.h"
#include "loader_utility.h"

#ifdef __MINGW32__
// defines enums and other undefined stuffs on MinGW32.. (incomplete)
#include "loader_utility_defs.h"
#endif

// defines enums and types related retrieving disk serial
#include "loader_utility_device_defs.h"
#include "_begin.h"

OSVERSIONINFO
Utility::getWindowsVersion(void)
{
	OSVERSIONINFO version_info;

	::ZeroMemory(&version_info, sizeof(OSVERSIONINFO));
	version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (::GetVersionEx(&version_info) == 0)
	{
		version_info.dwOSVersionInfoSize = 0;
	}

	return version_info;
}

DWORD
Utility::getWindowsMajorVersion(void)
{
	OSVERSIONINFO version_info;

	version_info = Utility::getWindowsVersion();

	if (version_info.dwOSVersionInfoSize == 0)
	{
		return 0;
	}

	return version_info.dwMajorVersion;
}

DWORD
Utility::getWindowsMinorVersion(void)
{
	OSVERSIONINFO version_info;

	version_info = Utility::getWindowsVersion();

	if (version_info.dwOSVersionInfoSize == 0)
	{
		return 0;
	}

	return version_info.dwMinorVersion;
}

bool
Utility::isWindowsVistaOrUp(void)
{
	DWORD major_version = 0;

	major_version = Utility::getWindowsMajorVersion();

	if (major_version >= 6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
Utility::isWindowsXPOrLess(void)
{
	DWORD major_version = 0;

	major_version = Utility::getWindowsMajorVersion();

	if (major_version != 0 && major_version < 6)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int
Utility::getDiskSerial(char *path, char** serial)
{
	HANDLE handle;
	DWORD generic_option = GENERIC_READ | GENERIC_WRITE;
	DWORD file_share_option = FILE_SHARE_READ | FILE_SHARE_WRITE;
	BOOL result = FALSE;
	DWORD length = 0;

	SENDCMDINPARAMS sci;
	SENDCMDOUTPARAMS sco;

	handle = ::CreateFile(
			path,
			generic_option,
			file_share_option,
			0,
			OPEN_EXISTING,
			0,
			NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return 1;
	}

	::ZeroMemory(&sci, sizeof(sci));
	::ZeroMemory(&sco, sizeof(sco));

	sci.DriveNumber = 0;
	sci.BufferSize = sizeof(sco);
	sci.DriveRegs.DriveHead = 0xa0;
	sci.DriveRegs.Command = 0xec;
	sci.DriveRegs.SectorCount = 1;
	sci.DriveRegs.SectorNumber = 1;

	result = ::DeviceIoControl(
			handle,
			DFP_RECEIVE_DRIVE_DATA,
			(LPVOID) &sci,
			sizeof(sci),
			(LPVOID) &sco,
			sizeof(sco),
			&length,
			NULL);

	::CloseHandle(handle);

	if (!result)
	{
		return (int) GetLastError();
	}

	*serial = (char*) malloc(SERIAL_NUMBER_LENGTH + 1);
	::ZeroMemory(*serial, SERIAL_NUMBER_LENGTH + 1);
	::memcpy(*serial, sco.IDS.SerialNumber, SERIAL_NUMBER_LENGTH);
	(*serial)[SERIAL_NUMBER_LENGTH] = 0;

	return 0;
}

int
Utility::getFirstPhysicalDiskSerial(char** serial)
{
	return Utility::getDiskSerial(FIRST_PHYSICAL_DRIVE, serial);
}

/*
  source : http://msdn.microsoft.com/en-us/library/aa376389(VS.85).aspx
  header : winbase.h (include windows.h)
  library: advapi32.dll
*/
bool
Utility::isUnderAdministratorCredentialStandard(void)
{
	BOOL b;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;

	DEBUG("Utility::isUnderAdministratorCredentialStandard()\n");
	DEBUG("calling ::AllocateAndInitializeSid()\n");

	b = ::AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&AdministratorsGroup);

	if (b)
	{
		DEBUG("calling ::CheckTokenMembership()\n");

		if (!::CheckTokenMembership(NULL, AdministratorsGroup, &b))
		{
			b = FALSE;
		}

		DEBUG("calling ::FreeSid()\n");
		::FreeSid(AdministratorsGroup);
	}

	DEBUG("returning with %d\n", b);
	return b;
}

bool
Utility::isUnderAdministratorCredentialWithUAC(void)
{
	HANDLE token = NULL;
	HANDLE token_to_check = NULL;
	DWORD desired_access = TOKEN_QUERY | TOKEN_DUPLICATE;
	TOKEN_ELEVATION_TYPE elevation_type;
	BOOL is_admin_group = FALSE;
	DWORD size = 0;
	BOOL result = FALSE;
	BYTE sid[SECURITY_MAX_SID_SIZE];

	DEBUG("Utility::isUnderAdministratorCredentialWithUAC()\n");
	DEBUG("calling ::OpenProcessToken()\n");

	if (::OpenProcessToken(::GetCurrentProcess(), desired_access, &token))
	{
		return is_admin_group;
	}

	DEBUG("calling Utiliyu::isWindowsVistaOrUp()\n");

	if (Utility::isWindowsVistaOrUp())
	{
		DEBUG("calling ::GetTokenInformation()\n");
		result = ::GetTokenInformation(
				token,
				(TOKEN_INFORMATION_CLASS) TokenElevationType,
				&elevation_type,
				sizeof(elevation_type),
				&size);

		if (!result)
		{
			::CloseHandle(token);
			return is_admin_group;
		}

		if (elevation_type == TokenElevationTypeLimited)
		{
			DEBUG("calling ::GetTokenInformation()\n");
			result = ::GetTokenInformation(
					token,
					(TOKEN_INFORMATION_CLASS)
						TokenLinkedToken,
					&token_to_check,
					sizeof(token_to_check),
					&size);

			if (!result)
			{
				::CloseHandle(token);
				return is_admin_group;
			}
		}
	}

	if (!token_to_check)
	{
		DEBUG("calling ::DuplicateToken()\n");
		result = ::DuplicateToken(
				token,
				SecurityIdentification,
				&token_to_check);


		if (!result)
		{
			::CloseHandle(token);
			return is_admin_group;
		}
	}

#ifdef __MINGW32__
	DEBUG("getting CreateWellKnownSid()\n");
	CreateWellKnownSid = (fnCWKStype) ::GetProcAddress(
			::GetModuleHandle(TEXT("advapi32.dll")),
			"CreateWellKnownSid");

#endif
	DEBUG("calling ::CreateWellKnownSid()\n");
	size = sizeof(sid);
	result = ::CreateWellKnownSid(
			WinBuiltinAdministratorsSid,
			NULL,
			&sid,
			&size);

	if (result)
	{
		DEBUG("calling ::CheckTokenMembership()\n");
		::CheckTokenMembership(token_to_check, &sid, &is_admin_group);
	}

	DEBUG("closing handles\n");
	::CloseHandle(token);
	::CloseHandle(token_to_check);

	DEBUG("returning with %d\n", is_admin_group);
	return is_admin_group;
}

bool
Utility::isElevatedProcess(void)
{
	HANDLE token = NULL;
	TOKEN_ELEVATION elevation;
	DWORD size;
	BOOL result = FALSE;
	bool elevated = false;

	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token))
	{
		return elevated;
	}

	result = ::GetTokenInformation(
			token,
			(TOKEN_INFORMATION_CLASS) TokenElevation,
			&elevation,
			sizeof(TOKEN_ELEVATION),
			&size);

	if (!result)
	{
		::CloseHandle(token);
		return elevated;
	}

	elevated = elevation.TokenIsElevated;
	::CloseHandle(token);

	return elevated;
}

/*
bool
Utility::elevatePrivileges(void)
{
	char path[MAX_PATH + 1];
	SHELLEXECUTEINFO se_info;
	BOOL result = FALSE;

	result = ::GetModuleFileName(NULL, path, MAX_PATH);

	if (!result)
	{
		DEBUG("Could not get module file name\n");
		return false;
	}

	DEBUG("File name: %s\n", path);

	::ZeroMemory(&se_info, sizeof(SHELLEXECUTEINFO));
	se_info.cbSize = sizeof(SHELLEXECUTEINFO);
	se_info.lpVerb = "runas";
	se_info.lpFile = path;
	se_info.hwnd = NULL;
	se_info.nShow = SW_NORMAL;

	result = ::ShellExecuteEx(&se_info);

	if (result)
	{
		return true;
	}
	else
	{
		return false;
	}
}
*/

#include "_end.h"

//EOF
