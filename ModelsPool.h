#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Model.h"

class ModelsPool
{
public:
	static ModelsPool& GetInstance();
	ModelsPool(ModelsPool const&) = delete;
	void operator=(ModelsPool const&) = delete;

	std::shared_ptr<Model> GetModel(const std::string& modelPath);
private:
	ModelsPool() = default;
private:
	std::unordered_map<std::string, std::weak_ptr<Model>> modelsMap;
};
