struct GPassPSOut
{
    float3 color : SV_TARGET0;
    float4 viewNormal_roughness : SV_TARGET1;
    float3 specColor : SV_TARGET2;
    float4 viewPosition : SV_TARGET3;
    float4 worldPosition : SV_TARGET4;
};

GPassPSOut ConstructGPassPSOut(float3 color, float3 viewNormal, float roughness, float3 viewPosition, float4 worldPosition, float3 specularColor = float3(1.0f, 1.0f, 1.0f))
{
    GPassPSOut psOut;
    psOut.color = color;
    psOut.viewNormal_roughness = float4(viewNormal, roughness);
    psOut.viewPosition = float4(viewPosition, 1.f);
    psOut.worldPosition = worldPosition;
    psOut.specColor = specularColor;
    return psOut;
}