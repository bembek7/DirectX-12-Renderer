#pragma once
#include "BetterWindows.h"
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <unordered_map>
#include <d3d11.h>
#include "Bindable.h"
#include "IndexBuffer.h"

struct aiMesh;

class Model
{
public:
	Model(Graphics& graphics, const aiMesh* const assignedMesh, const bool hasTexture, const bool usesPhong, std::shared_ptr<IndexBuffer> givenIndexBuffer = nullptr);
	void Bind(Graphics& graphics) noexcept;
	std::shared_ptr<IndexBuffer> ShareIndexBuffer() noexcept;
	size_t GetIndicesNumber() const noexcept;
private:
	std::vector<float> verticesData;
	unsigned int vertexSize = 0;
	std::vector<unsigned int> indices;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;
	std::shared_ptr<IndexBuffer> indexBuffer; // we share index buffer so not to recreate it for shadow mapping

	enum class VertexElement
	{
		Position,
		Normal,
		TexCoords
	};
	std::unordered_map<VertexElement, unsigned int> elementOffset;
};
