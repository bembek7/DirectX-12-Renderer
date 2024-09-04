#pragma once
#include "BetterWindows.h"
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <memory>
#include <unordered_map>
#include <d3d11.h>
#include "Bindable.h"

struct aiMesh;

class Model
{
public:
	Model(Graphics& graphics, const aiMesh* const assignedMesh, const bool hasTexture, const bool usesPhong);
	void Bind(Graphics& graphics) noexcept;

	size_t GetIndicesNumber() const noexcept;
private:
	/*struct VertexPos {
		VertexPos(DirectX::XMFLOAT3 position) : position(position) {}
		DirectX::XMFLOAT3 position;
	};
	struct VertexPosN : public VertexPos {
		VertexPosN(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal) : VertexPos(position), normal(normal) {}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct VertexPosNTex : public VertexPosN {
		VertexPosNTex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT3 texCords) : VertexPosN(position, normal), texCords(texCords) {}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 texCords;
	};*/
	std::vector<float> verticesData;
	unsigned int vertexSize = 0;
	std::vector<unsigned int> indices;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
	std::vector<std::unique_ptr<Bindable>> bindables;
	std::vector<std::shared_ptr<Bindable>> sharedBindables;

	enum class VertexElement
	{
		Position,
		Normal,
		TexCoords
	};
	std::unordered_map<VertexElement, unsigned int> elementOffset;
};
