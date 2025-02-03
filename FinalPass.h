#pragma once
#include "Pass.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

class Graphics;
class Actor;


class FinalPass : public Pass
{
public:
	FinalPass(Graphics& graphics, ID3D12Resource* const sceneColorTexture, ID3D12Resource* const lightMapTexture);

	void Execute(Graphics& graphics);
private:
	std::unique_ptr<PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> drawingBundle;
	std::unique_ptr<IndexBuffer> indexBuffer;
	std::unique_ptr<VertexBuffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

