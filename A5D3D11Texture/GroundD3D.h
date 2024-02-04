#pragma once
#include "MeshD3D.h"

class GroundD3D : public MeshD3D
{
public:
	GroundD3D() {};
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
protected:
	void createMesh() override;
	HRESULT loadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev) override;
};

