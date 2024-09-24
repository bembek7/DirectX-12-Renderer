#include "SkyBox.h"
#include "ShaderSettings.h"
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include "Graphics.h"

Skybox::Skybox(Graphics& graphics, const std::string& fileName)
{
	static constexpr auto shaderSettings = ShaderSettings::Skybox;

	Assimp::Importer importer;

	const aiScene* const scene = importer.ReadFile(fileName,
		aiProcess_ConvertToLeftHanded |
		aiProcess_JoinIdenticalVertices
	);

	if (!scene)
	{
		throw std::runtime_error(importer.GetErrorString());
	}

	model = std::make_unique<Model>(graphics, scene->mMeshes[0], shaderSettings);
	material = std::make_unique<Material>(graphics, scene->mMaterials[0], shaderSettings);

	transformConstantBuffer = std::make_unique<ConstantBuffer<TransformBuffer>>(graphics, transformBuffer, BufferType::Vertex, 0u);

	skyDepthStencilState = std::make_unique<DepthStencilState>(graphics, DepthStencilState::Usage::Skybox);
}

void Skybox::Draw(Graphics& graphics)
{
	auto newViewProj = DirectX::XMMatrixTranspose(graphics.GetCamera() * graphics.GetProjection());
	DirectX::XMStoreFloat4x4(&transformBuffer.viewProjection, std::move(newViewProj));

	model->Bind(graphics);
	material->Bind(graphics);
	transformConstantBuffer->Update(graphics);
	transformConstantBuffer->Bind(graphics);
	skyDepthStencilState->Bind(graphics);
	graphics.DrawIndexed(model->GetIndicesNumber());
}