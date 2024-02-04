#include "pch.h"
#include "CrateTxT.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

HRESULT CrateTxT::CreateD3dRes(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	HRESULT hr = __super::CreateD3dRes(spD3d11Dev);

	m_spShaderPS.Reset();
	auto sptrBlobPS = make_shared<vector<std::byte>>();
	*sptrBlobPS = ShaderHelper::LoadCompiledShaderBlob(L"A5PSTxT.cso");
	hr = spD3d11Dev->CreatePixelShader(sptrBlobPS->data(), sptrBlobPS->size(), nullptr, &m_spShaderPS);

	return hr;
}

void CrateTxT::createMesh()
{
	m_uptrMesh = make_unique<GeometryGenerator::MeshData>(
		GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, 1.0f, 1.0f)
	);

	for (auto& v : m_uptrMesh->vertices) {
		v.color = XMFLOAT4(0.8f, 0.4f, 0.4f, 1.0f);
	}
}

HRESULT CrateTxT::loadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	wchar_t wszPath[1024];
	swprintf_s(wszPath, L"..\\Textures\\%s", L"flare.dds");

	DDS_HEADER* pHeader = nullptr;
	const auto vtBlob = ShaderHelper::LoadDDSTextureFromFile(wszPath, &pHeader);

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

	// The 2nd texture
	swprintf_s(wszPath, L"..\\Textures\\%s", L"flarealpha.dds");
	const auto vtBlob2 = ShaderHelper::LoadDDSTextureFromFile(wszPath, &pHeader);
	fmt = ShaderHelper::GetDXGIFormat(pHeader->ddspf);

	descTex.Width = pHeader->dwWidth;
	descTex.Height = pHeader->dwHeight;
	descTex.Format = fmt;

	initData.pSysMem = vtBlob2.data() + 4 + sizeof(DDS_HEADER);
	initData.SysMemPitch = ((pHeader->dwWidth + 3) / 4) * ShaderHelper::GetBlockSize(fmt);

	hr = spD3d11Dev->CreateTexture2D(&descTex, &initData, &m_spTex2DAlpha);
	if (FAILED(hr)) return hr;

	descSRV.Format = fmt;
	hr = spD3d11Dev->CreateShaderResourceView(m_spTex2DAlpha.Get(), &descSRV, &m_spSRVAlpha);

	// sampler
	D3D11_SAMPLER_DESC samplerDesc{
		.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
		.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
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

void CrateTxT::prepareRenderTxT(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	spDevCtx->IASetInputLayout(m_spInputLayout.Get());
	ID3D11Buffer* buf[] = { m_spVB.Get() };
	UINT stride[] = { (UINT)this->getVBStride() };
	UINT offset[] = { 0 };
	spDevCtx->IASetVertexBuffers(0, 1, buf, stride, offset);
	spDevCtx->IASetIndexBuffer(m_spIB.Get(), this->getIBFmt(), 0);
	spDevCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// shaders
	spDevCtx->VSSetShader(m_spShaderVS.Get(), nullptr, 0);
	spDevCtx->PSSetShader(m_spShaderPS.Get(), nullptr, 0);

	ID3D11ShaderResourceView* srv[] = { m_spSRV.Get(), m_spSRVAlpha.Get() };
	spDevCtx->PSSetShaderResources(0, 2, srv);
	spDevCtx->PSSetSamplers(0, 1, m_spSampler.GetAddressOf());
}

void CrateTxT::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRenderTxT(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}