#pragma once
#include<unordered_map>
#include<string>
#include<memory>
#include "Bindable.h"
#include "VertexShader.h"

class BindablesPool
{
public:
	static BindablesPool& GetInstance();
	BindablesPool(BindablesPool const&) = delete;
	void operator=(BindablesPool const&) = delete;

	template<typename T, typename... Params>
	std::shared_ptr<Bindable> GetBindable(Graphics& graphics, const Params&... params)
	{
		std::shared_ptr<Bindable> bindableSP = std::make_shared<T>(graphics, params...);
		const std::string bindableID = bindableSP->GetID();
		auto bindableIt = bindablesMap.find(bindableID);
		if (bindableIt == bindablesMap.end())
		{
			bindablesMap[bindableID] = std::move(bindableSP);
		}
		return bindablesMap[bindableID];
	}

private:
	BindablesPool() = default;
private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> bindablesMap;
};
