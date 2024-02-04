struct SunLight
{
    float4 vAmbient;
    float4 vDiffuse;
    float4 vSpecular;
    float4 vDir;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix mxWorld;
    matrix mxView;
    matrix mxProjection;
    matrix mxWVP;
    matrix mxInvWorld;
    
    float4 vEyePos;
    float4 vFog; // { R, G, B, Start }
    
    SunLight gSun;
    
    float4 vTmDelta;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 PosW : POSITION;
    float4 Color : COLOR;
    float4 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};