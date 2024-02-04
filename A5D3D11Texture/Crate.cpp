#include "pch.h"
#include "Crate.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void Crate::createMesh()
{
	m_uptrMesh = make_unique<GeometryGenerator::MeshData>(
		GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, 1.0f, 1.0f)
	);

	for (auto& v : m_uptrMesh->vertices) {
		v.color = XMFLOAT4(0.8f, 0.4f, 0.4f, 1.0f);
	}
}

HRESULT Crate::loadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	wchar_t wszPath[1024];
	swprintf_s(wszPath, L"..\\Textures\\%s", m_wstrTex.c_str());

	DDS_HEADER* pHeader = nullptr;
	auto vtBlob = ShaderHelper::LoadDDSTextureFromFile(wszPath, &pHeader);

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
		.SysMemPitch = ((pHeader->dwWidth + 3) / 4) * ShaderHelper::GetBlockSize(fmt),
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
		.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		.MipLODBias = 0.0f,
		.MaxAnisotropy = 8,
		.ComparisonFunc = D3D11_COMPARISON_NEVER,
		.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f },
		.MinLOD = 0.0f,
		.MaxLOD = D3D11_FLOAT32_MAX
	};

	hr = spD3d11Dev->CreateSamplerState(&samplerDesc, &m_spSampler);
	return hr;
}

void Crate::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}