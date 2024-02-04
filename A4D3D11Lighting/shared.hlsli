struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Pos;
    float  Range;
    float4 Attenuation;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
    matrix mWorldViewProject;
    matrix mWorldInvTrans;
    
    float4 v4EyePos;
    
    DirectionalLight gDirLight;
    PointLight gPointLight;
    
    Material gMaterial;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 PosW : POSITION;
};