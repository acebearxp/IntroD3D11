#pragma once
class GroundD3D;
class WaterD3D;
class Crate;
class CrateMM;
class CrateTxT;
class CrateFA;

struct SunLight
{
	DirectX::XMFLOAT4 vAmbient;
	DirectX::XMFLOAT4 vDiffuse;
	DirectX::XMFLOAT4 vSpecular;
	DirectX::XMFLOAT4 vDirection;
};

struct FrameConstantBuffer
{
	DirectX::XMMATRIX mxWorld;
	DirectX::XMMATRIX mxView;
	DirectX::XMMATRIX mxProjection;
	DirectX::XMMATRIX mxWVP;
	DirectX::XMMATRIX mxWorld_InvTrans;

	DirectX::XMVECTOR vEyePos;
	DirectX::XMFLOAT4 vFog;

	SunLight sunLight;

	DirectX::XMFLOAT4 vTmDelta;
};

class TextureWnd : public D11Wnd
{
public:
	TextureWnd();
protected:
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
	void OnSize(HWND hwnd, UINT state, int cx, int cy) override;
	virtual void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnMouseWheel(HWND hwnd, int xPos, int yPos, int zDelta, UINT fwKeys);
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void Render() override;
private:
	static constexpr UINT c_msg_init = WM_USER + 1;
	bool m_bPaused = false;
	LARGE_INTEGER m_i64Now;

	float m_fX = 0.0f;
	float m_fY = 0.0f;
	float m_fRotateX = 0.0f;
	float m_fRotateY = 0.0f;
	POINT m_pointTracking = { 0, 0 };
	float m_fZoom = 1.0f;

	FrameConstantBuffer m_cbFrame;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spCBFrame;
	std::unique_ptr<GroundD3D> m_uptrGround;
	std::unique_ptr<WaterD3D> m_uptrWater;
	std::unique_ptr<Crate> m_uptrCrate1;
	std::unique_ptr<Crate> m_uptrCrate2;
	std::unique_ptr<CrateMM> m_uptrCrateMM;
	std::unique_ptr<CrateTxT> m_uptrCrateTxT;
	std::unique_ptr<CrateFA> m_uptrCrateFA;

	LRESULT OnInit(WPARAM wParam, LPARAM lParam);
	HRESULT Set3DSpace();
	void UpdateCBFrame(float fDeltaT);
};

