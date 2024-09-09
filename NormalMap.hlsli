Texture2D nMap : register(t2);

float3 CalculateViewNormal(const float3 viewNormal, const float3 tangent, const float3 bitangent, const float3 normalSample)
{
    const float3x3 tanToView = float3x3(normalize(tangent), normalize(bitangent), normalize(viewNormal));
    
    return mul(normalSample * 2 - 1.0f, tanToView);
}