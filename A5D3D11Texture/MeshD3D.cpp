#include "pch.h"
#include "MeshD3D.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

HRESULT MeshD3D::CreateD3dRes(ComPtr<ID3D11Device>& spD3d11Dev)
{
	HRESULT hr;
    this->createMesh();
	hr = this->loadTexture(spD3d11Dev);
	if (FAILED(hr)) return hr;
    
    // load shaders
    auto sptrBlobVS = make_shared<vector<std::byte>>();
	hr = this->loadShaders(spD3d11Dev, sptrBlobVS, nullptr);
	if (FAILED(hr)) return hr;

	// create input layout
	auto spanBlobVS = span<std::byte>(*sptrBlobVS);
	hr = this->createInputLayout(spD3d11Dev, spanBlobVS);
	if (FAILED(hr)) return hr;

	// create vertics buffer
	hr = this->createVerticesBuffer(spD3d11Dev);
	if (FAILED(hr)) return hr;

	// create indices buffer
	hr = this->createIndicesBuffer(spD3d11Dev);
	if (FAILED(hr)) return hr;

	// create states
	hr = this->createStates(spD3d11Dev);
	return hr;


    return S_OK;
}

HRESULT MeshD3D::loadShaders(ComPtr<ID3D11Device>& spD3d11Dev, shared_ptr<vector<std::byte>> sptrBlobVS, shared_ptr<vector<std::byte>> sptrBlobPS)
{
	if (!sptrBlobVS) sptrBlobVS = make_shared<vector<std::byte>>();
	*sptrBlobVS = ShaderHelper::LoadCompiledShaderBlob(L"A5VS.cso");
	HRESULT hr = spD3d11Dev->CreateVertexShader(sptrBlobVS->data(), sptrBlobVS->size(), nullptr, &m_spShaderVS);
	if (FAILED(hr)) return hr;

	if (!sptrBlobPS) sptrBlobPS = make_shared<vector<std::byte>>();
	*sptrBlobPS = ShaderHelper::LoadCompiledShaderBlob(L"A5PS.cso");
	hr = spD3d11Dev->CreatePixelShader(sptrBlobPS->data(), sptrBlobPS->size(), nullptr, &m_spShaderPS);
	return hr;
}

HRESULT MeshD3D::createInputLayout(ComPtr<ID3D11Device>& spD3d11Dev, const std::span<std::byte>& spanVertexShader)
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",      0, DXGI_FORMAT_R32G32_FLOAT,       0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = spD3d11Dev->CreateInputLayout(desc, sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		spanVertexShader.data(), spanVertexShader.size_bytes(), &m_spInputLayout);

	return hr;
}

HRESULT MeshD3D::createVerticesBuffer(ComPtr<ID3D11Device>& spD3d11Dev)
{
	D3D11_BUFFER_DESC desc{
		.ByteWidth = static_cast<UINT>(sizeof(GeometryGenerator::Vertex) * m_uptrMesh->vertices.size()),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};

	D3D11_SUBRESOURCE_DATA data{ .pSysMem = m_uptrMesh->vertices.data() };

	HRESULT hr = spD3d11Dev->CreateBuffer(&desc, &data, &m_spVB);
	return hr;
}

HRESULT MeshD3D::createIndicesBuffer(ComPtr<ID3D11Device>& spD3d11Dev)
{
	D3D11_BUFFER_DESC desc{
		.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_uptrMesh->indices.size()),
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};

	D3D11_SUBRESOURCE_DATA data{ .pSysMem = m_uptrMesh->indices.data() };

	HRESULT hr = spD3d11Dev->CreateBuffer(&desc, &data, &m_spIB);
	return hr;
}

HRESULT MeshD3D::createStates(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	D3D11_RASTERIZER_DESC descR{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_BACK,
		// .FillMode = D3D11_FILL_WIREFRAME,
		// .CullMode = D3D11_CULL_NONE,

		.DepthClipEnable = true,
		.MultisampleEnable = true,
		.AntialiasedLineEnable = true
	};
	HRESULT hr = spD3d11Dev->CreateRasterizerState(&descR, &m_spRS);
	if (FAILED(hr)) return hr;

	D3D11_BLEND_DESC descB{
		.AlphaToCoverageEnable = false,
		.IndependentBlendEnable = false,
		.RenderTarget = {
			{
				.BlendEnable = true,
				.SrcBlend = D3D11_BLEND_SRC_ALPHA,
				.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_ONE,
				.DestBlendAlpha = D3D11_BLEND_ZERO,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
			}
		}
	};

	hr = spD3d11Dev->CreateBlendState(&descB, &m_spBS);
	return hr;
}

void MeshD3D::prepareRender(ComPtr<ID3D11DeviceContext>& spDevCtx)
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
	spDevCtx->PSSetShaderResources(0, 1, m_spSRV.GetAddressOf());
	spDevCtx->PSSetSamplers(0, 1, m_spSampler.GetAddressOf());
}