
#include "../src/loader_application.h"

class MyLoader : public Loader::Application
{
public:
	MyLoader();
	~MyLoader();
	int onStartUp(Loader::Engine *e);
	int beforePatching(Loader::Engine *e);
	int afterPatching(Loader::Engine *e);
	int afterRunning(Loader::Engine *e);
	int onClose(Loader::Engine *e);
	int onError(int code, bool critical = false);
};

//EOF
