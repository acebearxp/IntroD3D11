#pragma once
#include "App.h"

class D11Wnd;
class D3D11App : public App
{
public:
    static int bootstrap(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow, std::unique_ptr<D11Wnd>& uptrMainWnd);
public:
    virtual BOOL Init(HINSTANCE hInstance, std::unique_ptr<D11Wnd>& uptrMainWnd);
    int Run(PWSTR pCmdLine, int nCmdShow) override;
protected:
    std::unique_ptr<D11Wnd> m_uptrMainWnd;
};

