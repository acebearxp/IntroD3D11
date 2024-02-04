#include "pch.h"
#include "MainWnd.h"
#include "resource.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

MainWnd::MainWnd()
{
    m_wstrClsName.assign(L"MainWnd");
    m_wstrTitle.assign(L"D3D11 Basic App");
    m_uAppIcon = IDI_ICON_APP;
}

void MainWnd::Render()
{
    if (!m_bD3dInitialized) return;
    // clear
    m_spD3d11DevCtx4->ClearRenderTargetView(m_spRTV.Get(), Colors::DarkBlue);
    m_spD3d11DevCtx4->ClearDepthStencilView(m_spDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // rotate
    this->UpdateConstBuf();
    m_spD3d11DevCtx4->VSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());
    m_spD3d11DevCtx4->PSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());
    // RS State
    m_spD3d11DevCtx4->RSSetState(m_spRS2.Get());

    m_spD3d11DevCtx4->OMSetRenderTargets(1, m_spRTV.GetAddressOf(), m_spDSV.Get());
    m_cube.Render(m_spD3d11DevCtx4);
    m_spDXGISwapChain4->Present(1, 0);

    // fps
    wchar_t wszBuf[128];
    float fps = m_fps.touch();
    swprintf_s(wszBuf, 128, L"%s - FPS: %7.2f\n", m_wstrTitle.c_str(), fps);
    SetWindowText(m_hwnd, wszBuf);
}

BOOL MainWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    // this->m_gpu_prefer = DXGI_GPU_PREFERENCE_MINIMUM_POWER;
    this->EnableMSAA(false, 1);

    BOOL bOK = D11Wnd::OnCreate(hwnd, lpCreateStruct);
    if (!bOK) return bOK;

    // 1. Create Rasterizer Stage
    m_spRS2 = this->CreateResterizerStage();
    if (!m_spRS2) {
        MessageBox(hwnd, L"Create resterizer stage failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // 3. 3D Space
    HRESULT hr = this->Set3DSpace();
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Set 3d space failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // 4. Create Geometry
    auto spDev = ComPtr<ID3D11Device>(m_spD3d11Dev5);
    hr = m_cube.CreateD3dRes(spDev);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Create geometry failed!", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return bOK;
}

void MainWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    D11Wnd::OnSize(hwnd, state, cx, cy);
    m_space.mProjection = ::XMMatrixPerspectiveFovLH(XM_PI / 6.0f, 1.0f * cx / cy, 0.01f, 100.0f);
}

ComPtr<ID3D11RasterizerState2> MainWnd::CreateResterizerStage()
{
    D3D11_RASTERIZER_DESC2 desc{
        .FillMode = D3D11_FILL_WIREFRAME,
        .CullMode = D3D11_CULL_NONE,
        .DepthClipEnable = true,
        .MultisampleEnable = true,
        .AntialiasedLineEnable = true
    };
    ComPtr<ID3D11RasterizerState2> spRS2;
    HRESULT hr = m_spD3d11Dev5->CreateRasterizerState2(&desc, &spRS2);
    return spRS2;
}

HRESULT MainWnd::Set3DSpace()
{
    D3D11_BUFFER_DESC desc{
        .ByteWidth = sizeof(ConstantBuffer),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER
    };
    HRESULT hr = m_spD3d11Dev5->CreateBuffer(&desc, nullptr, &m_spConstBuf);

    m_space.mWorld = XMMatrixIdentity();

    XMVECTOR xmvEye = XMVectorSet(3.0f, 6.0f, -8.0f, 0.0f);
    XMVECTOR xmvAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR xmvUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_space.mView = XMMatrixLookAtLH(xmvEye, xmvAt, xmvUp);

    return hr;
}

void MainWnd::UpdateConstBuf()
{
    LARGE_INTEGER i64Now, i64Begin = m_fps.getBegin();
    QueryPerformanceCounter(&i64Now);
    float fAngle = 0.2f * XM_PI * (i64Now.QuadPart - i64Begin.QuadPart) / m_fps.getFreq().QuadPart;

    ConstantBuffer constBuf;
    constBuf.mWorld = ::XMMatrixTranspose(m_space.mWorld * ::XMMatrixRotationY(fAngle));
    constBuf.mView = ::XMMatrixTranspose(m_space.mView);
    constBuf.mProjection = ::XMMatrixTranspose(m_space.mProjection);

    m_spD3d11DevCtx4->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
}
