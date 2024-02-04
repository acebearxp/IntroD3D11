#pragma once
#include "Shape.h"
class ShapeSphere : public Shape
{
public:
	void RenderScene(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx, UINT nSeg, bool bWireFrame);
	int GetCount()const { return static_cast<int>(m_vSegments.size()) - 1; }
protected:
	void createShape() override;
private:
	std::vector<UINT> m_vSegments = {0};
};

