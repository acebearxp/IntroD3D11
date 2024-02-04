cbuffer ConstantBuffer : register(b0)
{
	matrix mWorld;
	matrix mView;
	matrix mProjection;
}

struct VS_OUTPUT
{
	float4 Pos   : SV_POSITION;
	float4 Color : COLOR0;
};

VS_OUTPUT main(float4 Pos : POSITION, float4 Color : COLOR)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(Pos, mWorld);
	output.Pos = mul(output.Pos, mView);
	output.Pos = mul(output.Pos, mProjection);
	output.Color = Color;
	return output;
}