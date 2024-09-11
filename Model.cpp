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

const std::unordered_map<ShaderSettings, std::wstring, ShaderSettingsHash> Model::vsPaths =
{
	{ ShaderSettings{}, L"SolidVS.cso"},
	{ ShaderSettings::Skybox, L"SkyboxVS.cso" },
	{ ShaderSettings::Color, L"SolidVS.cso" },
	{ ShaderSettings::Color | ShaderSettings::Phong, L"PhongColorVS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture, L"PhongTexVS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap, L"PhongTexNMVS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::SpecularMap, L"PhongTexVS.cso" },
	{ ShaderSettings::Phong | ShaderSettings::Texture | ShaderSettings::NormalMap | ShaderSettings::SpecularMap, L"PhongTexNMVS.cso" },
};

Model::Model(Graphics& graphics, const aiMesh* const assignedMesh, const ShaderSettings shaderSettings, std::shared_ptr<IndexBuffer> givenIndexBuffer) :
	indexBuffer(givenIndexBuffer)
{
	if (assignedMesh->HasPositions())
	{
		inputElementDescs.push_back({ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Position] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Phong))
	{
		inputElementDescs.push_back({ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Normal] = vertexSize;
		vertexSize += sizeof(float) * 3;
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::Texture))
	{
		inputElementDescs.push_back({ "TEX_COORD", 0u, DXGI_FORMAT_R32G32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::TexCoords] = vertexSize;
		vertexSize += sizeof(float) * 2;
	}
	if (static_cast<bool>(shaderSettings & ShaderSettings::NormalMap))
	{
		inputElementDescs.push_back({ "TANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Tangent] = vertexSize;
		vertexSize += sizeof(float) * 3;
		inputElementDescs.push_back({ "BITANGENT", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0u });
		elementOffset[VertexElement::Bitangent] = vertexSize;
		vertexSize += sizeof(float) * 3;
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
				break;
			}
			default:
				break;
			}
		}
	}

	auto& bindablesPool = BindablesPool::GetInstance();

	bindables.push_back(std::make_unique<VertexBuffer>(graphics, verticesData, vertexSize));

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