#include "pch.h"
#include "ShapesWnd.h"
#include "resource.h"
#include "ShapeFrustum.h"
#include "ShapeIcosahedron.h"
#include "ShapeSphere.h"
#include "ShapeWave.h"
#include "ShapeAIO.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

ShapesWnd::ShapesWnd()
{
	m_wstrTitle = L"Shapes";
	m_uAppIcon = IDI_ICON_APP;
}

void ShapesWnd::Render()
{
	if (!m_bD3dInitialized) return;
	// clear
	m_spD3d11DevCtx4->ClearRenderTargetView(m_spRTV.Get(), Colors::DarkBlue);
	m_spD3d11DevCtx4->ClearDepthStencilView(m_spDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// tm
	LARGE_INTEGER i64Begin = m_fps.getBegin();
	if (!m_bPaused) QueryPerformanceCounter(&m_i64Now);
	float t = 1.0f * (m_i64Now.QuadPart - i64Begin.QuadPart) / m_fps.getFreq().QuadPart;

	// rotate
	m_space.mWorld = ::XMMatrixIdentity();
	this->UpdateConstBuf(t);
	m_spD3d11DevCtx4->VSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());
	m_spD3d11DevCtx4->PSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());

	m_spD3d11DevCtx4->OMSetRenderTargets(1, m_spRTV.GetAddressOf(), m_spDSV.Get());

	auto spDevCtx = ComPtr<ID3D11DeviceContext>(m_spD3d11DevCtx4);
	if (m_nScene < 4) {
		switch (m_nScene) {
		case 1:
			m_uptrShapeIcosahedron->RenderScene1(spDevCtx);
			break;
		case 2:
			m_uptrShapeIcosahedron->RenderScene2(spDevCtx);
			break;
		case 3:
			m_uptrShapeIcosahedron->RenderScene3(spDevCtx);
			break;
		default:
			m_uptrShapeFrustum->RenderScene(spDevCtx);
		}
	}
	else if(m_nScene < 14) {
		m_uptrShapeSphere->RenderScene(spDevCtx, m_nScene - 4);
	}
	else if (m_nScene < 15) {
		this->UpdateConstBuf(0.0f);
		m_uptrShapeWave->RenderScene(spDevCtx, t);
	}
	else {
		m_uptrShapeAIO->RenderScene(spDevCtx, 0); // ground grid

		m_space.mWorld = ::XMMatrixTranslation(0.0f, 5.0f, 0.0f);
		this->UpdateConstBuf(t);
		m_uptrShapeAIO->RenderScene(spDevCtx, 1); // center box

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 2; j++) {
				m_space.mWorld = ::XMMatrixTranslation(40.0f - 80.0f * j, 0.0f, 60.0f - 30.0f * i);
				this->UpdateConstBuf(t);
				m_uptrShapeAIO->RenderScene(spDevCtx, 2); // Frustum

				m_space.mWorld *= ::XMMatrixTranslation(0.0f, 30.0f, 0.0f);
				this->UpdateConstBuf(t);
				m_uptrShapeAIO->RenderScene(spDevCtx, 3); // Sphere
			}
		}

		m_space.mWorld = ::XMMatrixScaling(3.0f, 3.0f, 3.0f) * ::XMMatrixTranslation(0.0f, 2.4f * 3.0f, -40.0f);
		this->UpdateConstBuf(t);
		m_uptrShapeAIO->RenderScene(spDevCtx, 4); // car

		m_space.mWorld = ::XMMatrixScaling(5.0f, 5.0f, 5.0f) * ::XMMatrixTranslation(0.0f, 10.0f, 0.0f);
		this->UpdateConstBuf(t);
		m_uptrShapeAIO->RenderScene(spDevCtx, 5); // skull
	}
	
	m_spDXGISwapChain4->Present(1, 0);

	// fps
	wchar_t wszBuf[128];
	float fps = m_fps.touch();
	swprintf_s(wszBuf, 128, L"%s - FPS: %7.2f\n", m_wstrTitle.c_str(), fps);
	SetWindowText(m_hwnd, wszBuf);
}

