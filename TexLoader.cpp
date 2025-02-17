#include "TexLoader.h"
#include "dxtex\DirectXTex.h"
#include "Utils.h"
#include "ThrowMacros.h"
#include <ranges>
#include "d3dx12\d3dx12.h"
#include "Graphics.h"

namespace Dx = DirectX;
namespace Wrl = Microsoft::WRL;
namespace views = std::ranges::views;

TexLoader& TexLoader::GetInstance()
{
	static TexLoader instance;
	return instance;
}

std::shared_ptr<TexLoader::Image> TexLoader::GetTexture(Graphics& graphics, const std::string& fileName)
{
	auto texIt = texturesMap.find(fileName);
	std::shared_ptr<Image> sharedTex;
	if (texIt != texturesMap.end())
	{
		sharedTex = texIt->second.lock();
		if (!sharedTex)
		{
			sharedTex = std::make_shared<TexLoader::Image>(TexLoader::LoadTextureFromFile(graphics, fileName));
			texIt->second = sharedTex;
		}
	}
	else
	{
		sharedTex = std::make_shared<TexLoader::Image>(TexLoader::LoadTextureFromFile(graphics, fileName));
		texturesMap[fileName] = sharedTex;
	}
	return sharedTex;
}

TexLoader::Image TexLoader::LoadTextureFromFile(Graphics& graphics, const std::string& fileName)
{
	Dx::ScratchImage scratchImage;
	CHECK_HR(Dx::LoadFromWICFile(Utils::StringToWstring("Textures\\" + fileName).c_str(), Dx::WIC_FLAGS_NONE, nullptr, scratchImage));

	Dx::ScratchImage mipChain;
	CHECK_HR(Dx::GenerateMipMaps(*scratchImage.GetImages(), Dx::TEX_FILTER_BOX, 0, mipChain));

	TexLoader::Image image;
	image.hasAlpha = !scratchImage.IsAlphaAllOpaque();

	{
		const auto& chainBase = *mipChain.GetImages();
		const D3D12_RESOURCE_DESC texDesc{
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Width = (UINT)chainBase.width,
			.Height = (UINT)chainBase.height,
			.DepthOrArraySize = 1,
			.MipLevels = (UINT16)mipChain.GetImageCount(),
			.Format = chainBase.format,
			.SampleDesc = {.Count = 1 },
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		CHECK_HR(graphics.GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&image.resource)
		));
	}

	// collect subresource data
	const std::vector<D3D12_SUBRESOURCE_DATA> subresourceData = { views::iota(0, (int)mipChain.GetImageCount()) |
		   views::transform([&](int i)
			   {
				   const auto img = mipChain.GetImage(i, 0, 0);
				   return D3D12_SUBRESOURCE_DATA{
					   .pData = img->pixels,
					   .RowPitch = (LONG_PTR)img->rowPitch,
					   .SlicePitch = (LONG_PTR)img->slicePitch,
				   };
			   }) |
		   std::ranges::to<std::vector>() };

	// create the intermediate upload buffer
	Wrl::ComPtr<ID3D12Resource> uploadBuffer;
	{
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
		const auto uploadBufferSize = GetRequiredIntermediateSize(image.resource.Get(), 0, (UINT)subresourceData.size());
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		CHECK_HR(graphics.GetDevice()->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)
		));
	}

	graphics.ResetCommandListAndAllocator();
	// write commands to copy data to upload texture (copying each subresource)
	UpdateSubresources(
		graphics.GetMainCommandList(),
		image.resource.Get(),
		uploadBuffer.Get(),
		0, 0,
		(UINT)subresourceData.size(),
		subresourceData.data()
	);

	const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		image.resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	graphics.GetMainCommandList()->ResourceBarrier(1, &barrier);

	// close command list
	CHECK_HR(graphics.GetMainCommandList()->Close());
	graphics.ExecuteCommandList();

	graphics.WaitForQueueFinish();
	return image;
}