#pragma once
#include "XWnd.h"
#include "FrameCounter.h"

class D11Wnd : public XWnd
{
public:
    D11Wnd();

    BOOL Create(HINSTANCE hInstance);
    virtual void Render();
protected:
    std::wstring m_wstrTitle;

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
    void OnDestroy(HWND hwnd) override;
    virtual void OnPaint(HWND hwnd);
    virtual BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
    virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);

    virtual bool loadCfg();
    virtual bool saveCfg();
protected:
    bool m_bD3dInitialized = false;
    bool m_bEnableMSAA = false;
    UINT m_uBackBufferCount = 2;
    FrameCounter m_fps;

    DXGI_GPU_PREFERENCE m_gpu_prefer = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    DXGI_SAMPLE_DESC m_msaa_desc = { 1, 0 };
    DXGI_FORMAT m_fmt_back = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_SWAP_EFFECT m_swap_mode = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    Microsoft::WRL::ComPtr<ID3D11Device5> m_spD3d11Dev5;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext4> m_spD3d11DevCtx4;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_spDXGISwapChain4;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_spRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_spDSV;

    Microsoft::WRL::ComPtr<IDXGIFactory7> CreateDXGIFactory();
    Microsoft::WRL::ComPtr<IDXGIAdapter4> EnumDXGIDevices(Microsoft::WRL::ComPtr<IDXGIFactory7>& spDXGIFactory);
    std::tuple<Microsoft::WRL::ComPtr<ID3D11Device5>, Microsoft::WRL::ComPtr<ID3D11DeviceContext4>>
        CreateD3dDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4>& spDXGIAdapter);
    Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(Microsoft::WRL::ComPtr<IDXGIFactory7>& spDXGIFactory);
    HRESULT CheckMSAAQualityLevel(UINT uSampleCount);

    Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_spD2d1RenderTarget;
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_spD2d1Brush;
    Microsoft::WRL::ComPtr<IDWriteTextFormat> m_spDWriteTextFormat;
    HRESULT CreateD2DRenderTarget();

    HRESULT SetSize(int width, int height);
    void RSSetViewport(int width, int height);
    HRESULT CreateRTV(int width, int height);
    HRESULT CreateDSV(int width, int height);

    void EnableMSAA(bool bEnable, UINT uSampleCount);
private:
    const static wchar_t c_wszConfigPattern[];
};

