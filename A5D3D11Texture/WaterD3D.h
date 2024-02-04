#pragma once
#include "MeshD3D.h"
class WaterD3D : public MeshD3D
{
public:
	WaterD3D() {};
	HRESULT CreateD3dRes(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev) override;
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx, float fDeltaT);
protected:
	void createMesh() override;
	HRESULT loadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev) override;
};