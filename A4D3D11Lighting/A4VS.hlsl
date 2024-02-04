#include "shared.hlsli"

VS_OUTPUT main(float4 pos : POSITION, float4 color: COLOR, float4 normal: NORMAL)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	// output.Pos = mul(pos, mWorld);
	// output.Pos = mul(output.Pos, mView);
	// output.Pos = mul(output.Pos, mProjection);
    output.Pos = mul(pos, mWorldViewProject);
	output.PosW = mul(pos, mWorld).xyz;
    output.Normal = mul((float3) normal, (float3x3) mWorldInvTrans);
	
	output.Color = color;
	return output;
}