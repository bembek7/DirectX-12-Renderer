#pragma once
#include<unordered_map>
#include<string>
#include<memory>
#include "Bindable.h"

class BindablesPool
{
public:
	static BindablesPool& GetInstance();
	BindablesPool(BindablesPool const&) = delete;
	void operator=(BindablesPool const&) = delete;

	template<typename T, typename... Params>
	std::shared_ptr<Bindable> GetBindable(Graphics& graphics, const Params&... params)
	{
		const std::string bindableID = T::ResolveID(params...) + "#" + typeid(T).name();
		auto bindableIt = bindablesMap.find(bindableID);
		if (bindableIt == bindablesMap.end())
		{
			bindablesMap[bindableID] = std::make_shared<T>(graphics, params...);
		}
		return bindablesMap[bindableID];
	}

private:
	BindablesPool() = default;
private:
	std::unordered_map<std::string, std::shared_ptr<Bindable>> bindablesMap;
};
