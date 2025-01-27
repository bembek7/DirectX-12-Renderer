#include "TransformCB.hlsli"

float4 main(float3 pos : POSITION) : SV_POSITION
{
    const matrix modelView = mul(TransformCB.model, TransformCB.view);
    const matrix modelViewProj = mul(modelView, TransformCB.proj);
    return mul(float4(pos, 1.0f), modelViewProj);
}