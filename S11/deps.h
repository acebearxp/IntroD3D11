#pragma once

// Windows Header Files
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <wincodec.h>
#include <dwmapi.h>
#include <wrl.h>
#pragma comment(lib, "Dwmapi.lib")

// DirectX
#include <dxgi1_6.h>
#pragma comment(lib, "DXGI.lib")

#include <d3d11_4.h>
#pragma comment(lib, "D3D11.lib")
#include <DirectXMath.h>
#include <DirectXColors.h>

// Direct2D
#include <d2d1_3.h>
#pragma comment(lib, "D2D1.lib")
#include <dwrite_3.h>
#pragma comment(lib, "Dwrite.lib")

// C++
#include <cstddef>
#include <memory>
#include <string>
#include <vector>
