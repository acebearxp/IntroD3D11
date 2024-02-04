#pragma once
#include "Shape.h"
#include <thread>
#include <functional>
// Shape All in One
class ShapeAIO : public Shape
{
public:
	virtual ~ShapeAIO() {
		m_atomBreak = true;
		m_threadSkull.join();
	}
	void SetNotify(std::function<void(HWND)> argFn, HWND target) { m_notifyFn = argFn; m_notifyTarget = target; }
	void UpdateD3DBuf(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);

	void RenderScene(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx, UINT nSeg);
protected:
	void createShape() override;

	virtual GeometryGenerator::MeshData LoadMeshFromFile(const wchar_t* wszFile, bool bSkipContent=false);
private:
	constexpr static float c_fWidth = 200.0f;
	constexpr static float c_fDepth = 300.0f;

	std::vector<unsigned int> m_vSegments = { 0 };

	std::atomic_int m_atomLockCount = 1;
	std::atomic_bool m_atomBreak = false;
	std::thread m_threadSkull;
	GeometryGenerator::MeshData m_meshSkull;
	std::function<void(HWND)> m_notifyFn;
	HWND m_notifyTarget;
};

