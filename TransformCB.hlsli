struct Transform
{
    matrix model;
    matrix view;
    matrix proj;
};

ConstantBuffer<Transform> TransformCB : register(b0);