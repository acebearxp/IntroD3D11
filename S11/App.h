#pragma once

class App
{
public:
    App();
    virtual ~App();

    HINSTANCE GetInstance() { return m_hInstance; }

    virtual BOOL Init(HINSTANCE hInstance);
    virtual int Run(PWSTR pCmdLine, int nCmdShow);
    virtual void UnInit();
protected:
    HINSTANCE m_hInstance = NULL;
};

