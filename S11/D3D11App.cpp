#include "pch.h"
#include "D3D11App.h"
#include "D11Wnd.h"

using namespace std;

int D3D11App::bootstrap(HINSTANCE hInstance, PWSTR pCmdLine, int nCmdShow, unique_ptr<D11Wnd>& uptrMainWnd)
{
	D3D11App app;

	if (!app.Init(hInstance, uptrMainWnd)) return -1;
	int nRet = app.Run(pCmdLine, nCmdShow);
	app.UnInit();

	return nRet;
}

BOOL D3D11App::Init(HINSTANCE hInstance, unique_ptr<D11Wnd>& uptrMainWnd)
{
	BOOL bOK = App::Init(hInstance);
	if (!bOK) return bOK;

	if (uptrMainWnd) swap(m_uptrMainWnd, uptrMainWnd);
	else m_uptrMainWnd = make_unique<D11Wnd>();

	return m_uptrMainWnd->Create(hInstance);
}

int D3D11App::Run(PWSTR pCmdLine, int nCmdShow)
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			m_uptrMainWnd->Render();
		}
	}

	return static_cast<int>(msg.wParam);
}
