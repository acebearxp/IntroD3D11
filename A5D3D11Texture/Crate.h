#pragma once
#include "MeshD3D.h"

class Crate : public MeshD3D
{
public:
	Crate(const wchar_t* pwszTex) { m_wstrTex = pwszTex; };
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
protected:
	void createMesh() override;
	HRESULT loadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev) override;
private:
	std::wstring m_wstrTex;
};

