/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_engine.h
 * - Description
 *   : header file for Loader::Engine
 *     It defines a structure, enumeration and Engine class.
 */

#include <vector>

typedef unsigned char uchar;

#ifndef _LOADER_ENGINE_H
#define _LOADER_ENGINE_H
#include "loader.h"
#include "_begin.h"

struct PATCH_DATA_STRUCT
{
	DWORD address;
	unsigned int size;
	uchar *original;
	uchar *data;

	PATCH_DATA_STRUCT(DWORD addr, int len, uchar *orig, uchar *dat)
	{
		address = addr;
		size = len;
		original = (uchar*) malloc(len);
		data = (uchar*) malloc(len);
		memcpy(original, orig, len);
		memcpy(data, dat, len);
	};
	 ~PATCH_DATA_STRUCT()
	{
		free(original);
		free(data);
	};
};

typedef PATCH_DATA_STRUCT patchData;

enum ENGINE_ERROR_CODE
{
	NO_ENGINE_ERROR = 0,
	UNKNOWN_ENGINE_ERROR,
	NO_EXECUTABLE_ERROR,
	PROCESS_CREATION_ERROR, // CreateProcess() failed
	PROCESS_OPEN_ERROR, // OpenProcess() failed
	MEMORY_READ_ERROR, // ReadProcessMemory() failed
	DATA_MISMATCH_ERROR,
	INFO_RETRIEVE_ERROR, // VirtualQueryEx() failed
	PROTECTION_CHANGE_ERROR, // VirtualProtectEx() failed
	MEMORY_WRITE_ERROR, // WriteProcessMemory() failed
	PATCH_CHECK_ERROR,
	MEMORY_ALLOCATION_ERROR,
	MODULE_HANDLE_RETRIEVE_ERROR,
	MEMORY_WRITE_DLL_ERROR,
	GET_PROC_ADDRESS_ERROR,
	REMOTE_THREAD_CREATION_ERROR
};

class Application;

class Engine
{
protected:
	std::vector<patchData*> patch_data;
	std::vector<char*> dll;
	char *target_executable;

	MEMORY_BASIC_INFORMATION mbi;
	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	HANDLE handle;
	DWORD create_mode;
	DWORD access_mode;
	bool running;

	int patch(patchData *p);

public:
	Engine();
	~Engine();
	bool isRunning(void);
	bool setTargetExecutable(char *path);
	HANDLE getTargetProcess(void);
	DWORD getTargetProcessId(void);
	void addPatch(DWORD address, int size, uchar *original, uchar *data);
#ifdef NEED_DLL_INJECTION
	void addDll(char *path);
	int injectDll(char *path, bool wait = true);
#endif
	int run(Application *application);
	void terminate(void);
};

#include "_end.h"
#endif

//EOF
