#include "Bindable.h"
#include "Graphics.h"
#include <stdexcept>
#include<sstream>

void Bindable::Update(Graphics& graphics)
{}

std::string Bindable::GetID() const
{
	if (!id.empty())
	{
		return id;
	}
	std::stringstream ss;
	ss << "No ID generation implemented for " << typeid(*this).name() << " shouldn't be used with shared pool.";
	throw std::runtime_error(ss.str());
}

ID3D11DeviceContext* Bindable::GetContext(Graphics& graphics) noexcept
{
	return graphics.context.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& graphics) noexcept
{
	return graphics.device.Get();
}