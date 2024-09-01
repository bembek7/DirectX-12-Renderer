#include "Model.h"
#include "BetterWindows.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>

Model::Model(const std::string& fileName)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_GenNormals
	);

	if (!scene)
	{
		throw std::runtime_error(importer.GetErrorString());
	}

	const unsigned int numVertices = scene->mMeshes[0]->mNumVertices;

	vertices.reserve(numVertices);

	assert(scene->mMeshes[0]->HasNormals());
	for (size_t i = 0; i < numVertices; i++)
	{
		vertices.push_back(Vertex({ scene->mMeshes[0]->mVertices[i].x, scene->mMeshes[0]->mVertices[i].y, scene->mMeshes[0]->mVertices[i].z },
			{ scene->mMeshes[0]->mNormals[i].x, scene->mMeshes[0]->mNormals[i].y, scene->mMeshes[0]->mNormals[i].z }));
	}

	for (size_t i = 0; i < scene->mMeshes[0]->mNumFaces; i++)
	{
		for (size_t j = 0; j < scene->mMeshes[0]->mFaces[i].mNumIndices; j++)
		{
			indices.push_back(scene->mMeshes[0]->mFaces[i].mIndices[j]);
		}
	}

	assert(vertices.size() > 0 && indices.size() > 0);
}