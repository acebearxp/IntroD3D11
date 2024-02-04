#include "pch.h"
#include "ShapeCube.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;


void ShapeCube::RenderScene(ComPtr<ID3D11DeviceContext>& spDevCtx, bool bWireFrame)
{
	this->prepareRender(spDevCtx);
	// RS State
	if(bWireFrame) spDevCtx->RSSetState(m_spRS_WireFrame.Get());
	else spDevCtx->RSSetState(m_spRS_Solid.Get());

	// Blend State
	spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	spDevCtx->DrawIndexed((UINT)m_uptrMesh->indices.size(), 0, 0);
}

void ShapeCube::createShape()
{
	m_uptrMesh = std::make_unique<GeometryGenerator::MeshData>();
	m_uptrMesh->vertices.insert(m_uptrMesh->vertices.end(), {
		// 前
		{ XMFLOAT4(-1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::White),   XMFLOAT4(+0.0f, +0.0f, -1.0f, 0.0f)},
		{ XMFLOAT4(-1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Gray),    XMFLOAT4(+0.0f, +0.0f, -1.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Red),     XMFLOAT4(+0.0f, +0.0f, -1.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Green),   XMFLOAT4(+0.0f, +0.0f, -1.0f, 0.0f)},
		// 后
		{ XMFLOAT4(-1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Blue),    XMFLOAT4(+0.0f, +0.0f, +1.0f, 0.0f)},
		{ XMFLOAT4(-1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow),  XMFLOAT4(+0.0f, +0.0f, +1.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan),    XMFLOAT4(+0.0f, +0.0f, +1.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Magenta), XMFLOAT4(+0.0f, +0.0f, +1.0f, 0.0f)},
		// 左
		{ XMFLOAT4(-1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::White),   XMFLOAT4(-1.0f, +0.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(-1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Blue),    XMFLOAT4(-1.0f, +0.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(-1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow),  XMFLOAT4(-1.0f, +0.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(-1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Gray),    XMFLOAT4(-1.0f, +0.0f, +0.0f, 0.0f)},
		// 右
		{ XMFLOAT4(+1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Green),   XMFLOAT4(+1.0f, +0.0f, -0.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Red),     XMFLOAT4(+1.0f, +0.0f, -0.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan),    XMFLOAT4(+1.0f, +0.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Magenta), XMFLOAT4(+1.0f, +0.0f, +0.0f, 0.0f)},
		// 顶
		{ XMFLOAT4(-1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Gray),    XMFLOAT4(+0.0f, +1.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(-1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow),  XMFLOAT4(+0.0f, +1.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, +1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan),    XMFLOAT4(+0.0f, +1.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, +1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Red),     XMFLOAT4(+0.0f, +1.0f, +0.0f, 0.0f)},
		// 底
		{ XMFLOAT4(-1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::White),   XMFLOAT4(+0.0f, -1.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, -1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Green),   XMFLOAT4(+0.0f, -1.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(+1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Magenta), XMFLOAT4(+0.0f, -1.0f, +0.0f, 0.0f)},
		{ XMFLOAT4(-1.0f, -1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Blue),    XMFLOAT4(+0.0f, -1.0f, +0.0f, 0.0f)},
	});
	m_uptrMesh->indices.insert(m_uptrMesh->indices.end(), {
		0,  1,  2,	  0,  2,  3, // 前
		4,  6,  5,	  4,  7,  6, // 后
		8,  9, 10,	  8, 10, 11, // 左
		12, 13, 14,	 12, 14, 15, // 右
		16, 17, 18,	 16, 18, 19, // 顶
		20, 21, 22,	 20, 22, 23  // 底
	});
}
