#include <ios>
#include <tchar.h>
#include <thread>

#include "Graphics.h"
#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    MainWindow window;

    auto r = window.Create(
        WS_OVERLAPPEDWINDOW,
        "Aqua Engine Sample",
        200,
        200,
        1920,
        1080
    );
    if (r != 0)
    {
        return 0;
    }

    ShowWindow(window.GetWindow(), nCmdShow);
    window.SetUp();

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
