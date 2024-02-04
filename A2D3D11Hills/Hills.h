#pragma once

class Hills
{
public:
	Hills() {};

	HRESULT CreateD3dRes(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev);
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext4>& spDevCtx);
private:
	GeometryGenerator::MeshData m_mesh = GeometryGenerator::CreateGrid(160.0f, 160.0f, 50, 50, 1.0f, 1.0f);
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_spInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spVB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spIB;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_spShaderVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_spShaderPS;
	constexpr UINT getVBStride() { return sizeof(GeometryGenerator::Vertex); }
	constexpr UINT getIBCount() { return (UINT)m_mesh.indices.size(); }
	constexpr DXGI_FORMAT getIBFmt() { return DXGI_FORMAT_R32_UINT; }

	HRESULT loadShaders(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev,
		std::shared_ptr<std::vector<std::byte>> sptrBlobVS, std::shared_ptr<std::vector<std::byte>> sptrBlobPS);
	HRESULT createInputLayout(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev, const std::span<std::byte>& spanVertexShader);
	HRESULT createVerticesBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev);
	HRESULT createIndicesBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev);
};

