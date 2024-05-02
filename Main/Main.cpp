#define NOMINMAX
#include <Windows.h>
#include "../Engine/Yngine.h"
#include "Demo.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Demo* pDemoProcess = new Demo();
    pDemoProcess->Initialize(hInstance);
    pDemoProcess->Loop();
    pDemoProcess->Finalize();

    return 0; // (int)msg.wParam;
}
