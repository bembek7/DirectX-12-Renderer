#include "ModelsPool.h"
#include "Model.h"

ModelsPool& ModelsPool::GetInstance()
{
	static ModelsPool instance;
	return instance;
}

//std::shared_ptr<Model> ModelsPool::GetModel(const std::string& modelPath)
//{
//	auto modelIt = modelsMap.find(modelPath);
//	std::shared_ptr<Model> sharedModel;
//	if (modelIt != modelsMap.end())
//	{
//		sharedModel = modelIt->second.lock();
//		if (!sharedModel)
//		{
//			sharedModel = std::make_shared<Model>(modelPath);
//			modelIt->second = sharedModel;
//		}
//	}
//	else
//	{
//		sharedModel = std::make_shared<Model>(modelPath);
//		modelsMap[modelPath] = sharedModel;
//	}
//	return sharedModel;
//}