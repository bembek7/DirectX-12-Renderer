#include "Model.h"
#include "BetterWindows.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Utils.h"
#include "ThrowMacros.h"
#include <d3dcompiler.h>
#include "ShadersPool.h"

const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> Model::vsPaths =
{
	{ ShaderSettings{}, L"PositionVS.cso"},
	{ ShaderSettings::Color, L"PosNormalVS.cso" },
	{ ShaderSettings::Texture, L"PosNormalTexCoordVS.cso" },
	{ ShaderSettings::Texture | ShaderSettings::NormalMap, L"PosNormalTexCoordTanBitanVS.cso" },
	{ ShaderSettings::Texture | ShaderSettings::SpecularMap, L"PosNormalTexCoordVS.cso" },
	{ ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap, L"PosNormalTexCoordTanBitanVS.cso" },
};

Model::Model(Graphics& graphics, const aiMesh* const assignedMesh, const ShaderSettings shaderSettings, std::shared_ptr<IndexBuffer> givenIndexBuffer) :
	indexBuffer(givenIndexBuffer)
{
	vertexLayout = GenerateVertexLayout(assignedMesh, shaderSettings);

	if (!indexBuffer)
	{
		indexBuffer = GenerateIndexBuffer(graphics, assignedMesh);
	}

	bindables.push_back(GenerateVertexBuffer(graphics, assignedMesh, vertexLayout));

	std::wstring vertexShaderPath;
	auto it = Model::vsPaths.find(shaderSettings);
	if (it != Model::vsPaths.end())
	{
		vertexShaderPath = it->second;
	}
	else
	{
		throw std::runtime_error("Vertex shader path not found for given flags");
	}

	auto& shadersPool = ShadersPool::GetInstance();
	vertexShaderBlob = shadersPool.GetShaderBlob(vertexShaderPath); // be wary that its a shared ptr to com ptr
}

void Model::Bind(ID3D12GraphicsCommandList* const commandList) noexcept
{
	for (auto& bindable : bindables)
	{
		bindable->Bind(commandList);
	}

	indexBuffer->Bind(commandList);
}

Model::VertexLayout Model::GenerateVertexLayout(const aiMesh* const assignedMesh, const ShaderSettings shaderSettings) const
{
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	std::unordered_map<VertexElement, UINT> elementOffset;
	UINT vertexSize = 0U;

	if (assignedMesh->HasPositions())
	{
		inputLayout.push_back({ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Position] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Color))
	{
		inputLayout.push_back({ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Normal] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Texture))
	{
		inputLayout.push_back({ "TEX_COORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::TexCoords] = vertexSize;
		vertexSize += sizeof(float) * 2;
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::NormalMap))
	{
		inputLayout.push_back({ "TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Tangent] = vertexSize;
		vertexSize += sizeof(float) * 3;
		inputLayout.push_back({ "BITANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Bitangent] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}

	return { inputLayout, elementOffset, vertexSize };
}

std::shared_ptr<IndexBuffer> Model::GenerateIndexBuffer(Graphics& graphics, const aiMesh* const assignedMesh) const
{
	std::vector<WORD> indices;
	for (size_t i = 0; i < assignedMesh->mNumFaces; i++)
	{
		for (size_t j = 0; j < assignedMesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(assignedMesh->mFaces[i].mIndices[j]);
		}
	}

	return std::make_shared<IndexBuffer>(graphics, indices);
}

std::unique_ptr<VertexBuffer> Model::GenerateVertexBuffer(Graphics& graphics, const aiMesh* const assignedMesh, const Model::VertexLayout& vertexLayout) const
{
	std::vector<float> verticesData;
	const auto& vertexSize = vertexLayout.vertexSize;
	const unsigned int numVertices = assignedMesh->mNumVertices;
	verticesData.resize(vertexSize / sizeof(float) * numVertices);
	for (size_t i = 0; i < numVertices; i++)
	{
		for (const auto& element : vertexLayout.elementOffset)
		{
			const auto elementIndex = i * vertexSize / sizeof(float) + element.second / sizeof(float);
			switch (element.first)
			{
			case VertexElement::Position:
			{
				const auto& position = assignedMesh->mVertices[i];
				verticesData[elementIndex] = position.x;
				verticesData[elementIndex + 1] = position.y;
				verticesData[elementIndex + 2] = position.z;
				break;
			}
			case VertexElement::Normal:
			{
				const auto& normal = assignedMesh->mNormals[i];
				verticesData[elementIndex] = normal.x;
				verticesData[elementIndex + 1] = normal.y;
				verticesData[elementIndex + 2] = normal.z;
				break;
			}
			case VertexElement::TexCoords:
			{
				const auto& texCoords = assignedMesh->mTextureCoords[0][i];
				verticesData[elementIndex] = texCoords.x;
				verticesData[elementIndex + 1] = texCoords.y;
				break;
			}
			case VertexElement::Tangent:
			{
				const auto& tangent = assignedMesh->mTangents[i];
				verticesData[elementIndex] = tangent.x;
				verticesData[elementIndex + 1] = tangent.y;
				verticesData[elementIndex + 2] = tangent.z;
				break;
			}
			case VertexElement::Bitangent:
			{
				const auto& bitangent = assignedMesh->mBitangents[i];
				verticesData[elementIndex] = bitangent.x;
				verticesData[elementIndex + 1] = bitangent.y;
				verticesData[elementIndex + 2] = bitangent.z;
				break;
			}
			default:
				break;
			}
		}
	}

	return std::make_unique<VertexBuffer>(graphics, verticesData, vertexSize, numVertices);
}

CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT Model::GetInputLayout() const noexcept
{
	return D3D12_INPUT_LAYOUT_DESC{ vertexLayout.inputLayout.data(), (UINT)vertexLayout.inputLayout.size() };
}

ID3DBlob* Model::GetVSBlob() const noexcept
{
	return vertexShaderBlob->Get();
}

std::shared_ptr<IndexBuffer> Model::ShareIndexBuffer() noexcept
{
	return indexBuffer;
}

UINT Model::GetIndicesNumber() const noexcept
{
	return indexBuffer->GetIndicesNumber();
}