#include "pch.h"
#include "CrateFA.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

HRESULT CrateFA::CreateD3dRes(ComPtr<ID3D11Device>& spD3d11Dev)
{
	HRESULT hr = __super::CreateD3dRes(spD3d11Dev);

	m_spShaderPS.Reset();
	auto sptrBlobPS = make_shared<vector<std::byte>>();
	*sptrBlobPS = ShaderHelper::LoadCompiledShaderBlob(L"A5PSFireAnim.cso");
	hr = spD3d11Dev->CreatePixelShader(sptrBlobPS->data(), sptrBlobPS->size(), nullptr, &m_spShaderPS);

	return hr;
}

void CrateFA::createMesh()
{
	m_uptrMesh = make_unique<GeometryGenerator::MeshData>(
		GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, 1.0f, 1.0f)
	);

	for (auto& v : m_uptrMesh->vertices) {
		v.color = XMFLOAT4(0.8f, 0.4f, 0.4f, 1.0f);
	}
}

HRESULT CrateFA::loadTexture(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	wchar_t wszPath[1024];
	swprintf_s(wszPath, L"..\\Textures\\FireAnim\\%s", L"Fire001.bmp");

	UINT uWidth, uHeight;
	const auto vtBlob = ShaderHelper::LoadImageTextureFromFile(wszPath, &uWidth, &uHeight);
	UINT uPitch = ((uWidth + 3) / 4) * ShaderHelper::GetBlockSize(DXGI_FORMAT_R8G8B8A8_UNORM);

	// 120 frames animation
	constexpr UINT uFrameCount = 120;
	vector<std::byte> vtBlob120(vtBlob.size() * uFrameCount);
	vector< D3D11_SUBRESOURCE_DATA> vtInitData(uFrameCount);
	auto pDst = vtBlob120.data();
	for (UINT i = 1; i <= uFrameCount; i++) {
		swprintf_s(wszPath, L"..\\Textures\\FireAnim\\Fire%03d.bmp", i);
		const auto vtBlobFrame = ShaderHelper::LoadImageTextureFromFile(wszPath, &uWidth, &uHeight);
		memcpy(pDst, vtBlobFrame.data(), vtBlobFrame.size());

		vtInitData[i - 1].pSysMem = pDst;
		vtInitData[i-1].SysMemPitch = uPitch;

		pDst += vtBlobFrame.size();
	}

	D3D11_TEXTURE2D_DESC descTex{
		.Width = uWidth,
		.Height = uHeight,
		.MipLevels = 1,
		.ArraySize = uFrameCount,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc = { 1, 0 },
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		.MiscFlags = 0
	};

	HRESULT hr = spD3d11Dev->CreateTexture2D(&descTex, vtInitData.data(), &m_spTex2D);
	if (FAILED(hr)) return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV{
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY,
		.Texture2DArray = {
			.MostDetailedMip = 0,
			.MipLevels = 1,
			.FirstArraySlice = 0,
			.ArraySize = uFrameCount
		}
	};
	hr = spD3d11Dev->CreateShaderResourceView(m_spTex2D.Get(), &descSRV, &m_spSRV);

	// sampler
	D3D11_SAMPLER_DESC samplerDesc{
		.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		.AddressU = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressV = D3D11_TEXTURE_ADDRESS_BORDER,
		.AddressW = D3D11_TEXTURE_ADDRESS_BORDER,
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

void CrateFA::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}