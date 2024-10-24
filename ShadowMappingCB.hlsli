struct ShadowMapping
{
    matrix lightPerspective;
};

ConstantBuffer<ShadowMapping> ShadowMappingCB : register(b1);