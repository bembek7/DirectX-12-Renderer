#include "TransformCB.hlsli"

struct VSOut
{
    float3 viewNormal : NORMAL;
    float2 texCoord : TEX_COORD;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
    float3 viewPosition : VIEW_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float4 pos : SV_Position;
};

VSOut main(float3 position : POSITION, float3 normal : NORMAL, float2 texCoord : TEX_COORD, float3 tangent : TANGENT, float3 bitangent : BITANGENT)
{
    const matrix modelView = mul(TransformCB.model, TransformCB.view);
    const matrix modelViewProj = mul(modelView, TransformCB.proj);
    VSOut vsout;
    vsout.viewNormal = mul(normal, (float3x3) modelView);
    vsout.texCoord = texCoord;
    vsout.viewTangent = mul(tangent, (float3x3) modelView);
    vsout.viewBitangent = mul(bitangent, (float3x3) modelView);
    vsout.viewPosition = mul(float4(position, 1.0f), modelView).xyz;
    vsout.worldPosition = mul(float4(position, 1.0f), TransformCB.model).xyz;
    vsout.pos = mul(float4(position, 1.0f), modelViewProj);
    
    return vsout;
}