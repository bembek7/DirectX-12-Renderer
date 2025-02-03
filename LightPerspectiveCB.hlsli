struct LightPerspective
{
    matrix view;
    matrix proj;
};

ConstantBuffer<LightPerspective> LightPerspectiveCB : register(b1);