struct Transform
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

ConstantBuffer<Transform> TransformCBuf : register(b0);

float4 main( float3 pos : POSITION ) : SV_POSITION
{
    return mul(float4(pos, 1.0f), TransformCBuf.modelViewProj);
}