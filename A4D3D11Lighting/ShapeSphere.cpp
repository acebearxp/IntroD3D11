#include "pch.h"
#include "ShapeSphere.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void ShapeSphere::RenderScene(ComPtr<ID3D11DeviceContext>& spDevCtx, UINT nSeg, bool bWireFrame)
{
	this->prepareRender(spDevCtx);
	// RS State
	if(bWireFrame) spDevCtx->RSSetState(m_spRS_WireFrame.Get());
	else spDevCtx->RSSetState(m_spRS_Solid.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	spDevCtx->DrawIndexed(m_vSegments[nSeg + 1] - m_vSegments[nSeg], m_vSegments[nSeg], 0);
}

void ShapeSphere::createShape()
{
	m_uptrMesh = std::make_unique<GeometryGenerator::MeshData>();
	int offsetI = 0, offsetV = 0;

	auto appendMesh = [&uptrMesh = m_uptrMesh, &vSegments = m_vSegments, &offsetI, &offsetV](const GeometryGenerator::MeshData& mesh) {
		uptrMesh->vertices.insert(uptrMesh->vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
		offsetI += static_cast<int>(mesh.indices.size());
		vSegments.push_back(offsetI);
		for (const auto& x : mesh.indices) {
			uptrMesh->indices.push_back(x + offsetV);
		}
		offsetV += static_cast<int>(mesh.vertices.size());
	};

	for (int i = 0; i <= 5; i++) {
		auto mesh = GeometryGenerator::CreateSphereFromTetrahedron(36.0f, i);
		appendMesh(mesh);
	}

	for (int i = 0; i <= 3; i++) {
		auto mesh = GeometryGenerator::CreateSphereFromIcosahedron(36.0f, i);
		appendMesh(mesh);
	}

	for (auto& v : m_uptrMesh->vertices) {
		v.color = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);
	}
}