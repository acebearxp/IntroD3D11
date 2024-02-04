#pragma once
#include "MeshD3D.h"
class CrateTxT : public MeshD3D
{
public:
	CrateTxT() { };
	HRESULT CreateD3dRes(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev) override;
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
protected:
	void createMesh() override;
	HRESULT loadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev) override;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_spTex2DAlpha;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_spSRVAlpha;

	void prepareRenderTxT(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
};

