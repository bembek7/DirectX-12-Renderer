#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

// could be struct now, but maybe expended later
class Model
{
public:
	Model(const std::string& fileName);
	Model() = default;
public:
	struct Vertex {
		Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal) : position(position), normal(normal) {}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};
