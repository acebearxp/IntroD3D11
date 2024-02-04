#pragma once
#include "Shape.h"

class ShapeWave : public Shape
{
public:
	void RenderScene(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx, float t);
protected:
	void createShape() override;
private:
	const int c_nInWidth = 40;
	const int c_nInDepth = 60;
};

