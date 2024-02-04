cbuffer ConstantBuffer : register(b0)
{
	matrix mWorld;
	matrix mView;
	matrix mProjection;
}

struct VS_OUTPUT
{
	float4 Pos   : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	return input.Color;
}