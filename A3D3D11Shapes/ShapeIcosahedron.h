#pragma once
#include "Shape.h"
class ShapeIcosahedron : public Shape
{
public:
	void RenderScene1(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
	void RenderScene2(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
	void RenderScene3(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& spDevCtx);
protected:
	void createShape() override;
};

