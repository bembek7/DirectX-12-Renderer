#include "TransformCB.hlsli"
#include "MainPerspectiveCB.hlsli"

struct VSOut
{
    float4 mainPerspectivePos : MAIN_PERSPECTIVE_POSITION;
    float4 pos : SV_POSITION;
};

VSOut main(float3 pos : POSITION)
{
    VSOut vsOut;
    const matrix modelView = mul(TransformCB.model, TransformCB.view);
    const matrix modelViewProj = mul(modelView, TransformCB.proj);
    const matrix mainPerspective = mul(mul(TransformCB.model, MainPerspectiveCB.view), MainPerspectiveCB.proj);
    vsOut.mainPerspectivePos = mul(float4(pos, 1.0f), mainPerspective);
    vsOut.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vsOut;
    
}