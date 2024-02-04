#include "pch.h"
#include "HillsWnd.h"

using namespace std;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	unique_ptr<D11Wnd> uptrWnd = make_unique<HillsWnd>();
	return D3D11App::bootstrap(hInstance, lpCmdLine, nCmdShow, uptrWnd);
}