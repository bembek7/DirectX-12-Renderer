#include "TransformCB.hlsli"

struct VSOut
{
    float3 viewPosition : VIEW_POSITION;
    float4 pos : SV_POSITION;
};

VSOut main(float3 pos : POSITION)
{
    VSOut vsOut;
    
    const matrix modelView = mul(TransformCB.model, TransformCB.view);
    const matrix modelViewProj = mul(modelView, TransformCB.proj);

    vsOut.viewPosition = mul(pos, (float3x3) modelView);
    vsOut.pos = mul(float4(pos, 1.0f), modelViewProj);
    
    return vsOut;
}