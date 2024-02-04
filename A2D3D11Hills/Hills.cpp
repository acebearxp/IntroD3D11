#include "pch.h"
#include "Hills.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

HRESULT Hills::CreateD3dRes(ComPtr<ID3D11Device>& spD3d11Dev)
{
	HRESULT hr;
	// load shaders
	auto sptrBlobVS = make_shared<vector<std::byte>>();
	hr = this->loadShaders(spD3d11Dev, sptrBlobVS, nullptr);
	if (FAILED(hr)) return hr;

	// create input layout
	auto spanBlobVS = span<std::byte>(*sptrBlobVS);
	hr = this->createInputLayout(spD3d11Dev, spanBlobVS);
	if (FAILED(hr)) return hr;

	// create vertices buffer
	hr = this->createVerticesBuffer(spD3d11Dev);
	if (FAILED(hr)) return hr;

	// create indices buffer
	hr = this->createIndicesBuffer(spD3d11Dev);
	if (FAILED(hr)) return hr;

    return hr;
}

HRESULT Hills::loadShaders(ComPtr<ID3D11Device>& spD3d11Dev, shared_ptr<vector<std::byte>> sptrBlobVS, shared_ptr<vector<std::byte>> sptrBlobPS)
{
	if (!sptrBlobVS) sptrBlobVS = make_shared<vector<std::byte>>();
	*sptrBlobVS = ShaderHelper::LoadCompiledShaderBlob(L"A2VS.cso");
	HRESULT hr = spD3d11Dev->CreateVertexShader(sptrBlobVS->data(), sptrBlobVS->size(), nullptr, &m_spShaderVS);
	if (FAILED(hr)) return hr;

	if (!sptrBlobPS) sptrBlobPS = make_shared<vector<std::byte>>();
	*sptrBlobPS = ShaderHelper::LoadCompiledShaderBlob(L"A2PS.cso");
	hr = spD3d11Dev->CreatePixelShader(sptrBlobPS->data(), sptrBlobPS->size(), nullptr, &m_spShaderPS);
	return hr;
}

HRESULT Hills::createInputLayout(ComPtr<ID3D11Device>& spD3d11Dev, const std::span<std::byte>& spanVertexShader)
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = spD3d11Dev->CreateInputLayout(desc, sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		spanVertexShader.data(), spanVertexShader.size_bytes(), &m_spInputLayout);

	return hr;
}

HRESULT Hills::createVerticesBuffer(ComPtr<ID3D11Device>& spD3d11Dev)
{
	// generate height
	for (auto& v : m_mesh.vertices) {
		float x = v.pos.x, z = v.pos.z;
		v.pos.y = 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));

		// for color
		if     (v.pos.y < -10.0f) v.color = XMFLOAT4(1.00f, 0.96f, 0.62f, 1.0f); // Sandy beach
		else if(v.pos.y <   5.0f) v.color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f); // Light yellow-green
		else if(v.pos.y <  12.0f) v.color = XMFLOAT4(0.10f, 0.48f, 0.19f, 1.0f); // Dark yellow-green
		else if(v.pos.y <  20.0f) v.color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f); // Dark brown
		else                      v.color = XMFLOAT4(1.00f, 1.00f, 1.00f, 1.0f); // White snow
	}

	D3D11_BUFFER_DESC desc{
		.ByteWidth = static_cast<UINT>(sizeof(GeometryGenerator::Vertex) * m_mesh.vertices.size()),
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER
	};

	D3D11_SUBRESOURCE_DATA data{ .pSysMem = m_mesh.vertices.data() };

	HRESULT hr = spD3d11Dev->CreateBuffer(&desc, &data, &m_spVB);
	return hr;
}

HRESULT Hills::createIndicesBuffer(ComPtr<ID3D11Device>& spD3d11Dev)
{
	D3D11_BUFFER_DESC desc{
		.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_mesh.indices.size()),
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_INDEX_BUFFER
	};

	D3D11_SUBRESOURCE_DATA data{ .pSysMem = m_mesh.indices.data()};

	HRESULT hr = spD3d11Dev->CreateBuffer(&desc, &data, &m_spIB);
	return hr;
}

void Hills::Render(ComPtr<ID3D11DeviceContext4>& spDevCtx)
{
	spDevCtx->IASetInputLayout(m_spInputLayout.Get());
	ID3D11Buffer* buf[] = { m_spVB.Get() };
	UINT stride[] = { this->getVBStride() };
	UINT offset[] = { 0 };
	spDevCtx->IASetVertexBuffers(0, 1, buf, stride, offset);
	spDevCtx->IASetIndexBuffer(m_spIB.Get(), this->getIBFmt(), 0);
	spDevCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// shaders
	spDevCtx->VSSetShader(m_spShaderVS.Get(), nullptr, 0);
	spDevCtx->PSSetShader(m_spShaderPS.Get(), nullptr, 0);
	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}