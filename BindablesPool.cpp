#include "BindablesPool.h"
#include"Bindable.h"

BindablesPool& BindablesPool::GetInstance()
{
	static BindablesPool instance;
	return instance;
}