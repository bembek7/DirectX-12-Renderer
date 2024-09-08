#include "Model.h"
#include "BetterWindows.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>
#include <d3d11.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include "BindablesPool.h"
#include "InputLayout.h"
#include "Utils.h"

Model::Model(Graphics& graphics, const aiMesh* const assignedMesh, const bool hasTexture, const bool usesPhong, const bool hasNormalMap, std::shared_ptr<IndexBuffer> givenIndexBuffer) :
	indexBuffer(givenIndexBuffer)
{
	std::wstring vertexShaderPath;

	if (assignedMesh->HasPositions())
	{
		vertexShaderPath = L"SolidVS.cso";
		inputElementDescs.push_back({ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Position] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}
	if (usesPhong)
	{
		if (assignedMesh->HasNormals())
		{
			vertexShaderPath = L"PhongColorVS.cso";
			inputElementDescs.push_back({ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
			elementOffset[VertexElement::Normal] = vertexSize;
			vertexSize += sizeof(float) * 3;
		}
		if (assignedMesh->HasTextureCoords(0) && hasTexture)
		{
			vertexShaderPath = L"PhongTexVS.cso";
			inputElementDescs.push_back({ "TEX_COORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
			elementOffset[VertexElement::TexCoords] = vertexSize;
			vertexSize += sizeof(float) * 2;

			if (assignedMesh->HasTangentsAndBitangents() && hasNormalMap)
			{
				vertexShaderPath = L"PhongTexNMVS.cso";
				inputElementDescs.push_back({ "TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
				elementOffset[VertexElement::Tangent] = vertexSize;
				vertexSize += sizeof(float) * 3;
				inputElementDescs.push_back({ "BITANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
				elementOffset[VertexElement::Bitangent] = vertexSize;
				vertexSize += sizeof(float) * 3;
			}
		}
	}

	if (!indexBuffer)
	{
		for (size_t i = 0; i < assignedMesh->mNumFaces; i++)
		{
			for (size_t j = 0; j < assignedMesh->mFaces[i].mNumIndices; j++)
			{
				indices.push_back(assignedMesh->mFaces[i].mIndices[j]);
			}
		}

		indexBuffer = std::make_shared<IndexBuffer>(graphics, indices);
	}

	const unsigned int numVertices = assignedMesh->mNumVertices;
	verticesData.resize(vertexSize / sizeof(float) * numVertices);
	for (size_t i = 0; i < numVertices; i++)
	{
		for (const auto& element : elementOffset)
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
			}
			default:
				break;
			}
		}
	}

	auto& bindablesPool = BindablesPool::GetInstance();

	bindables.push_back(std::make_unique<VertexBuffer>(graphics, verticesData, vertexSize));

	auto vertexShader = bindablesPool.GetBindable<VertexShader>(graphics, vertexShaderPath);
	const VertexShader& vertexShaderRef = dynamic_cast<VertexShader&>(*vertexShader);
	sharedBindables.push_back(bindablesPool.GetBindable<InputLayout>(graphics, inputElementDescs, vertexShaderRef.GetBufferPointer(), vertexShaderRef.GetBufferSize(), Utils::WstringToString(vertexShaderPath)));
	sharedBindables.push_back(std::move(vertexShader));
	sharedBindables.push_back(indexBuffer);
}

void Model::Bind(Graphics& graphics) noexcept
{
	for (auto& bindable : bindables)
	{
		bindable->Update(graphics);
		bindable->Bind(graphics);
	}

	for (auto& sharedBindable : sharedBindables)
	{
		sharedBindable->Update(graphics);
		sharedBindable->Bind(graphics);
	}
}

std::shared_ptr<IndexBuffer> Model::ShareIndexBuffer() noexcept
{
	return indexBuffer;
}

size_t Model::GetIndicesNumber() const noexcept
{
	return indexBuffer->GetIndicesNumber();
}