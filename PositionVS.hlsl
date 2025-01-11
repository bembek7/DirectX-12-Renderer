#include "TransformCB.hlsli"

float4 main( float3 pos : POSITION ) : SV_POSITION
{
    const matrix modelViewProj = mul(mul(TransformCB.model, TransformCB.view), TransformCB.proj);

    return mul(float4(pos, 1.0f), modelViewProj);
}