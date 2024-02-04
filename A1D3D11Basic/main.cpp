#include "pch.h"
#include "D3D11BasicApp.h"

#pragma warning(disable: 28251)

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	D3D11BasicApp app;

	if (!app.Init(hInstance)) return -1;
	int nRet = app.Run(lpCmdLine, nCmdShow);
	app.UnInit();

	return nRet;
}