#include "pch.h"
#include "App.h"

App::App()
{
}


App::~App()
{
}

BOOL App::Init(HINSTANCE hInstance)
{
    m_hInstance = hInstance;
    return TRUE;
}

void App::UnInit()
{
}

int App::Run(PWSTR pCmdLine, int nCmdShow)
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}
