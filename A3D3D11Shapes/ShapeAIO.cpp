#include "pch.h"
#include "ShapeAIO.h"
#include "S11.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

void ShapeAIO::UpdateD3DBuf(ComPtr<ID3D11DeviceContext>& spDevCtx)
{
	HRESULT hr;
	size_t offset;
	size_t total_size;
	D3D11_MAPPED_SUBRESOURCE data;

	offset = m_uptrMesh->vertices.size() - m_meshSkull.vertices.size();
	total_size = sizeof(GeometryGenerator::Vertex) * m_meshSkull.vertices.size();
	hr = spDevCtx->Map(m_spVB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &data);
	memcpy_s(reinterpret_cast<GeometryGenerator::Vertex*>(data.pData) + offset, total_size, m_meshSkull.vertices.data(), total_size);
	spDevCtx->Unmap(m_spVB.Get(), 0);

	for (auto& x : m_meshSkull.indices)		x += static_cast<UINT>(offset);
	offset = m_uptrMesh->indices.size() - m_meshSkull.indices.size();
	total_size = sizeof(UINT) * m_meshSkull.indices.size();
	hr = spDevCtx->Map(m_spIB.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &data);
	memcpy_s(reinterpret_cast<UINT*>(data.pData) + offset, total_size, m_meshSkull.indices.data(), total_size);
	spDevCtx->Unmap(m_spIB.Get(), 0);

	m_atomLockCount--;
}

void ShapeAIO::RenderScene(ComPtr<ID3D11DeviceContext>& spDevCtx, UINT nSeg)
{
	if (nSeg+1 < m_vSegments.size() - m_atomLockCount) {
		this->prepareRender(spDevCtx);
		// RS State
		spDevCtx->RSSetState(m_spRS_WireFrame.Get());

		// Blend State
		spDevCtx->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		spDevCtx->DrawIndexed(m_vSegments[nSeg + 1] - m_vSegments[nSeg], m_vSegments[nSeg], 0);
	}
}

void ShapeAIO::createShape()
{
	int offsetI = 0, offsetV = 0;
	m_uptrMesh = std::make_unique<GeometryGenerator::MeshData>();

	auto fnAppendMesh = [&uptrMesh = m_uptrMesh, &vSegments = m_vSegments, &offsetI, &offsetV](const GeometryGenerator::MeshData& mesh) {
		uptrMesh->vertices.insert(uptrMesh->vertices.end(), mesh.vertices.begin(), mesh.vertices.end());
		offsetI += static_cast<int>(mesh.indices.size());
		vSegments.push_back(offsetI);
		for (const auto& x : mesh.indices) {
			uptrMesh->indices.push_back(x + offsetV);
		}
		offsetV += static_cast<int>(mesh.vertices.size());
	};
	
	fnAppendMesh(GeometryGenerator::CreateGrid(c_fWidth, c_fDepth, 40, 60, 1.0f, 1.0f));
	fnAppendMesh(GeometryGenerator::CreateBox(30.0f, 10.0f, 30.0f));
	fnAppendMesh(GeometryGenerator::CreateFrustum(5.0f, 10.0f, 25.0f, 4, 36));
	fnAppendMesh(GeometryGenerator::CreateSphereFromIcosahedron(5.0f, 3));
	
	fnAppendMesh(LoadMeshFromFile(L"./Models/car.txt"));
	fnAppendMesh(LoadMeshFromFile(L"./Models/skull.txt", true));	

	for (auto& v : m_uptrMesh->vertices)	 v.color = XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f);

	// skull delay loading 
	m_threadSkull = thread([this](){
		m_meshSkull = LoadMeshFromFile(L"./Models/skull.txt", false);
		for (auto& v : m_meshSkull.vertices)	 v.color = XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f);
		if (m_atomBreak) return;
		m_notifyFn(m_notifyTarget);
	});
}

GeometryGenerator::MeshData ShapeAIO::LoadMeshFromFile(const wchar_t* wszFile, bool bSkipContent)
{
	GeometryGenerator::MeshData data;
	vector<char> buf;

	// read file
    wchar_t wszPath[MAX_PATH];
    int len = GetModuleFileName(0, wszPath, MAX_PATH);
    if (len > 0) {
		wchar_t *p = wcsrchr(wszPath, L'\\');
		if (p != nullptr) {
			p++;
			wcscpy_s(p, MAX_PATH - (p - wszPath), wszFile);
			HANDLE hModel = CreateFile(wszPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hModel != INVALID_HANDLE_VALUE) {
				DWORD dwSize = GetFileSize(hModel, nullptr);
				if (dwSize != INVALID_FILE_SIZE) {
					buf.resize(dwSize);
					ReadFile(hModel, buf.data(), dwSize, nullptr, nullptr);
				}
			}

			CloseHandle(hModel);
		}
    }

	// parse for header
	int countVT, countTC;
	char* pStart = buf.data();
	constexpr char anchorVC[] = "VertexCount:";
	constexpr char anchorTC[] = "TriangleCount:";
	pStart = strstr(pStart, anchorVC);
	if (pStart != nullptr) {
		pStart += sizeof(anchorVC);
		sscanf_s(pStart, "%d", &countVT);
		data.vertices.resize(countVT);
	}
	pStart = strstr(pStart, anchorTC); 
	if (pStart != nullptr) {
		pStart += sizeof(anchorTC);
		sscanf_s(pStart, "%d", &countTC);
		data.indices.resize(3 * countTC);
	}

	if (!bSkipContent) {
		int count = 0;
		auto fnProgress = [&pStart, &buf](int count) -> void {
#ifdef _DEBUG
			wchar_t bufProgress[64];
			if (buf.size() > 1024 * 512) {
				if (count % 1000 == 0) {
					swprintf_s(bufProgress, L"%5.2f%%\x20", 100.0f * (pStart - buf.data()) / buf.size());
					OutputDebugString(bufProgress);
					if (count % 5000 == 4000) OutputDebugString(L"\n");
				}
				else if (count < 0) {
					OutputDebugString(L"100.00%\n");
				}
			}
#endif
		};

		// parse for mesh:vertices
		char* pBegin = strchr(pStart, '{');
		pStart = pBegin + 1;

		auto v = data.vertices.begin();
		while (v < data.vertices.end()) {
			// move next
			char* pLine = strchr(pStart, '\n');
			pStart = pLine + 1;
			sscanf_s(pStart, "%f %f %f", &(v->pos.x), &(v->pos.y), &(v->pos.z));
			v->pos.w = 1.0f;
			v->color.y = 0.8f, v->color.w = 1.0f;
			v++;
			fnProgress(count++);
			if (m_atomBreak) break;
		}

		// parse for mesh:indices
		pBegin = strchr(pStart, '{');
		pStart = pBegin + 1;

		auto i = data.indices.begin();
		while (i < data.indices.end()) {
			// move next
			char* pLine = strchr(pStart, '\n');
			pStart = pLine + 1;
			sscanf_s(pStart, "%d %d %d", &(*i), &(*(i + 1)), &(*(i + 2)));
			i += 3;
			fnProgress(count++);
			if (m_atomBreak) break;
		}
		fnProgress(-1);
	}

	return data;
}
