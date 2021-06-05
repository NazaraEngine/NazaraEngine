// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <stdexcept>
#include <vma/vk_mem_alloc.h>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanTexture::VulkanTexture(Vk::Device& device, const TextureInfo& params) :
	m_image(VK_NULL_HANDLE),
	m_allocation(nullptr),
	m_device(device),
	m_params(params)
	{
		VkImageCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.mipLevels = params.mipmapLevel;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = ToVulkan(params.usageFlags);

		VkImageViewCreateInfo createInfoView = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		createInfoView.subresourceRange = {
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		};

		InitForFormat(params.pixelFormat, createInfo, createInfoView);

		switch (params.type)
		{
			case ImageType::E1D:
				NazaraAssert(params.width > 0, "Width must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D;

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = 1;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = 1;
				break;

			case ImageType::E1D_Array:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = 1;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = params.height;
				break;

			case ImageType::E2D:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D;

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = params.height;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = 1;
				break;

			case ImageType::E2D_Array:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");
				NazaraAssert(params.depth > 0, "Depth must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = params.height;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = params.depth;
				break;

			case ImageType::E3D:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");
				NazaraAssert(params.depth > 0, "Depth must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_3D;

				createInfo.imageType = VK_IMAGE_TYPE_3D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = params.height;
				createInfo.extent.depth = params.depth;
				createInfo.arrayLayers = 1;
				break;

			case ImageType::Cubemap:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				createInfoView.subresourceRange.layerCount = 6;

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = params.height;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = 6;
				createInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
				break;

			default:
				break;
		}

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkResult result = vmaCreateImage(m_device.GetMemoryAllocator(), &createInfo, &allocInfo, &m_image, &m_allocation, nullptr);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate image: " + TranslateVulkanError(result));

		createInfoView.image = m_image;

		if (!m_imageView.Create(device, createInfoView))
		{
			// FIXME
			vmaDestroyImage(m_device.GetMemoryAllocator(), m_image, m_allocation);
			throw std::runtime_error("Failed to create image view: " + TranslateVulkanError(m_imageView.GetLastErrorCode()));
		}
	}

	VulkanTexture::~VulkanTexture()
	{
		vmaDestroyImage(m_device.GetMemoryAllocator(), m_image, m_allocation);
	}

	PixelFormat VulkanTexture::GetFormat() const
	{
		return m_params.pixelFormat;
	}

	UInt8 VulkanTexture::GetLevelCount() const
	{
		return m_params.mipmapLevel;
	}

	Vector3ui VulkanTexture::GetSize(UInt8 level) const
	{
		return Vector3ui(GetLevelSize(m_params.width, level), GetLevelSize(m_params.height, level), GetLevelSize(m_params.depth, level));
	}

	ImageType VulkanTexture::GetType() const
	{
		return m_params.type;
	}

	bool VulkanTexture::Update(const void* ptr)
	{
		std::size_t textureSize = m_params.width * m_params.height * m_params.depth * PixelFormatInfo::GetBytesPerPixel(m_params.pixelFormat);
		if (m_params.type == ImageType::Cubemap)
			textureSize *= 6;

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = textureSize;
		createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VmaAllocationInfo allocationInfo;

		VkBuffer stagingBuffer;
		VmaAllocation stagingAllocation;

		VkResult result = vmaCreateBuffer(m_device.GetMemoryAllocator(), &createInfo, &allocInfo, &stagingBuffer, &stagingAllocation, &allocationInfo);
		if (result != VK_SUCCESS)
		{
			NazaraError("Failed to allocate staging buffer: " + TranslateVulkanError(result));
			return false;
		}

		CallOnExit freeStaging([&] {
			vmaDestroyBuffer(m_device.GetMemoryAllocator(), stagingBuffer, stagingAllocation);
		});

		std::memcpy(allocationInfo.pMappedData, ptr, textureSize);

		Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Graphics);
		if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			return false;

		VkImageSubresourceLayers subresourceLayers = { //< FIXME
			VK_IMAGE_ASPECT_COLOR_BIT,
			0, //< mipLevel
			0, //< baseArrayLayer
			(m_params.type == ImageType::Cubemap) ? 6 : 1 //< layerCount
		};

		VkImageSubresourceRange subresourceRange = { //< FIXME
			VK_IMAGE_ASPECT_COLOR_BIT,
			0, //< baseMipLevel
			1, //< levelCount
			subresourceLayers.baseArrayLayer, //< baseArrayLayer
			subresourceLayers.layerCount      //< layerCount
		};

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

		copyCommandBuffer->CopyBufferToImage(stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceLayers, m_params.width, m_params.height, m_params.depth);

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

		if (!copyCommandBuffer->End())
			return false;

		Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetDefaultFamilyIndex(QueueType::Graphics), 0);
		if (!transferQueue.Submit(copyCommandBuffer))
			return false;

		transferQueue.WaitIdle();

		return true;
	}

	void VulkanTexture::InitForFormat(PixelFormat pixelFormat, VkImageCreateInfo& createImage, VkImageViewCreateInfo& createImageView)
	{
		createImageView.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A
		};

		// TODO: Fill this switch
		switch (pixelFormat)
		{
			case PixelFormat::BGR8:
			case PixelFormat::BGR8_SRGB:
			case PixelFormat::BGRA8:
			case PixelFormat::BGRA8_SRGB:
			case PixelFormat::RGB8:
			case PixelFormat::RGB8_SRGB:
			case PixelFormat::RGBA8:
			case PixelFormat::RGBA8_SRGB:
			case PixelFormat::RGBA32F:
			{
				createImage.format = ToVulkan(pixelFormat);
				createImageView.format = createImage.format;
				break;
			}

			case PixelFormat::Depth16:
			{
				createImage.format = VK_FORMAT_D16_UNORM;
				createImageView.format = createImage.format;
				createImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				break;
			}

			case PixelFormat::Depth16Stencil8:
			{
				createImage.format = VK_FORMAT_D16_UNORM_S8_UINT;
				createImageView.format = createImage.format;
				createImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				break;
			}

			case PixelFormat::Depth24Stencil8:
			{
				createImage.format = VK_FORMAT_D24_UNORM_S8_UINT;
				createImageView.format = createImage.format;
				createImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				break;
			}

			case PixelFormat::Depth32F:
			{
				createImage.format = VK_FORMAT_D32_SFLOAT;
				createImageView.format = createImage.format;
				createImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
				break;
			}

			case PixelFormat::Depth32FStencil8:
			{
				createImage.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
				createImageView.format = createImage.format;
				createImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				break;
			}

			case PixelFormat::L8:
			{
				createImage.format = VK_FORMAT_R8_UNORM;
				createImageView.format = createImage.format;
				createImageView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_A
				};
				break;
			}

			case PixelFormat::LA8:
			{
				createImage.format = VK_FORMAT_R8G8_UNORM;
				createImageView.format = createImage.format;
				createImageView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_G
				};
				break;
			}

			default:
				throw std::runtime_error("Unsupported pixel format " + PixelFormatInfo::GetName(pixelFormat));
		}
	}
}
