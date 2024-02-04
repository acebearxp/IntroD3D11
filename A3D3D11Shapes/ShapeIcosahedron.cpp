#include "pch.h"
#include "ShapeIcosahedron.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void ShapeIcosahedron::RenderScene1(ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS_Solid.Get());

	// Blend State
	float factors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	spDevCtx->OMSetBlendState(m_spBS.Get(), factors, 0xffffffff);

	spDevCtx->DrawIndexed(this->getIBCount(), 0, 0);
}

void ShapeIcosahedron::RenderScene2(ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS_Solid.Get());

	// Blend State
	float factors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	spDevCtx->OMSetBlendState(m_spBS.Get(), factors, 0xffffffff);
	spDevCtx->DrawIndexed(36, 0, 0);

	spDevCtx->RSSetState(m_spRS_WireFrame.Get());
	spDevCtx->DrawIndexed(this->getIBCount() - 36, 36, 0);
}

void ShapeIcosahedron::RenderScene3(ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	this->prepareRender(spDevCtx);
	// RS State
	spDevCtx->RSSetState(m_spRS_Solid.Get());

	// Blend State
	float factors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	spDevCtx->RSSetState(m_spRS_WireFrame.Get());
	spDevCtx->DrawIndexed(this->getIBCount() - 36, 36, 0);
}

void ShapeIcosahedron::createShape()
{
	m_uptrMesh = std::make_unique<GeometryGenerator::MeshData>(GeometryGenerator::CreateIcosahedronWithAssistantPlanes(36.0f));

	int k = 0;
	for (auto& v : m_uptrMesh->vertices) {
		v.color = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);

		if (k / 4 == 0) {
			v.color = XMFLOAT4(0.0f, 0.8f, 0.0f, 0.7f);
		}
		else if (k / 4 == 1) {
			v.color = XMFLOAT4(0.8f, 0.8f, 0.0f, 0.7f);
		}
		else {
			v.color = XMFLOAT4(0.8f, 0.0f, 0.0f, 0.7f);
		}

		k++;
	}
}
