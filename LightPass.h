#pragma once
#include "Pass.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Light.h"
#include "RTVHeap.h"

class LightPass : public Pass
{
public:
	LightPass(Graphics& graphics, ID3D12Resource* const sceneNormal_RoughnessTexture, ID3D12Resource* const sceneSpecularColor, ID3D12Resource* const sceneViewPosition, Light* const light);

	void Execute(Graphics& graphics);

	ID3D12Resource* GetLightMap() noexcept;
private:
	Light* light;
	std::unique_ptr<PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> drawingBundle;
	std::unique_ptr<IndexBuffer> indexBuffer;
	std::unique_ptr<VertexBuffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	std::unique_ptr<RTVHeap> rtvHeap;
};

