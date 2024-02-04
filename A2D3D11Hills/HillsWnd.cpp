#include "pch.h"
#include "HillsWnd.h"
#include "resource.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

HillsWnd::HillsWnd()
{
	m_wstrTitle = L"Hills";
	m_uAppIcon = IDI_ICON_APP;
}

void HillsWnd::Render()
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
	m_uptrHills->Render(m_spD3d11DevCtx4);
	m_spDXGISwapChain4->Present(1, 0);

	// fps
	wchar_t wszBuf[128];
	float fps = m_fps.touch();
	swprintf_s(wszBuf, 128, L"%s - FPS: %7.2f\n", m_wstrTitle.c_str(), fps);
	SetWindowText(m_hwnd, wszBuf);
}

LRESULT HillsWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case c_msg_init:
		return OnInit(wParam, lParam);
	default:
		return D11Wnd::WndProc(hWnd, uMsg, wParam, lParam);
	}
}

BOOL HillsWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	this->EnableMSAA(true, 4);

	BOOL bOK = D11Wnd::OnCreate(hwnd, lpCreateStruct);
	if (!bOK) return bOK;

	m_bD3dInitialized = false;
	PostMessage(hwnd, c_msg_init, 0, 0);

	return TRUE;
}

void HillsWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	D11Wnd::OnSize(hwnd, state, cx, cy);
	m_space.mProjection = ::XMMatrixPerspectiveFovLH(XM_PI / 6.0f, 1.0f * cx / cy, 0.01f, 1000.0f);
}

LRESULT HillsWnd::OnInit(WPARAM wParam, LPARAM lParam)
{
	auto spDev = ComPtr<ID3D11Device>(m_spD3d11Dev5);

	m_spRS2 = this->CreateResterizerStage();
	
	HRESULT hr = this->Set3DSpace();

	m_uptrHills = make_unique<Hills>();
	m_uptrHills->CreateD3dRes(spDev);

	m_bD3dInitialized = true;
	
	return 0;
}

ComPtr<ID3D11RasterizerState2> HillsWnd::CreateResterizerStage()
{
	D3D11_RASTERIZER_DESC2 desc{
		.FillMode = D3D11_FILL_WIREFRAME,
		.CullMode = D3D11_CULL_BACK,
		.DepthClipEnable = true,
		.MultisampleEnable = true,
		.AntialiasedLineEnable = true
	};
	ComPtr<ID3D11RasterizerState2> spRS2;
	HRESULT hr = m_spD3d11Dev5->CreateRasterizerState2(&desc, &spRS2);
	return spRS2;
}

HRESULT HillsWnd::Set3DSpace()
{
	D3D11_BUFFER_DESC desc{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER
	};
	HRESULT hr = m_spD3d11Dev5->CreateBuffer(&desc, nullptr, &m_spConstBuf);

	m_space.mWorld = XMMatrixIdentity();

	XMVECTOR xmvEye = XMVectorSet(0.0f, 80.0f, -160.0f, 0.0f);
	XMVECTOR xmvAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR xmvUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_space.mView = XMMatrixLookAtLH(xmvEye, xmvAt, xmvUp);

	return hr;
}

void HillsWnd::UpdateConstBuf()
{
	LARGE_INTEGER i64Now, i64Begin = m_fps.getBegin();
	QueryPerformanceCounter(&i64Now);
	float fAngle = 0.02f * XM_PI * (i64Now.QuadPart - i64Begin.QuadPart) / m_fps.getFreq().QuadPart;

	ConstantBuffer constBuf;
	constBuf.mWorld = ::XMMatrixTranspose(m_space.mWorld * ::XMMatrixRotationY(fAngle));
	constBuf.mView = ::XMMatrixTranspose(m_space.mView);
	constBuf.mProjection = ::XMMatrixTranspose(m_space.mProjection);

	m_spD3d11DevCtx4->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
}