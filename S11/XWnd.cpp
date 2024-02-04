#include "pch.h"
#include "XWnd.h"

using namespace std;

ATOM XWnd::Register(HINSTANCE hInstance)
{
    WNDCLASS wcls;
    ZeroMemory(&wcls, sizeof(WNDCLASS));
    wcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcls.lpfnWndProc = XWnd::WindowProc;
    wcls.hInstance = hInstance;
    wcls.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcls.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcls.lpszClassName = m_wstrClsName.c_str();
    if (m_uAppIcon)
        wcls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(m_uAppIcon));
    else
        wcls.hIcon = LoadIcon(hInstance, IDI_APPLICATION);

    return RegisterClass(&wcls);
}

BOOL XWnd::Create(HINSTANCE hInstance, wstring &wstrTitle, const RECT &rect, const XWnd &xwndParent, DWORD dwStyle)
{
    WNDCLASS cls;
    if (!GetClassInfo(hInstance, m_wstrClsName.c_str(), &cls)) Register(hInstance);

    HWND hWnd = CreateWindow(m_wstrClsName.c_str(), wstrTitle.c_str(),
        dwStyle | WS_CHILD | WS_VISIBLE | WS_BORDER,
        rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
        xwndParent, NULL, hInstance, this);
    if (!hWnd) {
        MessageBox(xwndParent, L"Create window failed!", L"Error", MB_OK);
        return FALSE;
    }
    return TRUE;
}

void XWnd::Destroy()
{
    DestroyWindow(m_hwnd);
}

LRESULT XWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0L;
}

LRESULT XWnd::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    XWnd *pXWnd = nullptr;
    if (uMsg == WM_CREATE) {
        LPVOID lpCreateParams = reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams;
        pXWnd = reinterpret_cast<XWnd*>(lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pXWnd));
    }
    else {
        pXWnd = reinterpret_cast<XWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (!pXWnd) return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return pXWnd->WndProc(hWnd, uMsg, wParam, lParam);
}

BOOL XWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    m_hwnd = hwnd;

    LOGFONT lf;
    ZeroMemory(&lf, sizeof(LOGFONT));
    lf.lfHeight = 12;
    lf.lfWeight = FW_MEDIUM;
    wcscpy_s(lf.lfFaceName, 32, L"����");
    m_hfontSong = CreateFontIndirect(&lf);

    return TRUE;
}

void XWnd::OnDestroy(HWND hwnd)
{
    DeleteFont(m_hfontSong);
    PostQuitMessage(0);
}

void XWnd::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
}