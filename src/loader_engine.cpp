/*
 * Loader Framework
 *
 * - Author  : t3RRa
 * - License : 2-clause BSD license (see LICENSE file)
 * - File    : loader_engine.cpp
 * - Description   
 *   : implementation of Loader::Engine.
 *     This class does all the nitty gritty of loading and patching executable.
 */

//#define DEBUG_MODE 1
//#define NEED_DLL_INJECTION 1
#include "loader_application.h"
#include "loader_engine.h"
#include "_begin.h"

Engine::Engine()
{
	target_executable = 0;
	//create_mode = CREATE_SUSPENDED|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS;
	create_mode = CREATE_SUSPENDED;
	//access_mode = PROCESS_ALL_ACCESS;
	access_mode = MAXIMUM_ALLOWED;
	running = false;
}

Engine::~Engine()
{
	patch_data.clear();

	#ifdef NEED_DLL_INJECTION
		dll.clear();
	#endif

	if (target_executable)
	{
		free(target_executable);
	}

	DEBUG("Loader::Engine freed\n");
}

bool
Engine::isRunning(void)
{
	DEBUG("Loader::Engine::isRunning()\n");
	return running;
}

bool
Engine::setTargetExecutable(char *path)
{
	HANDLE file_handle = NULL;
	size_t length = 0;

	if (target_executable)
	{
		free(target_executable);
		target_executable = 0;
	}

	// CHECK EXISTENCE
	file_handle = ::CreateFile(
				TEXT(path),
				GENERIC_READ,
				0,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	CloseHandle(file_handle);

	length = strlen(path);
	target_executable = (char*) malloc(length + 1);
	strcpy(target_executable, path);
	target_executable[length] = 0;

	return true;
}

HANDLE
Engine::getTargetProcess(void)
{
	return handle;
}

DWORD
Engine::getTargetProcessId(void)
{
	return process_info.dwProcessId;
}

void
Engine::addPatch(DWORD address, int size, uchar *original, uchar *data)
{
	patchData *new_data;

	new_data = new patchData(address, size, original, data);
	patch_data.push_back(new_data);
}

#ifdef NEED_DLL_INJECTION
void
Engine::addDll(char* path)
{
	dll.push_back(path);
}

int
Engine::injectDll(char *path, bool wait)
{
	HANDLE thread_handle = NULL;
	HMODULE module = NULL;
	LPVOID remote_buffer = NULL;
	LPTHREAD_START_ROUTINE thread_proc = NULL;
	BOOL result = FALSE;

	remote_buffer = ::VirtualAllocEx(
				handle,
				NULL,
				strlen(path),
				MEM_COMMIT | MEM_RESERVE,
				PAGE_READWRITE);

	if (remote_buffer == NULL)
	{
		return MEMORY_ALLOCATION_ERROR;
	}

	result = ::WriteProcessMemory(
			handle,
			remote_buffer,
			path,
			strlen(path),
			NULL);

	if (result == 0)
	{
		return MEMORY_WRITE_DLL_ERROR;
	}

	module = ::GetModuleHandle("kernel32.dll");

	if (module == NULL)
	{
		return MODULE_HANDLE_RETRIEVE_ERROR;
	}

	thread_proc = (LPTHREAD_START_ROUTINE)
			::GetProcAddress(module, "LoadLibraryA");

	if (thread_proc == NULL)
	{
		return GET_PROC_ADDRESS_ERROR;
	}

	thread_handle = ::CreateRemoteThread(
				handle,
				NULL,
				0,
				thread_proc,
				remote_buffer,
				0,
				NULL);

	if (thread_handle == NULL)
	{
		return REMOTE_THREAD_CREATION_ERROR;
	}

	if (wait)
	{
		::WaitForSingleObject(thread_handle, INFINITE);
	}

	return 0;
}
#endif

int
Engine::run(Application *application)
{
	std::vector<patchData*>::iterator iter;

	#ifdef NEED_DLL_INJECTION
	std::vector<char*>::iterator iterd;
	#endif

	BOOL result = FALSE;
	int error_code = 0;

	// ANY INITIALIZATION
	::ZeroMemory(&mbi, sizeof(MEMORY_BASIC_INFORMATION));
	::ZeroMemory(&startup_info, sizeof(STARTUPINFO));
	startup_info.cb = sizeof(STARTUPINFO);

	DEBUG("Loader::Engine::run()\n");

	error_code = application->onStartUp(this);

	if (error_code != 0)
	{
		return error_code;
	}

	if (target_executable == 0 || strlen(target_executable) < 4)
	{
		return NO_EXECUTABLE_ERROR;
	}

	// LOAD
	DEBUG("calling ::CreateProcess()\n");
	result = ::CreateProcess(
			TEXT(target_executable),
			NULL, NULL, NULL, FALSE,
			create_mode,
			NULL, NULL,
			&startup_info,
			&process_info);

	if (!result)
	{
		terminate();
		return application->onError(PROCESS_CREATION_ERROR, true);
	}

	DEBUG("calling ::OpenProcess()\n");
	handle = ::OpenProcess(access_mode, TRUE, process_info.dwProcessId);

	if (handle == NULL)
	{
		terminate();
		return application->onError(PROCESS_OPEN_ERROR, true);
	}

	DEBUG("process beforePatching..\n");
	application->beforePatching(this);

	// PATCH!!
	if (patch_data.size() > 0)
	for (iter = patch_data.begin(); iter < patch_data.end(); iter++)
	{
		result = patch(*iter);

		if (result != NO_ENGINE_ERROR)
		{
			terminate();
			return application->onError(result, true);
		}
	}

	DEBUG("process afterPatching..\n");
	application->afterPatching(this);

	#ifdef NEED_DLL_INJECTION
	// INJECT DLLs
	if (dll.size() > 0)
	for (iterd = dll.begin(); iterd < dll.end(); iterd++)
	{
		result = injectDll(*iterd);

		if (result != NO_ENGINE_ERROR)
		{
			//DEBUG();
		}
	}
	#endif

	// RUN AND WAIT
	DEBUG("Resuming the patched executable thread on %d in %s\n",
		__LINE__, __FILE__);
	running = true;
	DEBUG("calling ::ResumeThread(%d)\n", (int) process_info.hThread);
	::ResumeThread(process_info.hThread);

	// WAIT TIL PROGRAM ENDS
	DEBUG("waiting for the thread to finish\n");
	DEBUG("calling ::WaitForSingleObject()\n");
	::WaitForSingleObject(process_info.hThread, INFINITE);

	// CLOSING
	DEBUG("process onClose..\n");
	application->onClose(this);

	// ANY UNINITIALIZATION
	DEBUG("calling ::CloseHandle()\n");
	::CloseHandle(process_info.hThread);
	::CloseHandle(process_info.hProcess);
	running = false;

	DEBUG("leaving Loader::Engine::run()\n");
	return 0;
}

void
Engine::terminate(void)
{
	if (running && process_info.hProcess)
	{
		::TerminateProcess(process_info.hProcess, 0);
	}

	running = false;
}

int
Engine::patch(patchData *p)
{
	int state = NO_ENGINE_ERROR;
	uchar *buffer = 0;
	DWORD old_protection;
	BOOL result;
	SIZE_T bytes;

	DEBUG("Loader::Engine::patch(0x%08x, %d, ...)\n",
		(unsigned int) p->address,
		p->size);

	buffer = (uchar*) malloc(p->size);

	DEBUG("calling ::ReadProcessMemory() on %d in %s\n",
		__LINE__, __FILE__);
	result = ::ReadProcessMemory(
			process_info.hProcess,
			(LPCVOID) p->address,
			buffer,
			p->size,
			NULL);

	if (result == 0)
	{
		state = MEMORY_READ_ERROR;
		goto patch_clear;
	}

	if (::memcmp(buffer, p->original, p->size) != 0)
	{
		state = DATA_MISMATCH_ERROR;
		goto patch_clear;
	}

	DEBUG("calling ::VirtualQueryEx() on %d in %s\n", __LINE__, __FILE__);
	result = ::VirtualQueryEx(
			handle,
			(LPVOID) p->address,
			&mbi,
			sizeof(mbi));

	if (result == 0)
	{
		state = INFO_RETRIEVE_ERROR;
		goto patch_clear;
	}

	DEBUG("calling ::VirtualProtectEx() on %d in %s\n", __LINE__, __FILE__);
	result = ::VirtualProtectEx(
			handle,
			mbi.BaseAddress,
			mbi.RegionSize,
			PAGE_EXECUTE_READWRITE,
			&old_protection);

	if (result == 0)
	{
		state = PROTECTION_CHANGE_ERROR;
		goto patch_clear;
	}

	DEBUG("calling ::WriteProcessMemory() on %d in %s\n",
		__LINE__, __FILE__);
	result = ::WriteProcessMemory(
			handle,
			(LPVOID) p->address,
			(LPCVOID) p->data,
			(SIZE_T) p->size,
			&bytes);

	if (result == 0)
	{
		state = MEMORY_WRITE_ERROR;
		goto patch_clear;
	}

	DEBUG("calling ::VirtualProtectEx() on %d in %s\n",
		__LINE__, __FILE__);
	result = ::VirtualProtectEx(
			handle,
			mbi.BaseAddress,
			mbi.RegionSize,
			old_protection,
			NULL);

	if (result == 0)
	{
/*
		// reverting back the protection returns nonzero
		// so it fails at least on Windows 7 and probably on Vista..
		//state = PROTECTION_CHANGE_ERROR;
		//goto patch_clear;
*/
	}

	DEBUG("calling ::ReadProcessMemory() on %d in %s\n",
		__LINE__, __FILE__);
	result = ::ReadProcessMemory(
			process_info.hProcess,
			(LPCVOID) p->address,
			buffer,
			p->size,
			NULL);

	if (result == 0)
	{
		state = MEMORY_READ_ERROR;
		goto patch_clear;
	}

	if (::memcmp(buffer, p->data, p->size) != 0)
	{
		state = PATCH_CHECK_ERROR;
	}

patch_clear:
	free(buffer);
	return state;
}

#include "_end.h"

//EOF
