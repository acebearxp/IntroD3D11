#pragma once
#include "Shape.h"

class ShapeCube : public Shape
{
public:
	void RenderScene(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx, bool bWireFrame);
protected:
	void createShape() override;
};
