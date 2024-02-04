#include "pch.h"
#include "GeometryGenerator.h"

using namespace std;
using namespace DirectX;

GeometryGenerator::MeshData GeometryGenerator::CreateGrid(float fWidth, float fDepth, int nInWidth, int nInDepth, float fRatioU, float fRatioV)
{
	MeshData data;
	// At least should be 2*2
	if (nInWidth < 2) nInWidth = 2;
	if (nInDepth < 2) nInDepth = 2;

	float fWidthMin = -fWidth / 2;
	float fDepthMin = -fDepth / 2;
	float fStepInWidth = fWidth / (nInWidth - 1); 
	float fStepInDepth = fDepth / (nInDepth - 1);

	data.vertices.resize(nInWidth* nInDepth);
	for (int i = 0; i < nInDepth; i++) {
		float fz = fDepthMin + fStepInDepth * i;
		float fv = -fRatioV * i;
		for (int j = 0; j < nInWidth; j++) {
			float fx = fWidthMin + fStepInWidth * j;
			float fu = fRatioU * j;
			data.vertices[i * nInWidth + j].pos = XMFLOAT4(fx, 0.0f, fz, 1.0f);
			data.vertices[i * nInWidth + j].tex = XMFLOAT2(fu, fv);
		}
	}

	// Indices, each quad cell has 2 triangles
	data.indices.resize((nInWidth - 1) * (nInDepth - 1) * 2 * 3);
	int k = 0;
	for (int i = 0; i < nInDepth - 1; i++) {
		for (int j = 0; j < nInWidth - 1; j++) {
			data.indices[k + 0] = nInWidth * i + j;
			data.indices[k + 1] = nInWidth * (i + 1) + j;
			data.indices[k + 2] = nInWidth * i + j + 1;

			data.indices[k + 3] = nInWidth * i + j + 1;
			data.indices[k + 4] = nInWidth * (i + 1) + j;
			data.indices[k + 5] = nInWidth * (i + 1) + j + 1;

			k += 6;
		}
	}

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::CreateBox(float fWidth, float fHeight, float fDepth)
{
	MeshData data;

	float fw = 0.5f * fWidth, fh = 0.5f * fHeight, fd = 0.5f * fDepth;

	constexpr int size_v = 8;
	data.vertices.resize(size_v);
	for (int i = 0; i < size_v; i++) {
		float fx = fWidth  * ((i & 0b001) >> 0);
		float fy = fHeight * ((i & 0b010) >> 1);
		float fz = fDepth  * ((i & 0b100) >> 2);
		data.vertices[i].pos = XMFLOAT4(fw - fx, fh - fy, fd - fz, 1.0f);
	}

	data.indices = {
		0,1,2,  2,1,3,
		0,4,1,  1,4,5,
		0,2,4,  4,2,6,
		3,1,7,  7,1,5,
		3,7,2,  2,7,6,
		5,4,7,  7,4,6
	};

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::CreateBox(float fWidth, float fHeight, float fDepth, float fRatioU, float fRatioV)
{
	MeshData data;
	float fw = 0.5f * fWidth, fh = 0.5f * fHeight, fd = 0.5f * fDepth;

	data.vertices.resize(24);
	// front
	data.vertices[0].pos = XMFLOAT4(-fw, -fh, -fd, 1.0f); data.vertices[0].normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f); data.vertices[0].tex = XMFLOAT2(0.0f,    fRatioV);
	data.vertices[1].pos = XMFLOAT4(-fw, +fh, -fd, 1.0f); data.vertices[1].normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f); data.vertices[1].tex = XMFLOAT2(0.0f,    0.0f);
	data.vertices[2].pos = XMFLOAT4(+fw, -fh, -fd, 1.0f); data.vertices[2].normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f); data.vertices[2].tex = XMFLOAT2(fRatioU, fRatioV);
	data.vertices[3].pos = XMFLOAT4(+fw, +fh, -fd, 1.0f); data.vertices[3].normal = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f); data.vertices[3].tex = XMFLOAT2(fRatioU, 0.0f);
	// back
	data.vertices[4].pos = XMFLOAT4(-fw, -fh, +fd, 1.0f); data.vertices[4].normal = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f); data.vertices[4].tex = XMFLOAT2(0.0f,    fRatioV);
	data.vertices[5].pos = XMFLOAT4(-fw, +fh, +fd, 1.0f); data.vertices[5].normal = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f); data.vertices[5].tex = XMFLOAT2(0.0f,    0.0f);
	data.vertices[6].pos = XMFLOAT4(+fw, -fh, +fd, 1.0f); data.vertices[6].normal = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f); data.vertices[6].tex = XMFLOAT2(fRatioU, fRatioV);
	data.vertices[7].pos = XMFLOAT4(+fw, +fh, +fd, 1.0f); data.vertices[7].normal = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f); data.vertices[7].tex = XMFLOAT2(fRatioU, 0.0f);
	// left
	data.vertices[8].pos = XMFLOAT4(-fw, -fh, -fd, 1.0f); data.vertices[8].normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f); data.vertices[8].tex = XMFLOAT2(fRatioU,   fRatioV);
	data.vertices[9].pos = XMFLOAT4(-fw, -fh, +fd, 1.0f); data.vertices[9].normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f); data.vertices[9].tex = XMFLOAT2(0.0f,      fRatioV);
	data.vertices[10].pos = XMFLOAT4(-fw, +fh, -fd, 1.0f); data.vertices[10].normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f); data.vertices[10].tex = XMFLOAT2(fRatioU, 0.0f);
	data.vertices[11].pos = XMFLOAT4(-fw, +fh, +fd, 1.0f); data.vertices[11].normal = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f); data.vertices[11].tex = XMFLOAT2(0.0f,    0.0f);
	// right
	data.vertices[12].pos = XMFLOAT4(+fw, -fh, -fd, 1.0f); data.vertices[12].normal = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f); data.vertices[12].tex = XMFLOAT2(0.0f,    fRatioV);
	data.vertices[13].pos = XMFLOAT4(+fw, -fh, +fd, 1.0f); data.vertices[13].normal = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f); data.vertices[13].tex = XMFLOAT2(fRatioU, fRatioV);
	data.vertices[14].pos = XMFLOAT4(+fw, +fh, -fd, 1.0f); data.vertices[14].normal = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f); data.vertices[14].tex = XMFLOAT2(0.0f,    0.0f);
	data.vertices[15].pos = XMFLOAT4(+fw, +fh, +fd, 1.0f); data.vertices[15].normal = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f); data.vertices[15].tex = XMFLOAT2(fRatioU, 0.0f);
	// top
	data.vertices[16].pos = XMFLOAT4(-fw, -fh, -fd, 1.0f); data.vertices[16].normal = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f); data.vertices[16].tex = XMFLOAT2(0.0f,    fRatioV);
	data.vertices[17].pos = XMFLOAT4(-fw, -fh, +fd, 1.0f); data.vertices[17].normal = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f); data.vertices[17].tex = XMFLOAT2(0.0f,    0.0f);
	data.vertices[18].pos = XMFLOAT4(+fw, -fh, -fd, 1.0f); data.vertices[18].normal = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f); data.vertices[18].tex = XMFLOAT2(fRatioU, fRatioV);
	data.vertices[19].pos = XMFLOAT4(+fw, -fh, +fd, 1.0f); data.vertices[19].normal = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f); data.vertices[19].tex = XMFLOAT2(fRatioU, 0.0f);
	// bottom
	data.vertices[20].pos = XMFLOAT4(-fw, +fh, -fd, 1.0f); data.vertices[20].normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); data.vertices[20].tex = XMFLOAT2(0.0f,    fRatioV);
	data.vertices[21].pos = XMFLOAT4(-fw, +fh, +fd, 1.0f); data.vertices[21].normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); data.vertices[21].tex = XMFLOAT2(0.0f,    0.0f);
	data.vertices[22].pos = XMFLOAT4(+fw, +fh, -fd, 1.0f); data.vertices[22].normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); data.vertices[22].tex = XMFLOAT2(fRatioU, fRatioV);
	data.vertices[23].pos = XMFLOAT4(+fw, +fh, +fd, 1.0f); data.vertices[23].normal = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f); data.vertices[23].tex = XMFLOAT2(fRatioU, 0.0f);

	data.indices = {
		0,1,2,  2,1,3,
		4,6,5,  6,7,5,
		8,9,10, 10,9,11,
		12,14,13, 14,15,13,
		16,18,17, 18,19,17,
		20,21,22, 22,21,23
	};

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::CreateFrustum(float fRadiusTop, float fRadiusBottom, float fHeight, int nStack, int nSlice)
{
	MeshData data;

	// build sin&cos lookup table
	unique_ptr<float> uptrSinCos(new float[2 * (nSlice + 1)]);
	float* pData = uptrSinCos.get();
	for (int i = 0; i <= nSlice; i++) {
		XMScalarSinCos(&pData[2 * i], &pData[2 * i + 1], XM_2PI * i / nSlice);
	}

	data.vertices.resize((nSlice + 1) * (nStack + 1) + 2);
	data.vertices[0].pos = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // bottom center
	data.vertices[1].pos = XMFLOAT4(0.0f, fHeight, 0.0f, 1.0f); // top center
	for (int j = 0; j <= nStack; j++) {
		float fy = fHeight * j / nStack;
		float fR = fRadiusBottom + (fRadiusTop - fRadiusBottom) * j / nStack;
		for (int i = 0; i <= nSlice; i++) {
			float fx = fR * pData[2 * i + 1]; // cos
			float fz = fR * pData[2 * i]; // sin
			data.vertices[(nSlice + 1) * j + i + 2].pos = XMFLOAT4(fx, fy, fz, 1.0f);
		}
	}

	data.indices.resize((nStack + 1) * nSlice * 6);
	int k = 0;
	for (int j = 0; j < nStack; j++) {
		for (int i = 0; i < nSlice; i++) {
			// side
			int vertices_start = (nSlice + 1) * j + 2;

			data.indices[k + 0] = vertices_start + (i + 0) + (nSlice + 1);
			data.indices[k + 1] = vertices_start + (i + 1) + (nSlice + 1);
			data.indices[k + 2] = vertices_start + (i + 0) + 0;

			data.indices[k + 3] = vertices_start + (i + 0) + 0;
			data.indices[k + 4] = vertices_start + (i + 1) + (nSlice + 1);
			data.indices[k + 5] = vertices_start + (i + 1) + 0;

			k += 6;
		}
	}

	for (int i = 0; i < nSlice; i++) {
		// bottom
		data.indices[k + 3 * i + 0] = 0; // bottom center
		data.indices[k + 3 * i + 1] = 2 + (i + 0);
		data.indices[k + 3 * i + 2] = 2 + (i + 1);
		// top
		data.indices[k + 3 * (i + nSlice) + 0] = 1; // top center
		data.indices[k + 3 * (i + nSlice) + 1] = 2 + (nSlice + 1) * nStack + (i + 1);
		data.indices[k + 3 * (i + nSlice) + 2] = 2 + (nSlice + 1) * nStack + (i + 0);
	}

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::CreateIcosahedronWithAssistantPlanes(float fRadius)
{
	float a = fRadius * 0.8506508f;
	float b = fRadius * 0.5257311f;

	constexpr int T1 = 12;
	const XMFLOAT4 pos[T1] = {
		XMFLOAT4(  +a, 0.0f,   +b, 1.0f), XMFLOAT4(  +a, 0.0f,   -b, 1.0f),
		XMFLOAT4(  -a, 0.0f,   -b, 1.0f), XMFLOAT4(  -a, 0.0f,   +b, 1.0f),
		XMFLOAT4(  -b,   +a, 0.0f, 1.0f), XMFLOAT4(  +b,   +a, 0.0f, 1.0f),
		XMFLOAT4(  +b,   -a, 0.0f, 1.0f), XMFLOAT4(  -b,   -a, 0.0f, 1.0f),
		XMFLOAT4(0.0f,   +b,   +a, 1.0f), XMFLOAT4(0.0f,   -b,   +a, 1.0f),
		XMFLOAT4(0.0f,   -b,   -a, 1.0f), XMFLOAT4(0.0f,   +b,   -a, 1.0f)
	};

	constexpr DWORD k2[] = {
		0,1,2,  2,3,0,    1,0,3,  3,2,1,   // green  : orthogonal to y axis
		4,5,6,  6,7,4,    5,4,7,  7,6,5,   // yellow : orthogonal to z axis
		8,9,10, 10,11,8,  9,8,11, 11,10,9, // red    : orthogonal to x axis
		// ----- 
		0,1,5,    0,5,8,    0,8,9,    0,9,6,    0,6,1,
		1,0,6,    1,6,10,   1,10,11,  1,11,5,   1,5,0,
		2,3,4,    2,4,11,   2,11,10,  2,10,7,   2,7,3,
		3,2,7,    3,7,9,    3,9,8,    3,8,4,    3,4,2,

		4,5,11,   4,11,2,   4,2,3,    4,3,8,    4,8,5,
		5,4,8,    5,8,0,    5,0,1,    5,1,11,   5,11,4,
		6,7,10,   6,10,1,   6,1,0,    6,0,9,    6,9,7,
		7,6,9,    7,9,3,    7,3,2,    7,2,10,   7,10,6,

		8,9,0,    8,0,5,    8,5,4,    8,4,3,    8,3,9,
		9,8,3,    9,3,7,    9,7,6,    9,6,0,    9,0,8,
		10,11,1,  10,1,6,   10,6,7,   10,7,2,   10,2,11,
		11,10,2,  11,2,4,   11,4,5,   11,5,1,   11,1,10
	};
	constexpr int T4 = sizeof(k2) / sizeof(DWORD);

	MeshData data;
	data.vertices.resize(T1);
	for (int i = 0; i < T1; i++) {
		data.vertices[i].pos = pos[i];
	}

	data.indices.resize(T4);
	for (int i = 0; i < T4; i++) {
		data.indices[i] = k2[i];
	}

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::CreateIcosahedron(float fRadius)
{
	float a = fRadius * 0.8506508f;
	float b = fRadius * 0.5257311f;

	constexpr int T1 = 12;
	const XMFLOAT4 pos[T1] = {
		XMFLOAT4(+a, 0.0f,   +b, 1.0f), XMFLOAT4(+a, 0.0f,   -b, 1.0f),
		XMFLOAT4(-a, 0.0f,   -b, 1.0f), XMFLOAT4(-a, 0.0f,   +b, 1.0f),
		XMFLOAT4(-b,   +a, 0.0f, 1.0f), XMFLOAT4(+b,   +a, 0.0f, 1.0f),
		XMFLOAT4(+b,   -a, 0.0f, 1.0f), XMFLOAT4(-b,   -a, 0.0f, 1.0f),
		XMFLOAT4(0.0f,   +b,   +a, 1.0f), XMFLOAT4(0.0f,   -b,   +a, 1.0f),
		XMFLOAT4(0.0f,   -b,   -a, 1.0f), XMFLOAT4(0.0f,   +b,   -a, 1.0f)
	};

	constexpr DWORD k2[] = {
		// 穿过中心平面的一圈8个
		0,1,5,   10,11,1,   2,3,4,   9,8,3,
		1,0,6,   11,10,2,   3,2,7,   8,9,0,
		// 斜角8个
		0,5,8,   1,11,5,   2,4,11,   3,8,4,
		0,9,6,   1,6,10,   2,10,7,   3,7,9,
		// 顶底4个
		4,5,11,  5,4,8,
		6,7,10,  7,6,9
	};
	constexpr int T4 = sizeof(k2) / sizeof(DWORD);

	MeshData data;
	data.vertices.resize(T1);
	for (int i = 0; i < T1; i++) {
		data.vertices[i].pos = pos[i];
		auto vNorm = XMVector3Normalize(XMVectorSet(pos[i].x, pos[i].y, pos[i].z, 0.0f));
		XMStoreFloat4(&data.vertices[i].normal, vNorm);
	}

	data.indices.resize(T4);
	for (int i = 0; i < T4; i++) {
		data.indices[i] = k2[i];
	}

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::CreateSphereFromIcosahedron(float fRadius, int nSubdivision)
{
	MeshData data = CreateIcosahedron(fRadius);

	constexpr int MAX_LIMIT = 1000000;
	for (int i = 0; i < nSubdivision; i++) {
		data = subdivide(data);
		if (data.indices.size() > MAX_LIMIT) break;
	}

	projectToSphere(data.vertices, fRadius);

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::CreateSphereFromTetrahedron(float fRadius, int nSubdivision)
{
	float a = fRadius * 0.81649658f;
	float b = fRadius * 0.33333333f;
	float c = fRadius * 0.47140452f;

	const XMFLOAT4 pos[4] = {
		XMFLOAT4(0.0f, fRadius,   0.0f, 1.0f),
		XMFLOAT4(  +a,      -b,     -c, 1.0f),
		XMFLOAT4(  -a,      -b,     -c, 1.0f),
		XMFLOAT4(0.0f,      -b, 2.0f*c, 1.0f)
	};

	constexpr DWORD k[12] = {
		0,1,2,
		0,2,3,
		0,3,1,
		1,3,2
	};

	MeshData data;

	data.vertices.resize(4);
	for (int i = 0; i < 4; i++) {
		data.vertices[i].pos = pos[i];
		// 正中心(0,0,0), 因此法线正好就是pos
		auto vNormal = XMVector3Normalize(XMVectorSet(pos[i].x, pos[i].y, pos[i].z, 0.0f));
		XMStoreFloat4(&data.vertices[i].normal, vNormal);
	}

	data.indices.resize(12);
	for (int i = 0; i < 12; i++) data.indices[i] = k[i];

	constexpr int MAX_LIMIT = 1000000;
	for (int i = 0; i < nSubdivision; i++) {
		data = subdivide(data);
		if (data.indices.size() > MAX_LIMIT) break;
	}

	projectToSphere(data.vertices, fRadius);

	return data;
}

GeometryGenerator::MeshData GeometryGenerator::subdivide(const MeshData& data)
{
	MeshData dataTes;
	/*       v0
			 *
			/ \
		   /   \
		m2*-----*m0
		 / \   / \
		/   \ /   \
	   *-----*-----*
	   v2    m1     v1	*/
	for (int i = 0; i < data.indices.size() / 3; i++) {
		const Vertex v[3] = {
			data.vertices[data.indices[3 * i + 0]],
			data.vertices[data.indices[3 * i + 1]],
			data.vertices[data.indices[3 * i + 2]],
		};

		// push v0 v1 v2
		for (const auto& x : v) dataTes.vertices.push_back(x);

		Vertex m[3];
		for (int j = 0; j < 3; j++) {
			const Vertex& va = v[j];
			const Vertex& vb = v[(j + 1) % 3];

			m[j] = Vertex();
			m[j].pos.x = 0.5f * (va.pos.x + vb.pos.x);
			m[j].pos.y = 0.5f * (va.pos.y + vb.pos.y);
			m[j].pos.z = 0.5f * (va.pos.z + vb.pos.z);
			m[j].pos.w = 1.0f;

			auto vNorm = XMVector3Normalize(XMVectorSet(m[j].pos.x, m[j].pos.y, m[j].pos.z, 0.0f));
			XMStoreFloat4(&m[j].normal, vNorm);

			// push m0 m1 m2
			dataTes.vertices.push_back(m[j]);
		}

		constexpr int k[] = {
			0,3,5,
			5,3,4,
			4,3,1,
			5,4,2
		};
		for (const int& x : k) {
			dataTes.indices.push_back(6 * i + x);
		}
	}
	return dataTes;
}

void GeometryGenerator::projectToSphere(std::vector<Vertex>& vertices, float fRadius)
{
	for (auto& v : vertices) {
		auto pos = XMFLOAT3(v.pos.x, v.pos.y, v.pos.z);
		auto vNorm = XMVector3Normalize(XMLoadFloat3(&pos));
		auto vScale = fRadius * vNorm;
		XMStoreFloat4(&v.pos, vScale);
		v.pos.w = 1.0f;
	}
}