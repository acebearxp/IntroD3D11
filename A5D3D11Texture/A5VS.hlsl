#include "shared.hlsli"

VS_OUTPUT main(float4 pos : POSITION, float4 color: COLOR, float4 normal: NORMAL, float2 tex: TEXCOORD)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.Pos = mul(pos, mxWVP);
    output.PosW = mul(pos, mxWorld);
    output.Color = color;
    output.Normal = float4(mul((float3) normal, (float3x3) mxInvWorld), 1.0f);
    output.Tex = tex;
    
    return output;
}