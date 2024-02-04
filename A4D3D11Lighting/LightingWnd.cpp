#include "pch.h"
#include "LightingWnd.h"
#include "resource.h"
#include "ShapeCube.h"
#include "ShapeSphere.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

LightingWnd::LightingWnd()
{
	m_wstrTitle = L"Lighting";
	m_uAppIcon = IDI_ICON_APP;
}

void LightingWnd::Render()
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
	m_space.mWorld = ::XMMatrixIdentity() * XMMatrixScaling(10.0f, 10.0f, 10.0f); // scale
	this->UpdateConstBuf(t, -50.0f);

	m_spD3d11DevCtx4->VSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());
	m_spD3d11DevCtx4->PSSetConstantBuffers(0, 1, m_spConstBuf.GetAddressOf());
	m_spD3d11DevCtx4->OMSetRenderTargets(1, m_spRTV.GetAddressOf(), m_spDSV.Get());

	auto spDevCtx = ComPtr<ID3D11DeviceContext>(m_spD3d11DevCtx4);
	
	m_uptrShapeCube->RenderScene(spDevCtx, m_bWireFrame);

	// sphere
	m_space.mWorld = ::XMMatrixIdentity();
	this->UpdateConstBuf(t, 15.0f);
	m_uptrShapeSphere->RenderScene(spDevCtx, m_nScene, m_bWireFrame); // 0~5 6~9

	m_spDXGISwapChain4->Present(1, 0);

	// fps
	wchar_t wszBuf[128];
	float fps = m_fps.touch();
	swprintf_s(wszBuf, 128, L"%s - FPS: %7.2f\n", m_wstrTitle.c_str(), fps);
	SetWindowText(m_hwnd, wszBuf);
}

BOOL LightingWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	this->EnableMSAA(true, 4);

	BOOL bOK = __super::OnCreate(hwnd, lpCreateStruct);
	if (!bOK) return bOK;

	m_bD3dInitialized = false;
	PostMessage(hwnd, c_msg_init, 0, 0);

	return TRUE;
}

LRESULT LightingWnd::OnInit(WPARAM wParam, LPARAM lParam)
{
	auto spDev = ComPtr<ID3D11Device>(m_spD3d11Dev5);

	HRESULT hr = this->Set3DSpace();

	m_uptrShapeCube = make_unique<ShapeCube>();
	m_uptrShapeCube->CreateD3dRes(spDev);
	
	m_uptrShapeSphere = make_unique<ShapeSphere>();
	m_uptrShapeSphere->CreateD3dRes(spDev);
	m_nScene = m_uptrShapeSphere->GetCount() - 1;

	m_bD3dInitialized = true;

	return 0;
}

HRESULT LightingWnd::Set3DSpace()
{
	D3D11_BUFFER_DESC desc{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER
	};
	HRESULT hr = m_spD3d11Dev5->CreateBuffer(&desc, nullptr, &m_spConstBuf);

	m_space.mWorld = XMMatrixIdentity();

	m_space.v4EyePos = XMFLOAT4(0.0f, 80.0f, -160.0f, 0.0f);
	XMVECTOR xmvEye = XMLoadFloat4(&m_space.v4EyePos);
	XMVECTOR xmvAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR xmvUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_space.mView = XMMatrixLookAtLH(xmvEye, xmvAt, xmvUp);

	// light
	m_space.dirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_space.dirLight.Diffuse = XMFLOAT4(0.3f, 0.8f, 0.3f, 1.0f);
	m_space.dirLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_space.dirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	// point light
	m_space.pointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_space.pointLight.Diffuse = XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f);
	m_space.pointLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	m_space.pointLight.Position = XMFLOAT3(-35.0f, 25.0f, -40.0f);
	m_space.pointLight.Range = 100.0f;
	m_space.pointLight.Attenuation = XMFLOAT4(0.0f, 0.001f, 0.0002f, 0.0f); // (D+S)/(x0+x1*d+x2*d^2)

	// material
	m_space.material.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_space.material.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_space.material.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	return hr;
}

