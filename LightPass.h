#pragma once
#include "Pass.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Light.h"

class LightPass : public Pass
{
public:
	LightPass(Graphics& graphics, ID3D12Resource* const sceneNormal_RoughnessTexture, ID3D12Resource* const sceneSpecularColor,
		ID3D12Resource* const sceneViewPosition, ID3D12Resource* const sceneWorldPosition, ID3D12Resource* const lightDepthBuffer,
		Light* const light);

	void Execute(Graphics& graphics, const CD3DX12_CPU_DESCRIPTOR_HANDLE& lightMapRtvHandle);

	ID3D12Resource* GetLightMap() noexcept;
private:
	Light* light;
	std::unique_ptr<PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> drawingBundle;
	std::unique_ptr<IndexBuffer> indexBuffer;
	std::unique_ptr<VertexBuffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	static std::unordered_map<LightType, std::wstring> shaderPaths;
};

