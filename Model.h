#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

// could be struct now, but maybe expended later
class Model
{
public:
	Model(const std::string& fileName);

public:
	struct Vertex {
		Vertex(const float x, const float y, const float z, const float nx, const float ny, const float nz) : position(x, y, z), normal(nx, ny, nz) {}
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string fileName;
};
