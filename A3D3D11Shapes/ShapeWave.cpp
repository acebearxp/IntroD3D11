#include "pch.h"
#include "ShapeWave.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void ShapeWave::RenderScene(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx, float t)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS_WireFrame.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	// static float last = 0.0f;
	// if (t - last > 0.01f) {
		for (int i = 0; i < c_nInDepth; i++) {
			for (int j = 0; j < c_nInWidth; j++) {
				float yx = 5.0f * sinf(2.0f * XM_2PI * j / c_nInWidth + XM_2PI * t / 2.0f);
				float yz = 3.0f * sinf(1.5f * XM_2PI * i / c_nInDepth + XM_2PI * t / 2.7f);
				m_uptrMesh->vertices[c_nInWidth * i + j].pos.y = yx + yz;
			}
		}

		D3D11_MAPPED_SUBRESOURCE data;
		HRESULT hr = spDevCtx->Map(m_spVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, data.RowPitch, m_uptrMesh->vertices.data(), sizeof(GeometryGenerator::Vertex) * m_uptrMesh->vertices.size());
		spDevCtx->Unmap(m_spVB.Get(), 0);
	//	last = t;
	//}

	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}

void ShapeWave::createShape()
{
	m_uptrMesh = std::make_unique<GeometryGenerator::MeshData>(GeometryGenerator::CreateGrid(200.0f, 300.0f, c_nInWidth, c_nInDepth, 1.0f, 1.0f));

	for (auto& v : m_uptrMesh->vertices) {
		v.color = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);
	}
}
