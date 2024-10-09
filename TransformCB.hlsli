struct Transform
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

ConstantBuffer<Transform> TransformCB : register(b0);