void LightingWnd::UpdateConstBuf(float fDeltaT, float fOffsetX)
{
	float fAngle = 0.1f * XM_PI * fDeltaT;
	fAngle += (m_fY + m_fRotateY);

	ConstantBuffer constBuf;
	// C++使用的是行优先矩阵，而Shader使用的是列优先矩阵，所以需要转置
	constBuf.mWorld = ::XMMatrixTranspose(m_space.mWorld
		* ::XMMatrixRotationY(fAngle)
		* ::XMMatrixRotationX(m_fX + m_fRotateX)
		* ::XMMatrixTranslation(fOffsetX, 0.0f, 0.0f));
	constBuf.mView = ::XMMatrixTranspose(m_space.mView);
	constBuf.mProjection = ::XMMatrixTranspose(m_space.mProjection);

	constBuf.mWorldViewProjection = constBuf.mProjection * constBuf.mView * constBuf.mWorld;
	// 法线使用的是逆转置矩阵，因为变换如果有缩放，法线的缩放是反向的
	constBuf.mWorld_InvTrans = XMMatrixInverse(nullptr, constBuf.mWorld);
	constBuf.mWorld_InvTrans = XMMatrixTranspose(constBuf.mWorld_InvTrans);

	constBuf.v4EyePos = m_space.v4EyePos;
	constBuf.dirLight = m_space.dirLight;

	constBuf.pointLight = m_space.pointLight;
	auto pos = XMLoadFloat3(&constBuf.pointLight.Position);
	pos = XMVector3Transform(pos, XMMatrixRotationX(fAngle*2.0f));
	XMStoreFloat3(&constBuf.pointLight.Position, pos);
	
	constBuf.material = m_space.material;

	if (!m_bLight[0]) {
		constBuf.dirLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		constBuf.dirLight.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		constBuf.dirLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	if (!m_bLight[1]) {
		constBuf.pointLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		constBuf.pointLight.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		constBuf.pointLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	if (!m_bLightADS[0]) {
		constBuf.dirLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		constBuf.pointLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	if (!m_bLightADS[1]) {
		constBuf.dirLight.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		constBuf.pointLight.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	if (!m_bLightADS[2]) {
		constBuf.dirLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		constBuf.pointLight.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	m_spD3d11DevCtx4->UpdateSubresource(m_spConstBuf.Get(), 0, nullptr, &constBuf, 0, 0);
}


LRESULT LightingWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		HANDLE_MSG(hWnd, WM_KEYUP, OnKey);
	case c_msg_init:
		return OnInit(wParam, lParam);
	default:
		return D11Wnd::WndProc(hWnd, uMsg, wParam, lParam);
	}
}

void LightingWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	__super::OnSize(hwnd, state, cx, cy);
	m_space.mProjection = ::XMMatrixPerspectiveFovLH(XM_PI / 6.0f, 1.0f * cx / cy, 0.01f, 1000.0f);
}

void LightingWnd::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	int scene_max = m_uptrShapeSphere->GetCount()-1;
	if (vk == VK_SPACE || vk == VK_NEXT) {
		if (m_nScene >= scene_max) m_nScene = 0;
		else m_nScene++;
	}
	else if (vk == VK_PRIOR) {
		if (m_nScene == 0) m_nScene = scene_max;
		else m_nScene--;
	}
	else if (vk == 'W') {
		m_bWireFrame = !m_bWireFrame;
	}
	else if (vk == 'A') {
		m_bLightADS[0] = !m_bLightADS[0];
	}
	else if (vk == 'D') {
		m_bLightADS[1] = !m_bLightADS[1];
	}
	else if (vk == 'S') {
		m_bLightADS[2] = !m_bLightADS[2];
	}
	else if (vk == VK_PAUSE) {
		m_bPaused = !m_bPaused;
	}
	else if (vk > '0' && vk <= '3') {
		int sn = vk - '0' - 1;
		m_bLight[sn] = !m_bLight[sn];
	}
}