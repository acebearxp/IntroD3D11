#pragma once
#include "Hills.h"

struct ConstantBuffer
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
};

class HillsWnd : public D11Wnd
{
public:
	HillsWnd();
protected:
	void Render() override;
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
	void OnSize(HWND hwnd, UINT state, int cx, int cy) override;
private:
	std::unique_ptr<Hills> m_uptrHills;
	static constexpr UINT c_msg_init = WM_USER + 1;

	ConstantBuffer m_space;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState2> m_spRS2;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spConstBuf;

	LRESULT OnInit(WPARAM wParam, LPARAM lParam);
	Microsoft::WRL::ComPtr<ID3D11RasterizerState2> CreateResterizerStage();
	HRESULT Set3DSpace();
	void UpdateConstBuf();
};

