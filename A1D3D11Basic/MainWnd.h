#pragma once
#include "Cube.h"

struct ConstantBuffer
{
    DirectX::XMMATRIX mWorld;
    DirectX::XMMATRIX mView;
    DirectX::XMMATRIX mProjection;
};

class MainWnd : public D11Wnd
{
public:
    MainWnd();
    void Render() override;
protected:
    BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct) override;
    void OnSize(HWND hwnd, UINT state, int cx, int cy) override;
private:
    Cube m_cube;
    ConstantBuffer m_space;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState2> m_spRS2;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_spConstBuf;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState2> CreateResterizerStage();
    HRESULT Set3DSpace();
    void UpdateConstBuf();
};
