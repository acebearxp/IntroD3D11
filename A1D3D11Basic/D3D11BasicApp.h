#pragma once
#include "App.h"
#include "MainWnd.h"

class D3D11BasicApp : public D3D11App
{
public:
    D3D11BasicApp();
    virtual ~D3D11BasicApp();

    BOOL Init(HINSTANCE hInstance) override;
    void UnInit() override;
    
    int Run(PWSTR pCmdLine, int nCmdShow) override;
};