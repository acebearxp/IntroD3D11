#pragma once
#include "Material.h"
class ShapeCube;
class ShapeSphere;


struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mWorldViewProjection;
	DirectX::XMMATRIX mWorld_InvTrans;

	DirectX::XMFLOAT4 v4EyePos;

	DirectionalLight dirLight;
	PointLight pointLight;

	Material material;
};

class LightingWnd : public D11Wnd
{
public:
	LightingWnd();
protected:
	void Render() override;
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
	void OnSize(HWND hwnd, UINT state, int cx, int cy) override;
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
private:
	static constexpr UINT c_msg_init = WM_USER + 1;
	int m_nScene = 0;
	bool m_bWireFrame = false;
	bool m_bPaused = false;
	LARGE_INTEGER m_i64Now;
	float m_fY = 3.14f/4.0f;
	float m_fX = 0.0f;
	float m_fRotateY = 0.0f;
	float m_fRotateX = 0.0f;
	ConstantBuffer m_space;
	// 0: ambient, 1: diffuse, 2: specular
	bool m_bLightADS[3] = { true, true, true };
	// 0: DirectionalLight, 1: PointLight, 2: SpotLight
	bool m_bLight[3] = { true, true, false };

	std::unique_ptr<ShapeCube> m_uptrShapeCube;
	std::unique_ptr<ShapeSphere> m_uptrShapeSphere;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spConstBuf;

	LRESULT OnInit(WPARAM wParam, LPARAM lParam);
	HRESULT Set3DSpace();
	void UpdateConstBuf(float fDeltaT, float fOffsetX);
};
