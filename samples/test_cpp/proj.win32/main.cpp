#include "main.h"
#include "AppDelegate.h"
#include "cocos2d.h"

USING_NS_CC;

#if COCOS2D_DEBUG > 0
// uncomment below line, open debug console
#define USE_WIN32_CONSOLE
#endif

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef USE_WIN32_CONSOLE
	AllocConsole();
	SetConsoleCP(CP_ACP);
	SetConsoleOutputCP(CP_ACP);
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

	// create the application instance
    AppDelegate app;
    int nRet = Application::getInstance()->run();

	//app.applicationWillTerminate();
#ifdef USE_WIN32_CONSOLE
	FreeConsole();
#endif
	::Sleep(500);

#if CC_REF_LEAK_DETECTION > 0
	Ref::printLeaks();
#endif

	return nRet;
}
