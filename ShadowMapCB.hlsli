struct ShadowMap
{
    matrix lightPerspective;
};

ConstantBuffer<ShadowMap> ShadowMapCB : register(b1);