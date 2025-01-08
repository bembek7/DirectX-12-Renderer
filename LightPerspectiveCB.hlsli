struct LightPerspective
{
    matrix lightPerspective;
};

ConstantBuffer<LightPerspective> LightPerspectiveCB : register(b1);