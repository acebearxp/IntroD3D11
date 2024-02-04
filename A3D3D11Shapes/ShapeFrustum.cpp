#include "pch.h"
#include "ShapeFrustum.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void ShapeFrustum::RenderScene(ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS_WireFrame.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}

void ShapeFrustum::createShape()
{
	m_uptrMesh = std::make_unique<GeometryGenerator::MeshData>(GeometryGenerator::CreateFrustum(15.0f, 25.0f, 40.0f, 3, 36));

	for (auto& v : m_uptrMesh->vertices) {
		v.pos.y -= 20.0f;
		v.color = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);
	}
}
