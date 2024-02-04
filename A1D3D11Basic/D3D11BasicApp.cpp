#include "pch.h"
#include "D3D11BasicApp.h"

using namespace std;

D3D11BasicApp::D3D11BasicApp()
{
}


D3D11BasicApp::~D3D11BasicApp()
{
}

BOOL D3D11BasicApp::Init(HINSTANCE hInstance)
{
    BOOL bOK = App::Init(hInstance);
    if (!bOK) return bOK;
    
    m_uptrMainWnd = make_unique<MainWnd>();
    return m_uptrMainWnd->Create(hInstance);
}

void D3D11BasicApp::UnInit()
{
    D3D11App::UnInit();
}

int D3D11BasicApp::Run(PWSTR pCmdLine, int nCmdShow)
{
    return D3D11App::Run(pCmdLine, nCmdShow);
}
