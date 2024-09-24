#include "BindablesPool.h"

BindablesPool& BindablesPool::GetInstance()
{
	static BindablesPool instance;
	return instance;
}