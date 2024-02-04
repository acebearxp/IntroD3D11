#include "pch.h"
#include "WaterD3D.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

HRESULT WaterD3D::CreateD3dRes(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	HRESULT hr = __super::CreateD3dRes(spD3d11Dev);

	m_spShaderPS.Reset();
	auto sptrBlobPS = make_shared<vector<std::byte>>();
	*sptrBlobPS = ShaderHelper::LoadCompiledShaderBlob(L"A5PSWater.cso");
	hr = spD3d11Dev->CreatePixelShader(sptrBlobPS->data(), sptrBlobPS->size(), nullptr, &m_spShaderPS);

	return hr;
}

void WaterD3D::createMesh()
{
	const int nWidth = 100, nDepth = 100;
	const float fUVRatio = 2.0f;
	m_uptrMesh = make_unique<GeometryGenerator::MeshData>(
		GeometryGenerator::CreateGrid(100.0f, 100.0f,
			nWidth + 1, nDepth + 1,
			2.5f * fUVRatio / nWidth, 2.5f * fUVRatio / nDepth)
	);

	for (auto& v : m_uptrMesh->vertices) {
		float x = v.pos.x, z = v.pos.z;
		v.pos.y = -4.0f;
		v.color = XMFLOAT4(0.2f, 0.2f, 0.4f, 0.6f);
	}
}

HRESULT WaterD3D::loadTexture(ComPtr<ID3D11Device>& spD3d11Dev)
{
	DDS_HEADER* pHeader = nullptr;
	auto vtBlob = ShaderHelper::LoadDDSTextureFromFile(L"..\\Textures\\water2.dds", &pHeader);

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
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
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

void WaterD3D::Render(ComPtr<ID3D11DeviceContext>& spDevCtx, float fDeltaT)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS.Get());

	float factors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	spDevCtx->OMSetBlendState(m_spBS.Get(), factors, 0xffffffff);

	for (auto& v : m_uptrMesh->vertices) {
		float fY = sinf(128.0f * XM_2PI * v.pos.x / 100.0f + fDeltaT) + sinf(125.0f * XM_2PI * v.pos.z / 100.0f + 1.2f*fDeltaT);
		v.pos.y = -4.0f + 0.2f*fY;

		// normal
		float dx = 1.28f * XM_2PI * cosf(128.0f * XM_2PI * v.pos.x / 100.0f + fDeltaT);
		float dz = 1.25f * XM_2PI * cosf(125.0f * XM_2PI * v.pos.z / 100.0f + 1.2f * fDeltaT);
		XMFLOAT3 vNormal = XMFLOAT3(-0.2f*dx, 1.0f, -0.2f*dz);
		XMVECTOR vN = XMVector3Normalize(XMLoadFloat3(&vNormal));
		XMStoreFloat4(&v.normal, vN);
	}

	D3D11_MAPPED_SUBRESOURCE data;
	HRESULT hr = spDevCtx->Map(m_spVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy_s(data.pData, data.RowPitch, m_uptrMesh->vertices.data(), sizeof(GeometryGenerator::Vertex) * m_uptrMesh->vertices.size());
	spDevCtx->Unmap(m_spVB.Get(), 0);

	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}