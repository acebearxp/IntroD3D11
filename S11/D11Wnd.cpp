#include "pch.h"
#include "D11Wnd.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

constexpr wchar_t D11Wnd::c_wszConfigPattern[] = L"flags:%u\r\nshowCmd:%u\r\nptMinPosition:%d,%d\r\nptMaxPosition:%d,%d\r\nrcNormalPosition:%d,%d,%d,%d\r\n";

D11Wnd::D11Wnd()
    :m_wstrTitle(L"D3D11 App")
{
    m_wstrClsName.assign(L"D11Wnd");
}

BOOL D11Wnd::Create(HINSTANCE hInstance)
{
    int cxScreen = GetSystemMetrics(SM_CXSCREEN);
    int cyScreen = GetSystemMetrics(SM_CYSCREEN);
    int width = cxScreen > 800 ? 800 : cxScreen;
    int height = cyScreen > 600 ? 600 : cyScreen;

    WNDCLASS cls;
    if (!GetClassInfo(hInstance, m_wstrClsName.c_str(), &cls)) Register(hInstance);

    HWND hwnd = CreateWindow(m_wstrClsName.c_str(), m_wstrTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        (cxScreen - width) / 2, (cyScreen - height) / 2, width, height,
        NULL, NULL, hInstance, this);
    if (!hwnd) {
        MessageBox(NULL, L"Create window failed", L"Error", MB_OK);
        return FALSE;
    }
    else {
        BOOL enable;
        DwmIsCompositionEnabled(&enable);
        if (enable) {
            DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
            DwmSetWindowAttribute(m_hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(DWMNCRENDERINGPOLICY));
        }
    }

    UpdateWindow(m_hwnd);

    return TRUE;
}

void D11Wnd::Render()
{
    if (!m_bD3dInitialized) return;
    // clear
    m_spD3d11DevCtx4->ClearRenderTargetView(m_spRTV.Get(), Colors::DarkBlue);
    m_spD3d11DevCtx4->ClearDepthStencilView(m_spDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    // draw
    m_spD3d11DevCtx4->OMSetRenderTargets(1, m_spRTV.GetAddressOf(), m_spDSV.Get());
    // present
    m_spDXGISwapChain4->Present(1, 0);
    // fps
    wchar_t buf[128];
    float fps = m_fps.touch();
    swprintf_s(buf, 128, L"%s - FPS: %7.2f\n", m_wstrTitle.c_str(), fps);
    SetWindowText(m_hwnd, buf);
}

LRESULT D11Wnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_SIZE, OnSize);
        HANDLE_MSG(hWnd, WM_ERASEBKGND, OnEraseBkgnd);
    default:
        return XWnd::WndProc(hWnd, uMsg, wParam, lParam);
    }

    return 0L;
}

