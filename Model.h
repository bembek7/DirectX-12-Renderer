#pragma once
#include "BetterWindows.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "d3dx12\d3dx12.h"
#include "IndexBuffer.h"
#include "ShaderSettings.h"
#include "PipelineState.h"
#include "VertexBuffer.h"

struct aiMesh;
class IndexBuffer;

class Model
{
private:
	enum class VertexElement
	{
		Position,
		Normal,
		TexCoords,
		Tangent,
		Bitangent
	};

	struct VertexLayout
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
		std::unordered_map<VertexElement, UINT> elementOffset;
		UINT vertexSize = 0U;
	};

public:
	Model(Graphics& graphics, PipelineState::PipelineStateStream& pipelineStateStream, const aiMesh* const assignedMesh, const ShaderSettings shaderSettings, std::shared_ptr<IndexBuffer> givenIndexBuffer = nullptr);
	void Bind(ID3D12GraphicsCommandList* const commandList) noexcept;

	std::shared_ptr<IndexBuffer> ShareIndexBuffer() noexcept;
	UINT GetIndicesNumber() const noexcept;
private:
	VertexLayout GenerateVertexLayout(const aiMesh* const assignedMesh, const ShaderSettings shaderSettings) const;
	std::shared_ptr<IndexBuffer> GenerateIndexBuffer(Graphics& graphics, const aiMesh* const assignedMesh) const;
	std::unique_ptr<VertexBuffer> GenerateVertexBuffer(Graphics& graphics, const aiMesh* const assignedMesh, const VertexLayout& vertexLayout) const;

private:
	std::shared_ptr<Microsoft::WRL::ComPtr<ID3DBlob>> vertexShaderBlob;

	VertexLayout vertexLayout;
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::shared_ptr<IndexBuffer> indexBuffer; // we share index buffer so not to recreate it for shadow mapping

	static const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> vsPaths;
};
