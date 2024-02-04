#pragma once
#include "Shape.h"

class ShapeFrustum : public Shape
{
public:
	void RenderScene(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
protected:
	void createShape() override;
};

