#include "pch.h"
#include "Cube.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

Cube::Cube()
	: m_vertices{
		{ XMFLOAT4(-1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::White)   },
		{ XMFLOAT4(-1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Gray)    },
		{ XMFLOAT4(+1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Red)     },
		{ XMFLOAT4(+1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Green)   },
		{ XMFLOAT4(-1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Blue)    },
		{ XMFLOAT4(-1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow)  },
		{ XMFLOAT4(+1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan)    },
		{ XMFLOAT4(+1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Magenta) }
	},
	m_indicies{
		0, 1, 2,	 0, 2, 3, // 前
		4, 6, 5,	 4, 7, 6, // 后
		4, 5, 1,	 4, 1, 0, // 左
		3, 2, 6,	 3, 6, 7, // 右
		1, 5, 6,	 1, 6, 2, // 顶
		4, 0, 3,	 4, 3, 7  // 底
	}
{
}

Cube::~Cube()
{
}

HRESULT Cube::CreateD3dRes(ComPtr<ID3D11Device>& spD3d11Dev)
{
	// load shaders
	auto sptrBlobVS = make_shared<vector<std::byte>>();
	HRESULT hr = this->loadShaders(spD3d11Dev, sptrBlobVS, nullptr);
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

HRESULT Cube::loadShaders(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev, shared_ptr<vector<std::byte>> sptrBlobVS, shared_ptr<vector<std::byte>> sptrBlobPS)
{
	if (!sptrBlobVS) sptrBlobVS = make_shared<vector<std::byte>>();
	*sptrBlobVS = ShaderHelper::LoadCompiledShaderBlob(L"A1VS.cso");
	HRESULT hr = spD3d11Dev->CreateVertexShader(sptrBlobVS->data(), sptrBlobVS->size(), nullptr, &m_spShaderVS);
	if (FAILED(hr)) return hr;

	if (!sptrBlobPS) sptrBlobPS = make_shared<vector<std::byte>>();
	*sptrBlobPS = ShaderHelper::LoadCompiledShaderBlob(L"A1PS.cso");
	hr = spD3d11Dev->CreatePixelShader(sptrBlobPS->data(), sptrBlobPS->size(), nullptr, &m_spShaderPS);
	return hr;
}

HRESULT Cube::createInputLayout(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev, const std::span<std::byte>& spanVertexShader)
{
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = spD3d11Dev->CreateInputLayout(desc, sizeof(desc) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		spanVertexShader.data(), spanVertexShader.size_bytes(), &m_spInputLayout);

	return hr;
}

HRESULT Cube::createVerticesBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	D3D11_BUFFER_DESC desc{
		.ByteWidth = sizeof(m_vertices),
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER
	};

	D3D11_SUBRESOURCE_DATA data{ .pSysMem = m_vertices };

	HRESULT hr = spD3d11Dev->CreateBuffer(&desc, &data, &m_spVB);
	return hr;
}

HRESULT Cube::createIndicesBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev)
{
	D3D11_BUFFER_DESC desc{
		.ByteWidth = sizeof(m_indicies),
		.Usage = D3D11_USAGE_IMMUTABLE,
		.BindFlags = D3D11_BIND_INDEX_BUFFER
	};

	D3D11_SUBRESOURCE_DATA data{ .pSysMem = m_indicies };

	HRESULT hr = spD3d11Dev->CreateBuffer(&desc, &data, &m_spIB);
	return hr;
}

void Cube::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext4>& spDevCtx)
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