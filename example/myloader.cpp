
#define EXECUTABLE ".\\aGame.exe"
#define PRIVILEGE_ERROR "You must run this under Administrator account"

#include "resource.h"
#include "../src/loader_engine.h"
#include "../src/loader_splash.h"
#include "../src/loader_utility.h"
#include "myloader.h"

using namespace Loader;

MyLoader::MyLoader()
{
}

MyLoader::~MyLoader()
{
}

// called before anything else in the beginning.
// load splash and/or do task related to protection scheme etc.
int
MyLoader::onStartUp(Loader::Engine *e)
{
	char *serial;

	printf("\nExampleLoader v1.0\n\n");

	if (Utility::getFirstPhysicalDiskSerial(&serial) == 0)
	{
		char msg[100];
		::ZeroMemory(msg, 100);
		sprintf(msg, "Serial for the first physical drive: %s", serial);
		DEBUG("Serial for the first physical drive: %s\n", serial);
		::MessageBox(NULL, msg, "Disk Serial", 0);
	}
	else
	{
		::MessageBox(NULL, "Serial not retrieved..", "Info", 0);
	}

	if (!e->setTargetExecutable(EXECUTABLE))
	{
		::MessageBox(NULL, "Game executable not found", "Error", 0);
		return 1;
	}

	if (Utility::isUnderAdministratorCredentialWithUAC())
	{
		if (!Utility::isElevatedProcess())
		{
			::MessageBox(NULL, "UAC Problem", "Info", 0);
			return 1;
		}
	}
	else if (!Utility::isUnderAdministratorCredentialStandard())
	{
		::MessageBox(NULL, PRIVILEGE_ERROR, "Info", 0);
		return 1;
	}

	Splash::quickRun(ID_SPLASH, 2000);

	return 0;
}

// called before patching in memory happens. add patch data here
int
MyLoader::beforePatching(Loader::Engine *e)
{
	uchar cpatch[2] = {0x80, 0x81};
	uchar *next_patch0;
	uchar *next_patch1;
	uchar *another_data;
	uchar *another_data_patch;
	// ...

	// Charset patch
	e->addPatch(0x00444444, 1, &(cpatch[0]), &(cpatch[1]));

	// More patches
	e->addPatch(0x00433333, 2, next_patch0, next_patch1);
	e->addPatch(0x00422222, 5, another_data, another_data_patch);
	// ...

	return 0;
}

// called when the target application is patched in memory
int
MyLoader::afterPatching(Loader::Engine *e)
{
	//e->injectDll("..."); in here or afterRunning() only
	// or
	//e->addDll("..."); in onStartUp(), beforePatching() or here only
	return 0;
}

// called after the target application is started
// DO not try to run intensive tasks here
int
MyLoader::afterRunning(Loader::Engine *e)
{
	return 0;
}

// called when the target application is closed
int
MyLoader::onClose(Loader::Engine *e)
{
	::MessageBox(NULL, "Thank you for playing!", "Info", 0);
	return 0;
}

// called when an error occurs during running(including patching)
int
MyLoader::onError(int code, bool critical)
{
	char msg[100];
	sprintf(msg, "Error occured! error code: %d", code);
	::MessageBox(NULL, msg, "Error", 0);
	return 1;
}

MAIN
{
	MyLoader m;

	return m.RUN;
}

//EOF
