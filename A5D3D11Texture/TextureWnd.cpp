#include "pch.h"
#include "TextureWnd.h"
#include "resource.h"
#include "GroundD3D.h"
#include "WaterD3D.h"
#include "Crate.h"
#include "CrateMM.h"
#include "CrateTxT.h"
#include "CrateFA.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

TextureWnd::TextureWnd()
{
	m_wstrTitle = L"Texture";
	m_uAppIcon = IDI_ICON_APP;
}

LRESULT TextureWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_MOUSEWHEEL, OnMouseWheel);
		HANDLE_MSG(hWnd, WM_KEYUP, OnKey);
	case c_msg_init:
		return OnInit(wParam, lParam);
	default:
		return __super::WndProc(hWnd, uMsg, wParam, lParam);
	}
}

BOOL TextureWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	this->EnableMSAA(true, 4);

	BOOL bOK = __super::OnCreate(hwnd, lpCreateStruct);
	if (!bOK) return bOK;

	m_i64Now = m_fps.getBegin();
	m_bD3dInitialized = false;
	PostMessage(hwnd, c_msg_init, 0, 0);

	return TRUE;
}

LRESULT TextureWnd::OnInit(WPARAM wParam, LPARAM lParam)
{
	auto spDev = ComPtr<ID3D11Device>(m_spD3d11Dev5);
	HRESULT hr = this->Set3DSpace();
	if (FAILED(hr)) return -1L;

	m_uptrGround = make_unique<GroundD3D>();
	m_uptrGround->CreateD3dRes(spDev);
	m_uptrWater = make_unique<WaterD3D>();
	m_uptrWater->CreateD3dRes(spDev);
	m_uptrCrate1 = make_unique<Crate>(L"WoodCrate01.dds");
	m_uptrCrate1->CreateD3dRes(spDev);
	m_uptrCrate2 = make_unique<Crate>(L"WoodCrate02.dds");
	m_uptrCrate2->CreateD3dRes(spDev);
	m_uptrCrateMM = make_unique<CrateMM>();
	m_uptrCrateMM->CreateD3dRes(spDev);
	m_uptrCrateTxT = make_unique<CrateTxT>();
	m_uptrCrateTxT->CreateD3dRes(spDev);
	m_uptrCrateFA = make_unique<CrateFA>();
	m_uptrCrateFA->CreateD3dRes(spDev);

	m_bD3dInitialized = true;
	return 0L;
}

HRESULT TextureWnd::Set3DSpace()
{
	m_cbFrame.mxWorld = XMMatrixIdentity();

	m_cbFrame.vEyePos = XMVectorSet(0.0f, 80.0f, -160.0f, 0.0f);
	XMVECTOR vAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_cbFrame.mxView = XMMatrixLookAtLH(m_cbFrame.vEyePos, vAt, vUp);

	m_cbFrame.vFog = XMFLOAT4(0.5f, 0.5f, 0.5f, 100.0f);

	// sun light
	m_cbFrame.sunLight.vAmbient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_cbFrame.sunLight.vDiffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_cbFrame.sunLight.vSpecular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_cbFrame.sunLight.vDirection = XMFLOAT4(0.57735f, -0.57735f, 0.57735f, 0.0f);

	D3D11_BUFFER_DESC desc{
		.ByteWidth = sizeof(FrameConstantBuffer),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER
	};

	HRESULT hr = m_spD3d11Dev5->CreateBuffer(&desc, nullptr, &m_spCBFrame);
	return hr;
}

void TextureWnd::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	m_pointTracking = { x, y };
	SetCapture(hwnd);
}

void TextureWnd::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	m_fX += m_fRotateX;
	m_fY += m_fRotateY;
	m_fRotateX = m_fRotateY = 0.0f;
	ReleaseCapture();
}

void TextureWnd::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if (keyFlags & MK_LBUTTON) {
		m_fRotateY = XMConvertToRadians(0.25f * (m_pointTracking.x - x));
		m_fRotateX = XMConvertToRadians(0.25f * (m_pointTracking.y - y));
	}
}

void TextureWnd::OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys)
{
	if(zDelta>0) m_fZoom *= 1.1f;
	else m_fZoom *= 0.9f;

	if(m_fZoom < 0.2f) m_fZoom = 0.2f;
	else if(m_fZoom > 4.0f) m_fZoom = 4.0f;
}

void TextureWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	__super::OnSize(hwnd, state, cx, cy);
	m_cbFrame.mxProjection = XMMatrixPerspectiveFovLH(XM_PI/6.0f, 1.0f * cx / cy, 1.0f, 1000.0f);
}

void TextureWnd::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if (vk == VK_PAUSE) {
		m_bPaused = !m_bPaused;
		if (!m_bPaused) {
			float t = 1.0f * (m_i64Now.QuadPart - m_fps.getBegin().QuadPart) / m_fps.getFreq().QuadPart;
			QueryPerformanceCounter(&m_i64Now);
			LARGE_INTEGER i64Begin;
			i64Begin.QuadPart = m_i64Now.QuadPart - static_cast<LONGLONG>(t * m_fps.getFreq().QuadPart);
			m_fps.setBegin(i64Begin);
		}
	}
	else if (vk == 'A') {
		m_cbFrame.sunLight.vAmbient = m_cbFrame.sunLight.vAmbient.x < 0.5f
			? XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f) : XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	
	}
	else if (vk == 'D') {
		m_cbFrame.sunLight.vDiffuse = m_cbFrame.sunLight.vDiffuse.x < 0.5f
			? XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) : XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (vk == 'F') {
		m_cbFrame.vFog = m_cbFrame.vFog.w > 120.0f
			? XMFLOAT4(0.5f, 0.5f, 0.5f, 100.0f) : XMFLOAT4(0.0f, 0.0f, 0.545f, 9999.0f);
	}
}

