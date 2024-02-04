#pragma once
class ShapeFrustum;
class ShapeIcosahedron;
class ShapeSphere;
class ShapeWave;
class ShapeAIO;

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

class ShapesWnd : public D11Wnd
{
public:
	ShapesWnd();
protected:
	void Render() override;
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
	void OnSize(HWND hwnd, UINT state, int cx, int cy) override;
	virtual void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
private:
	int m_nScene = 0;
	bool m_bPaused = false;
	LARGE_INTEGER m_i64Now;

	std::unique_ptr<ShapeIcosahedron> m_uptrShapeIcosahedron;
	std::unique_ptr<ShapeFrustum> m_uptrShapeFrustum;
	std::unique_ptr<ShapeSphere> m_uptrShapeSphere;
	std::unique_ptr<ShapeWave> m_uptrShapeWave;
	std::unique_ptr<ShapeAIO> m_uptrShapeAIO;

	static constexpr UINT c_msg_init = WM_USER + 1;
	static constexpr UINT c_msg_shap_aio_finished = WM_USER + 2;
	POINT m_pointTracking;
	float m_fY = 0.8f;
	float m_fX = 0.0f;
	float m_fRotateY = 0.0f;
	float m_fRotateX = 0.0f;

	ConstantBuffer m_space;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spConstBuf;

	LRESULT OnInit(WPARAM wParam, LPARAM lParam);
	HRESULT Set3DSpace();
	void UpdateConstBuf(float fDeltaT);
};

