#pragma once

class Shape
{
public:
	Shape() {};

	HRESULT CreateD3dRes(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev);
protected:
	std::unique_ptr<GeometryGenerator::MeshData> m_uptrMesh;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_spInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spVB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_spIB;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_spShaderVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_spShaderPS;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_spRS_Solid;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_spRS_WireFrame;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_spBS;

	constexpr int getVBStride() { return (int)sizeof(GeometryGenerator::Vertex); }
	constexpr int getIBCount() { return (int)m_uptrMesh->indices.size(); }
	constexpr DXGI_FORMAT getIBFmt() { return DXGI_FORMAT_R32_UINT; }

	virtual void createShape() = 0;

	HRESULT loadShaders(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev,
		std::shared_ptr<std::vector<std::byte>> sptrBlobVS, std::shared_ptr<std::vector<std::byte>> sptrBlobPS);
	HRESULT createInputLayout(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev, const std::span<std::byte>& spanVertexShader);
	virtual HRESULT createVerticesBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev);
	HRESULT createIndicesBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev);
	HRESULT createStates(Microsoft::WRL::ComPtr<ID3D11Device>& spD3d11Dev);

	void prepareRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
};

