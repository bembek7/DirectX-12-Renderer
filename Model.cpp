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

Model::Model(Graphics& graphics, const aiMesh* const assignedMesh)
{
	if (assignedMesh->HasPositions())
	{
		inputElementDescs.push_back({ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Position] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}
	if (assignedMesh->HasNormals())
	{
		inputElementDescs.push_back({ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Normal] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}
	if (assignedMesh->HasTextureCoords(0))
	{
		inputElementDescs.push_back({ "TEX_COORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::TexCoords] = vertexSize;
		vertexSize += sizeof(float) * 2;
	}

	for (size_t i = 0; i < assignedMesh->mNumFaces; i++)
	{
		for (size_t j = 0; j < assignedMesh->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(assignedMesh->mFaces[i].mIndices[j]);
		}
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
				const auto position = assignedMesh->mVertices[i];
				verticesData[elementIndex] = position.x;
				verticesData[elementIndex + 1] = position.y;
				verticesData[elementIndex + 2] = position.z;
				break;
			}
			case VertexElement::Normal:
			{
				const auto normal = assignedMesh->mNormals[i];
				verticesData[elementIndex] = normal.x;
				verticesData[elementIndex + 1] = normal.y;
				verticesData[elementIndex + 2] = normal.z;
				break;
			}
			case VertexElement::TexCoords:
			{
				const auto texCoords = assignedMesh->mTextureCoords[0][i];
				verticesData[elementIndex] = texCoords.x;
				verticesData[elementIndex + 1] = 1 - texCoords.y; // flip the y beacuse of how d3d and opengl tex coords differ
				break;
			}
			default:
				break;
			}
		}
	}

	// establish which vertex shader to use

	std::wstring vertexShaderPath;

	vertexShaderPath = L"PhongVS.cso";
	auto& bindablesPool = BindablesPool::GetInstance();

	bindables.push_back(std::make_unique<VertexBuffer>(graphics, verticesData, vertexSize));
	bindables.push_back(std::make_unique<IndexBuffer>(graphics, indices));

	auto vertexShader = bindablesPool.GetBindable<VertexShader>(graphics, vertexShaderPath);
	const VertexShader& vertexShaderRef = dynamic_cast<VertexShader&>(*vertexShader);
	sharedBindables.push_back(bindablesPool.GetBindable<InputLayout>(graphics, inputElementDescs, vertexShaderRef.GetBufferPointer(), vertexShaderRef.GetBufferSize(), Utils::WstringToString(vertexShaderPath)));
	sharedBindables.push_back(std::move(vertexShader));
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

size_t Model::GetIndicesNumber() const noexcept
{
	return indices.size();
}