struct VSOut
{
    float2 texCoord : TEX_COORD;
    float4 pos : SV_POSITION;
};

VSOut main(float3 position : POSITION, float2 texCoord : TEX_COORD)
{
    VSOut output;
  
    output.pos = float4(position,  1.0f);
    output.texCoord = texCoord;

    return output;
}