#include "TransformCB.hlsli"

struct VSOut
{
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float4 pos : SV_Position;
};

VSOut main(float3 position : POSITION, float3 normal : NORMAL)
{
    const matrix modelView = mul(TransformCB.model, TransformCB.view);
    const matrix modelViewProj = mul(modelView, TransformCB.proj);
    VSOut vsout;
    vsout.viewPos = (float3) mul(float4(position, 1.0f), modelView);
    vsout.viewNormal = mul(normal, (float3x3) modelView);
    vsout.pos = mul(float4(position, 1.0f), modelViewProj);
    
    return vsout;
}