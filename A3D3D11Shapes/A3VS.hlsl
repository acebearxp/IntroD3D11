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

VS_OUTPUT main(float4 pos : POSITION, float4 color: COLOR)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul(pos, mWorld);
	output.Pos = mul(output.Pos, mView);
	output.Pos = mul(output.Pos, mProjection);
	output.Color = color;
	return output;
}