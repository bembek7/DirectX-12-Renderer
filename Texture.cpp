#include "Texture.h"
#include "TexLoader.h"
#include "Graphics.h"

Texture::Texture(Graphics& graphics, const UINT slot, const std::string& fileName, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters) :
	slot(slot)
{
	texture = std::move(TexLoader::LoadTextureFromFile(graphics, fileName));

	// create the descriptor in the heap
	const D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
		.Format = texture->GetDesc().Format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = texture->GetDesc().MipLevels },
	};
	graphics.device->CreateShaderResourceView(texture.Get(), &srvDesc, graphics.GetCbvSrvCpuHandle());
	graphics.OffsetCbvSrvCpuHandle(1);

	CD3DX12_ROOT_PARAMETER rootParameter{};
	descRange = { D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, slot };
	rootParameter.InitAsDescriptorTable(1, &descRange, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters.push_back(std::move(rootParameter));
	rootParameterIndex = (UINT)rootParameters.size() - 1;
}

void Texture::Bind(Graphics& graphics, const CD3DX12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle) noexcept
{
	graphics.commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, srvGpuHandle);
}

bool Texture::HasAlpha() const noexcept
{
	return hasAlpha;
}
//
//std::string Texture::ResolveID(const UINT slot, const std::string& fileName, std::vector<CD3DX12_ROOT_PARAMETER>& rootParameters) noexcept
//{
//	std::stringstream ss;
//	ss << slot << fileName;
//	return ss.str();
//}