LRESULT ShapesWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONUP, OnLButtonUp);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, OnMouseMove);
		HANDLE_MSG(hWnd, WM_KEYUP, OnKey);
	case c_msg_init:
		return OnInit(wParam, lParam);
	case c_msg_shap_aio_finished:
	{
		ComPtr<ID3D11DeviceContext> spDevCtx;
		m_spD3d11DevCtx4->QueryInterface(IID_PPV_ARGS(&spDevCtx));
		m_uptrShapeAIO->UpdateD3DBuf(spDevCtx);
		return 0L;
	}
	default:
		return D11Wnd::WndProc(hWnd, uMsg, wParam, lParam);
	}
}

BOOL ShapesWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	this->EnableMSAA(true, 4);

	BOOL bOK = D11Wnd::OnCreate(hwnd, lpCreateStruct);
	if (!bOK) return bOK;

	m_bD3dInitialized = false;
	PostMessage(hwnd, c_msg_init, 0, 0);

	return TRUE;
}

void ShapesWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	D11Wnd::OnSize(hwnd, state, cx, cy);
	m_space.mProjection = ::XMMatrixPerspectiveFovLH(XM_PI / 6.0f, 1.0f * cx / cy, 0.01f, 1000.0f);
}

void ShapesWnd::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	m_pointTracking = { x, y };
	SetCapture(hwnd);
}

void ShapesWnd::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	m_fY += m_fRotateY;
	m_fX += m_fRotateX;
	m_fRotateY = 0;
	m_fRotateX = 0;
	ReleaseCapture();
}

void ShapesWnd::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if (keyFlags & MK_LBUTTON) {
		m_fRotateY = XMConvertToRadians(0.25f * (m_pointTracking.x - x));
		m_fRotateX = XMConvertToRadians(0.25f * (m_pointTracking.y - y));
	}
}

void ShapesWnd::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	int scene_max = 5 + m_uptrShapeSphere->GetCount();
	if (vk == VK_SPACE || vk == VK_NEXT) {
		if (m_nScene >= scene_max) m_nScene = 0;
		else m_nScene++;
	}
	else if (vk == VK_PRIOR) {
		if (m_nScene == 0) m_nScene = scene_max;
		else m_nScene--;
	}
	else if (vk == VK_PAUSE) {
		m_bPaused = !m_bPaused;
	}
}

LRESULT ShapesWnd::OnInit(WPARAM wParam, LPARAM lParam)
{
	auto spDev = ComPtr<ID3D11Device>(m_spD3d11Dev5);
	
	HRESULT hr = this->Set3DSpace();

	m_uptrShapeFrustum = make_unique<ShapeFrustum>();
	m_uptrShapeFrustum->CreateD3dRes(spDev);

	m_uptrShapeIcosahedron = make_unique<ShapeIcosahedron>();
	m_uptrShapeIcosahedron->CreateD3dRes(spDev);

	m_uptrShapeSphere = make_unique<ShapeSphere>();
	m_uptrShapeSphere->CreateD3dRes(spDev);

	m_uptrShapeWave = make_unique<ShapeWave>();
	m_uptrShapeWave->CreateD3dRes(spDev);

	m_uptrShapeAIO = make_unique<ShapeAIO>();
	m_uptrShapeAIO->SetNotify([](HWND target) {
		PostMessage(target, c_msg_shap_aio_finished, 0, 0);
	}, m_hwnd);
	m_uptrShapeAIO->CreateD3dRes(spDev);

	m_bD3dInitialized = true;
	
	return 0;
}

HRESULT ShapesWnd::Set3DSpace()
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

void ShapesWnd::UpdateConstBuf(float fDeltaT)
{
	float fAngle = 0.1f * XM_PI * fDeltaT;
	fAngle += (m_fY + m_fRotateY);

	ConstantBuffer constBuf;
	constBuf.mWorld = ::XMMatrixTranspose(m_space.mWorld * ::XMMatrixRotationY(fAngle) * ::XMMatrixRotationX(m_fX + m_fRotateX));
	constBuf.mView = ::XMMatrixTranspose(m_space.mView);
	constBuf.mProjection = ::XMMatrixTranspose(m_space.mProjection);

	m_spD3d11DevCtx4->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
}
