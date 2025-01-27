#include "TransformCB.hlsli"

struct VSOut
{
    float3 viewPosition : VIEW_POSITION;
    float3 worldPosition : WORLD_POSITION;
    float4 pos : SV_POSITION;
};

VSOut main(float3 pos : POSITION)
{
    VSOut vsout;
    
    const matrix modelView = mul(TransformCB.model, TransformCB.view);
    const matrix modelViewProj = mul(modelView, TransformCB.proj);

    vsout.viewPosition = mul(float4(pos, 1.0f), modelView).xyz;
    vsout.worldPosition = mul(float4(pos, 1.0f), TransformCB.model).xyz;
    vsout.pos = mul(float4(pos, 1.0f), modelViewProj);
    
    return vsout;
}