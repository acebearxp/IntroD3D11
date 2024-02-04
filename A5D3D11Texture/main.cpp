#include "pch.h"
#include "TextureWnd.h"
#pragma warning(disable: 28251)

using namespace std;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// https://docs.microsoft.com/visualstudio/debugger/finding-memory-leaks-using-the-crt-library
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	unique_ptr<D11Wnd> uptrMainWnd = make_unique<TextureWnd>();
	return D3D11App::bootstrap(hInstance, lpCmdLine, nCmdShow, uptrMainWnd);
}
