#include "TransformCB.hlsli"
#include "LightPerspectiveCB.hlsli"

struct VSOut
{
    float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POS;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : POSITION)
{
    VSOut vsOut;
    const matrix modelViewProj = mul(mul(TransformCB.model, TransformCB.view), TransformCB.proj);

    vsOut.lightPerspectivePos = mul(float4(pos, 1.0f), mul(TransformCB.model, LightPerspectiveCB.lightPerspective));
    vsOut.pos = mul(float4(pos, 1.0f), modelViewProj);
    
    return vsOut;
}