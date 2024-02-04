#include "pch.h"
#include "GroundD3D.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void GroundD3D::createMesh()
{
	const int nWidth = 100, nDepth = 100;
	const float fUVRatio = 8.0f;
	m_uptrMesh = make_unique<GeometryGenerator::MeshData>(
		GeometryGenerator::CreateGrid(100.0f, 100.0f,
			nWidth + 1, nDepth + 1,
			fUVRatio / nWidth, fUVRatio / nDepth)
	);

	for (auto& v : m_uptrMesh->vertices) {
		float x = v.pos.x, z = v.pos.z;
		v.pos.y = 0.15f * (z * sinf(0.12f * x) + x * cosf(0.1f * z));
		// v.pos.y = max(v.pos.y, -4.1f);
		if(v.pos.y < -4.0f) v.pos.y = -6.0f;
		v.color = XMFLOAT4(0.6f, 0.8f, 0.6f, 1.0f);

		// normal
		float dx = 0.15f * (0.12f * z * cosf(0.12f * x) + cosf(0.1f * z));
		float dz = 0.15f * (sinf(0.12f * x) - 0.1f * x * sinf(0.1f * z));
		XMFLOAT3 vNormal = XMFLOAT3(-dx, 1.0f, -dz);
		XMVECTOR vN = XMVector3Normalize(XMLoadFloat3(&vNormal));
		XMStoreFloat4(&v.normal, vN);
		v.normal.w = 0.0f;
	}
}

HRESULT GroundD3D::loadTexture(ComPtr<ID3D11Device>& spD3d11Dev)
{
	DDS_HEADER* pHeader = nullptr;
	auto vtBlob = ShaderHelper::LoadDDSTextureFromFile(L"..\\Textures\\grass.dds", &pHeader);

	DXGI_FORMAT fmt = ShaderHelper::GetDXGIFormat(pHeader->ddspf);
	D3D11_TEXTURE2D_DESC descTex{
		.Width = pHeader->dwWidth,
		.Height = pHeader->dwHeight,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = fmt,
		.SampleDesc = { 1, 0 },
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};

	D3D11_SUBRESOURCE_DATA initData{
		.pSysMem = vtBlob.data() + 4 + sizeof(DDS_HEADER),
		.SysMemPitch = ((pHeader->dwWidth+3)/4)*ShaderHelper::GetBlockSize(fmt),
	};

	HRESULT hr = spD3d11Dev->CreateTexture2D(&descTex, &initData, &m_spTex2D);
	if (FAILED(hr)) return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV{
		.Format = fmt,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D = { 0, 1 }
	};
	hr = spD3d11Dev->CreateShaderResourceView(m_spTex2D.Get(), &descSRV, &m_spSRV);

	// sampler
	D3D11_SAMPLER_DESC samplerDesc{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		.MipLODBias = 0.0f,
		.MaxAnisotropy = 4,
		.ComparisonFunc = D3D11_COMPARISON_NEVER,
		.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f },
		.MinLOD = 0.0f,
		.MaxLOD = D3D11_FLOAT32_MAX
	};

	hr = spD3d11Dev->CreateSamplerState(&samplerDesc, &m_spSampler);
	return hr;
}

void GroundD3D::Render(ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}