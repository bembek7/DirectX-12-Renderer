struct MainPerspective
{
    matrix view;
    matrix proj;
};

ConstantBuffer<MainPerspective> MainPerspectiveCB : register(b1);