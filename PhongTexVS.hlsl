#include "TransformCB.hlsli"
#include "ShadowMapCB.hlsli"

struct VSOut
{
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEX_COORD;
    float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POSITION;
    float4 pos : SV_Position;
};

VSOut main(float3 position : POSITION, float3 normal : NORMAL, float2 texCoord : TEX_COORD)
{
    const matrix modelView = mul(TransformCB.model, TransformCB.view);
    const matrix modelViewProj = mul(modelView, TransformCB.proj);
    VSOut vsout;
    vsout.viewPos = (float3) mul(float4(position, 1.0f), modelView);
    vsout.viewNormal = mul(normal, (float3x3) modelView);
    vsout.texCoord = texCoord;
    vsout.pos = mul(float4(position, 1.0f), modelViewProj);
    
    // Transform the vertex position into projected space from the POV of the light.
    const float4 modelPos = mul(float4(position, 1.0f), TransformCB.model);
    vsout.lightPerspectivePos = mul(modelPos, ShadowMapCB.lightPerspective);
    
    return vsout;
}