BOOL D11Wnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    BOOL bOK = XWnd::OnCreate(hwnd, lpCreateStruct);
    if (!bOK) return bOK;

    this->loadCfg();

    // 1. Create DXGI Factory
    ComPtr<IDXGIFactory7> spDXGIFactory = this->CreateDXGIFactory();
    if (!spDXGIFactory) {
        MessageBox(hwnd, L"Create DXGI factory failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // 2. Enum for the best DXGI Adapter
    ComPtr<IDXGIAdapter4> spDXGIAdapter = this->EnumDXGIDevices(spDXGIFactory);
    if (!spDXGIAdapter) {
        MessageBox(hwnd, L"Enumerate DXGI adapter failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // 3. Create Direct3D Device
    tie(m_spD3d11Dev5, m_spD3d11DevCtx4) = this->CreateD3dDevice(spDXGIAdapter);
    if (!m_spD3d11Dev5 || !m_spD3d11DevCtx4) {
        MessageBox(hwnd, L"Create Direct3D device failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    HRESULT hr;
    // 4. Check MSAA
    if (m_bEnableMSAA) {
        hr = this->CheckMSAAQualityLevel(m_msaa_desc.Count);
        if (FAILED(hr)) {
            constexpr wchar_t wszWarning[] = L"WARNING: CheckMSAAQualityLevel failed, disable MSAA!";
            OutputDebugString(wszWarning);
            MessageBox(hwnd, wszWarning, L"Error", MB_OK | MB_ICONWARNING);
            this->EnableMSAA(false, 1);
        }
    }

    // 5. Create SwapChain
    m_spDXGISwapChain4 = this->CreateSwapChain(spDXGIFactory);
    if (!m_spDXGISwapChain4) {
        MessageBox(hwnd, L"Create SwapChain failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // 6. Set Size
    RECT rc;
    GetClientRect(hwnd, &rc);
    hr = this->SetSize(rc.right - rc.left, rc.bottom - rc.top);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Set Size failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    hr = this->CreateD2DRenderTarget();
    if (FAILED(hr)) {
		MessageBox(hwnd, L"Create D2D RenderTarget failed!", L"Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

    // 7. Mark initialized
    m_bD3dInitialized = true;

    return bOK;
}

void D11Wnd::OnDestroy(HWND hwnd)
{
    this->saveCfg();
    XWnd::OnDestroy(hwnd);
}

void D11Wnd::OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);

    this->Render();
}

BOOL D11Wnd::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

void D11Wnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    if (cx == 0 || cy == 0 || !m_spDXGISwapChain4) return;
    this->SetSize(cx, cy);
}

bool D11Wnd::loadCfg()
{
    WCHAR wszPath[MAX_PATH];
    int len = GetModuleFileName(0, wszPath, MAX_PATH);
    if (len > 0 && len + 4 < MAX_PATH) {
        wcscat_s(wszPath, L".cfg");
        HANDLE hCfg = CreateFile(wszPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (hCfg != INVALID_HANDLE_VALUE) {
            wchar_t wszBuff[512];
            char szBuff[1024];
            DWORD dwRead;
            BOOL b1 = FALSE;
            if (ReadFile(hCfg, szBuff, sizeof(szBuff), &dwRead, nullptr)) {
                if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, szBuff, dwRead, wszBuff, sizeof(wszBuff) / sizeof(wchar_t)) > 0) {
                    WINDOWPLACEMENT wndPlacement = { sizeof(WINDOWPLACEMENT) };
                    swscanf_s(wszBuff, c_wszConfigPattern,
                        &(wndPlacement.flags),
                        &(wndPlacement.showCmd),
                        &(wndPlacement.ptMinPosition.x), &(wndPlacement.ptMinPosition.y),
                        &(wndPlacement.ptMaxPosition.x), &(wndPlacement.ptMaxPosition.y),
                        &(wndPlacement.rcNormalPosition.left), &(wndPlacement.rcNormalPosition.top),
                        &(wndPlacement.rcNormalPosition.right), &(wndPlacement.rcNormalPosition.bottom));
                    wndPlacement.showCmd = SW_RESTORE;
                    b1 = SetWindowPlacement(m_hwnd, &wndPlacement);
                }
            }
            BOOL b2 = CloseHandle(hCfg);
            return b1 && b2;
        }
    }
    return false;
}

bool D11Wnd::saveCfg()
{
    WINDOWPLACEMENT wndPlacement = { sizeof(WINDOWPLACEMENT) };

    if (GetWindowPlacement(m_hwnd, &wndPlacement)) {
        WCHAR wszPath[MAX_PATH];
        int len = GetModuleFileName(0, wszPath, MAX_PATH);
        if (len > 0 && len + 4 < MAX_PATH) {
            wcscat_s(wszPath, L".cfg");
            HANDLE hCfg = CreateFile(wszPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
            if (hCfg != INVALID_HANDLE_VALUE) {
                wchar_t wszBuff[512];
                char szBuff[1024];
                swprintf_s(wszBuff, c_wszConfigPattern,
                    wndPlacement.flags,
                    wndPlacement.showCmd,
                    wndPlacement.ptMinPosition.x, wndPlacement.ptMinPosition.y,
                    wndPlacement.ptMaxPosition.x, wndPlacement.ptMaxPosition.y,
                    wndPlacement.rcNormalPosition.left, wndPlacement.rcNormalPosition.top,
                    wndPlacement.rcNormalPosition.right, wndPlacement.rcNormalPosition.bottom);
                int nWrite = static_cast<int>(WideCharToMultiByte(CP_UTF8, WC_NO_BEST_FIT_CHARS, wszBuff, (int)wcslen(wszBuff), szBuff, sizeof(szBuff), nullptr, nullptr));
                if (nWrite > 0) {
                    BOOL b1 = WriteFile(hCfg, szBuff, nWrite, nullptr, nullptr);
                    BOOL b2 = CloseHandle(hCfg);
                    return b1 && b2;
                }
            }
        }
    }
    return false;
}

Microsoft::WRL::ComPtr<IDXGIFactory7> D11Wnd::CreateDXGIFactory()
{
    ComPtr<IDXGIFactory7> spDXGIFactory;
    UINT uFlag = 0;
#ifdef _DEBUG
    uFlag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
    HRESULT hr = CreateDXGIFactory2(uFlag, IID_PPV_ARGS(&spDXGIFactory));
    return spDXGIFactory;
}

ComPtr<IDXGIAdapter4> D11Wnd::EnumDXGIDevices(ComPtr<IDXGIFactory7>& spDXGIFactory)
{
    ComPtr<IDXGIAdapter4> spAdapterBest;

    HRESULT hr;
    int nIndex = 0;
    do {
        ComPtr<IDXGIAdapter4> spAdapter;
        hr = spDXGIFactory->EnumAdapterByGpuPreference(nIndex, m_gpu_prefer, IID_PPV_ARGS(&spAdapter));

        if (SUCCEEDED(hr)) {
            if (nIndex == 0) spAdapterBest = spAdapter;

            DXGI_ADAPTER_DESC3 descAdapter;
            HRESULT hrDesc = spAdapter->GetDesc3(&descAdapter);
            if (SUCCEEDED(hrDesc)) {
                wchar_t buf[512];
                if (descAdapter.DedicatedVideoMemory > 1024 * 1024) {
                    int vram_size_mega = static_cast<int>(descAdapter.DedicatedVideoMemory / 1024 / 1024);
                    swprintf_s(buf, L"Found adapter : %s %d) %s (%dM)\n", spAdapterBest == spAdapter ? L"*" : L"\x20", nIndex + 1, descAdapter.Description, vram_size_mega);
                }
                else {
                    swprintf_s(buf, L"Found adapter : %s %d) %s\n", spAdapterBest == spAdapter ? L"*" : L"\x20", nIndex + 1, descAdapter.Description);
                }
                OutputDebugString(buf);
            }
        }
        nIndex++;
    } while (SUCCEEDED(hr));

    return spAdapterBest;
}

tuple<ComPtr<ID3D11Device5>, ComPtr<ID3D11DeviceContext4>> D11Wnd::CreateD3dDevice(ComPtr<IDXGIAdapter4>& spDXGIAdapter)
{
    ComPtr<ID3D11Device5> spD3d11Device5;
    ComPtr<ID3D11DeviceContext4> spD3d11DevCtx4;

    UINT uFlag = 0;
    uFlag |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Direct2D support
#ifdef _DEBUG
    uFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    D3D_FEATURE_LEVEL levels[] = {
        // D3D_FEATURE_LEVEL_12_1,
        // D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    D3D_FEATURE_LEVEL level;
    ComPtr<ID3D11Device> spD3d11Device;
    ComPtr<ID3D11DeviceContext> spD3d11DevCtx;
    HRESULT hr = D3D11CreateDevice(spDXGIAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL,
        uFlag, levels, sizeof(levels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION,
        &spD3d11Device, &level, &spD3d11DevCtx);
    if (SUCCEEDED(hr)) {
        hr = spD3d11Device->QueryInterface(IID_PPV_ARGS(&spD3d11Device5));
        hr = spD3d11DevCtx->QueryInterface(IID_PPV_ARGS(&spD3d11DevCtx4));
    }
    return { spD3d11Device5 , spD3d11DevCtx4 };
}

HRESULT D11Wnd::CheckMSAAQualityLevel(UINT uSampleCount)
{
    UINT uQuality = 1;
    HRESULT hr = m_spD3d11Dev5->CheckMultisampleQualityLevels(this->m_fmt_back, uSampleCount, &uQuality);
    if (SUCCEEDED(hr) && uQuality > 0) {
        this->m_msaa_desc.Count = uSampleCount;
        this->m_msaa_desc.Quality = uQuality - 1;
        return hr;
    }
    else {
        if (FAILED(hr)) return hr;
        else return E_INVALIDARG;
    }
}

ComPtr<IDXGISwapChain4> D11Wnd::CreateSwapChain(ComPtr<IDXGIFactory7>& spDXGIFactory)
{
    ComPtr<IDXGISwapChain1> spDXGISwapChain;

    DXGI_SWAP_CHAIN_DESC1 desc{
        .Format = m_fmt_back,
        .SampleDesc = m_msaa_desc,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = m_uBackBufferCount,
        .SwapEffect = m_swap_mode
    };

    ComPtr<IDXGISwapChain4> spDXGISwapChain4;
    HRESULT hr = spDXGIFactory->CreateSwapChainForHwnd(m_spD3d11Dev5.Get(), m_hwnd, &desc, nullptr, nullptr, &spDXGISwapChain);
    if (SUCCEEDED(hr))
        hr = spDXGISwapChain->QueryInterface(IID_PPV_ARGS(&spDXGISwapChain4));

    return spDXGISwapChain4;
}

HRESULT D11Wnd::SetSize(int width, int height)
{
    this->RSSetViewport(width, height);

    // reset before resize
    m_spD2d1RenderTarget.Reset();
    m_spRTV.Reset();

    HRESULT hr = this->CreateRTV(width, height);
    if (FAILED(hr)) return hr;

    hr = this->CreateDSV(width, height);
    if (FAILED(hr)) return hr;

    hr = this->CreateD2DRenderTarget();
    if (FAILED(hr)) return hr;

    return hr;
}

void D11Wnd::RSSetViewport(int width, int height)
{
    D3D11_VIEWPORT viewPort{
        0.0f, 0.0f,
        static_cast<float>(width), static_cast<float>(height),
        0.0f, 1.0f
    };
    m_spD3d11DevCtx4->RSSetViewports(1, &viewPort);
}

HRESULT D11Wnd::CreateRTV(int width, int height)
{
    // resize back buffers
    HRESULT hr = m_spDXGISwapChain4->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) return hr;

    // reset RTV(render target view) for back buffer
    ComPtr<ID3D11Texture2D> spBackBuf;
    hr = m_spDXGISwapChain4->GetBuffer(0, IID_PPV_ARGS(&spBackBuf));
    if (FAILED(hr)) return hr;

    hr = m_spD3d11Dev5->CreateRenderTargetView(spBackBuf.Get(), nullptr, &m_spRTV);
    return hr;
}

HRESULT D11Wnd::CreateDSV(int width, int height)
{
    D3D11_TEXTURE2D_DESC1 desc{
        .Width = (UINT)width,
        .Height = (UINT)height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc = m_msaa_desc,
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL
    };

    ComPtr<ID3D11Texture2D1> spDepthStencil;
    HRESULT hr = m_spD3d11Dev5->CreateTexture2D1(&desc, nullptr, &spDepthStencil);
    if (FAILED(hr)) return hr;

    hr = m_spD3d11Dev5->CreateDepthStencilView(spDepthStencil.Get(), nullptr, &m_spDSV);
    return hr;
}

void D11Wnd::EnableMSAA(bool bEnable, UINT uSampleCount)
{
    if (bEnable) {
        m_msaa_desc = { uSampleCount, 0 };
        m_uBackBufferCount = 1;
        m_swap_mode = DXGI_SWAP_EFFECT_DISCARD;
    }
    else {
        m_msaa_desc = { 1, 0 };
        m_uBackBufferCount = 2;
        m_swap_mode = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    }

    m_bEnableMSAA = bEnable;
}

HRESULT D11Wnd::CreateD2DRenderTarget()
{
    ComPtr<ID2D1Factory7> spD2dFactory;
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&spD2dFactory));
    if (FAILED(hr)) return hr;

    // D2D
    ComPtr<IDXGISurface> spDXGISurface;
    hr = m_spDXGISwapChain4->GetBuffer(0, IID_PPV_ARGS(&spDXGISurface));
    if (FAILED(hr)) return hr;

    D2D1_RENDER_TARGET_PROPERTIES props{
		.type = D2D1_RENDER_TARGET_TYPE_DEFAULT,
		.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		.dpiX = 0.0f,
		.dpiY = 0.0f,
		.usage = D2D1_RENDER_TARGET_USAGE_NONE,
		.minLevel = D2D1_FEATURE_LEVEL_DEFAULT
    };
    hr = spD2dFactory->CreateDxgiSurfaceRenderTarget(spDXGISurface.Get(), &props, &m_spD2d1RenderTarget);
    if (FAILED(hr)) return hr;

    hr = m_spD2d1RenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow, 0.8f), &m_spD2d1Brush);

    ComPtr<IDWriteFactory7> spDWriteFactory;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), &spDWriteFactory);
    hr = spDWriteFactory->CreateTextFormat(
        L"微软雅黑",
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        18.0f,
        L"zh-Hans",
        &m_spDWriteTextFormat);

    return hr;
}