void TextureWnd::UpdateCBFrame(float fDeltaT)
{
	FrameConstantBuffer cbFrame;

	float fAngle = 0.0f * XM_PI * fDeltaT;
	fAngle += (m_fY + m_fRotateY);
	cbFrame.mxWorld = XMMatrixTranspose(m_cbFrame.mxWorld * XMMatrixRotationY(fAngle) * XMMatrixRotationX(m_fX + m_fRotateX));

	cbFrame.vEyePos = m_cbFrame.vEyePos * m_fZoom;
	XMVECTOR vAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	cbFrame.mxView = XMMatrixLookAtLH(cbFrame.vEyePos, vAt, vUp);
	cbFrame.mxView = XMMatrixTranspose(cbFrame.mxView);

	cbFrame.mxProjection = XMMatrixTranspose(m_cbFrame.mxProjection);
	cbFrame.mxWVP = cbFrame.mxProjection * cbFrame.mxView * cbFrame.mxWorld;

	cbFrame.mxWorld_InvTrans = XMMatrixInverse(nullptr, cbFrame.mxWorld);
	cbFrame.mxWorld_InvTrans = XMMatrixTranspose(cbFrame.mxWorld_InvTrans);

	cbFrame.vFog = m_cbFrame.vFog;
	cbFrame.sunLight = m_cbFrame.sunLight;
	cbFrame.vTmDelta = XMFLOAT4(0.3f * fDeltaT, 0.0f, 0.0f, 0.0f);

	m_spD3d11DevCtx4->UpdateSubresource(m_spCBFrame.Get(), 0, nullptr, &cbFrame, 0, 0);
}

void TextureWnd::Render()
{
	if(!m_bD3dInitialized) return;

	// tm
	LARGE_INTEGER i64Begin = m_fps.getBegin();
	if (!m_bPaused) QueryPerformanceCounter(&m_i64Now);
	float t = 1.0f * (m_i64Now.QuadPart - i64Begin.QuadPart) / m_fps.getFreq().QuadPart;

	// clear
	XMVECTORF32 vBackColor{ m_cbFrame.vFog.x, m_cbFrame.vFog.y, m_cbFrame.vFog.z, 1.0f };
	m_spD3d11DevCtx4->ClearRenderTargetView(m_spRTV.Get(), vBackColor);
	m_spD3d11DevCtx4->ClearDepthStencilView(m_spDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_spD3d11DevCtx4->OMSetRenderTargets(1, m_spRTV.GetAddressOf(), m_spDSV.Get());
	auto spDevCtx = ComPtr<ID3D11DeviceContext>(m_spD3d11DevCtx4);
	m_spD3d11DevCtx4->VSSetConstantBuffers(0, 1, m_spCBFrame.GetAddressOf());
	m_spD3d11DevCtx4->PSSetConstantBuffers(0, 1, m_spCBFrame.GetAddressOf());

	m_cbFrame.mxWorld = XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(20.0f, -3.0f, -30.0f);
	this->UpdateCBFrame(t);
	m_uptrCrate1->Render(spDevCtx);

	m_cbFrame.mxWorld = XMMatrixScaling(8.0f, 8.0f, 8.0f) * XMMatrixTranslation(15.0f, 5.0f, 0.0f);
	this->UpdateCBFrame(t);
	m_uptrCrate2->Render(spDevCtx);

	m_cbFrame.mxWorld = XMMatrixScaling(10.0f, 10.0f, 10.0f) * XMMatrixTranslation(-10.0f, 2.0f, 0.0f);
	this->UpdateCBFrame(t);
	m_uptrCrateMM->Render(spDevCtx);

	m_cbFrame.mxWorld = XMMatrixScaling(10.0f, 10.0f, 10.0f) * XMMatrixTranslation(-10.0f, 2.0f, 20.0f);
	this->UpdateCBFrame(t);
	m_uptrCrateTxT->Render(spDevCtx);

	m_cbFrame.mxWorld = XMMatrixScaling(10.0f, 10.0f, 10.0f) * XMMatrixTranslation(5.0f, 6.0f, 24.0f);
	this->UpdateCBFrame(t);
	m_uptrCrateFA->Render(spDevCtx);

	m_cbFrame.mxWorld = XMMatrixIdentity();
	this->UpdateCBFrame(t);
	m_uptrWater->Render(spDevCtx, t);
	m_uptrGround->Render(spDevCtx);

	// D2D FPS
	wchar_t buf[128];
	float fps = m_fps.touch();
	swprintf_s(buf, 128, L"纹理 - FPS: %7.2f\n  Tm - %5.3f\n  A - 环境光\n  D - 太阳光\n  F - 雾霾\n  Pause - 动画", fps, t);

	auto targetSize = m_spD2d1RenderTarget->GetSize();
	D2D1_RECT_F rect = { 8.0f, 4.0f, targetSize.width/4.0f, targetSize.height/4.0f };
	m_spD2d1RenderTarget->BeginDraw();
	m_spD2d1RenderTarget->DrawText(buf, (UINT)wcslen(buf), m_spDWriteTextFormat.Get(), &rect, m_spD2d1Brush.Get());
	HRESULT hr = m_spD2d1RenderTarget->EndDraw();

	m_spDXGISwapChain4->Present(1, m_swap_mode);
}