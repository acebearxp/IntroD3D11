#pragma once
#pragma warning(disable: 26495)

class XWnd
{
public:
    XWnd() :m_wstrClsName(L"XWnd"){}
    virtual ~XWnd() {}

    operator HWND()const { return m_hwnd; }

    BOOL Create(HINSTANCE hInstance, std::wstring &wstrTitle, const RECT &rect, const XWnd &xwndParent, DWORD dwStyle);
    void Destroy();
protected:
    std::wstring m_wstrClsName;
    UINT m_uAppIcon = 0;
    HFONT m_hfontSong;
    HWND m_hwnd;

    virtual ATOM Register(HINSTANCE hInstance);

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
    virtual void OnDestroy(HWND hwnd);
    virtual void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};

