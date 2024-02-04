#pragma once

class GeometryGenerator
{
public:
	struct Vertex
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 normal;
		DirectX::XMFLOAT2 tex;

		Vertex():pos(0.0f, 0.0f, 0.0f, 1.0f), color(1.0f, 1.0f, 1.0f, 1.0f), normal(0.0f, 1.0f, 0.0f, 0.0f), tex(0.0f, 0.0f) {}
		Vertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& color) : pos(pos), color(color), normal(0.0f, 1.0f, 0.0f, 0.0f), tex(0.0f, 0.0f) {}
		Vertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& color, const DirectX::XMFLOAT4& normal) : pos(pos), color(color), normal(normal), tex(0.0f, 0.0f) {}
	};

	struct MeshData
	{
		std::vector<Vertex> vertices;
		std::vector<UINT> indices;
	};

public:
	/// <summary>
	/// 产生矩形网格.网格总尺寸为fWidth*fDepth,fWidth方向(宽)划分nInWidth个顶点,fDepth方向(深)划分nInDepth个顶点.
	/// 共有 (nInWidth-1)*(nInDepth-1)个格子
	/// 坐标原点位于正中的位置
	/// fRatioU,fRatioV为纹理坐标系的缩放比例,1.0f代表纹理坐标系的尺寸与网格尺寸相同,0.5f代表铺满一格只需要半个纹理
	/// </summary>
	static MeshData CreateGrid(float fWidth, float fDepth, int nInWidth, int nInDepth, float fRatioU, float fRatioV);

	/// <summary>
	/// 产生长方体
	/// 坐标原点位于正中的位置
	/// </summary>
	static MeshData CreateBox(float fWidth, float fHeight, float fDepth);
	static MeshData CreateBox(float fWidth, float fHeight, float fDepth, float fRatioU, float fRatioV);

	/// <summary>
	/// 产生锥台体.上下表面半径fRadiusTop,fRadiusBottom;台高fHeight
	/// 台高方向分成nStack层,水平圆周划分成nSlice份
	/// 坐标原点位于底面正中的位置
	/// </summary>
	static MeshData CreateFrustum(float fRadiusTop, float fRadiusBottom, float fHeight, int nStack, int nSlice);

	/// <summary>
	/// 尝试20面体
	/// 1. 前置3个正交的辅助平面共绘制12个三角形,每个平面使用不同的2种对角线绘2次
	///    每个平面重复绘制了2次
	/// 2. 20面体共绘制60个三角形,分成12组,每组包含以同1个顶点为中心为5个三角形
	///    每个三个角形会重复绘制了3次
	/// </summary>
	static MeshData CreateIcosahedronWithAssistantPlanes(float fRadius);

	/// <summary>
	/// 正20面体
	/// </summary>
	static MeshData CreateIcosahedron(float fRadius);

	/// <summary>
	/// 由正20面体镶嵌产生近似球体
	/// </summary>
	static MeshData CreateSphereFromIcosahedron(float fRadius, int nSubdivision);

	/// <summary>
	/// 由正4面体镶嵌产生近似球体
	/// </summary>
	static MeshData CreateSphereFromTetrahedron(float fRadius, int nSubdivision);
private:
	/// <summary>
	/// 镶嵌三角形
	/// </summary>
	static MeshData subdivide(const MeshData& data);

	/// <summary>
	/// 投影到球面
	/// </summary>
	static void projectToSphere(std::vector<Vertex>& vertices, float fRadius);
};

