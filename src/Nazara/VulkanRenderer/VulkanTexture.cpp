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
		createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

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
			case ImageType_1D:
				NazaraAssert(params.width > 0, "Width must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D;

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = 1;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = 1;
				break;

			case ImageType_1D_Array:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = 1;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = params.height;
				break;

			case ImageType_2D:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D;

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfo.extent.width = params.width;
				createInfo.extent.height = params.height;
				createInfo.extent.depth = 1;
				createInfo.arrayLayers = 1;
				break;

			case ImageType_2D_Array:
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

			case ImageType_3D:
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

			case ImageType_Cubemap:
				NazaraAssert(params.width > 0, "Width must be over zero");
				NazaraAssert(params.height > 0, "Height must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_CUBE;

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

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		copyCommandBuffer->CopyBufferToImage(stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_params.width, m_params.height, m_params.depth);

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

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
			case PixelFormat_BGR8:
			{
				createImage.format = VK_FORMAT_B8G8R8_SRGB;
				createImageView.format = createImage.format;
				break;
			}

			case PixelFormat_BGRA8:
			{
				createImage.format = VK_FORMAT_B8G8R8A8_SRGB;
				createImageView.format = createImage.format;
				break;
			}

			case PixelFormat_L8:
			{
				createImage.format = VK_FORMAT_R8_SRGB;
				createImageView.format = createImage.format;
				createImageView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_A
				};
				break;
			}

			case PixelFormat_LA8:
			{
				createImage.format = VK_FORMAT_R8G8_SRGB;
				createImageView.format = createImage.format;
				createImageView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_G
				};
				break;
			}

			case PixelFormat_RGB8:
			{
				createImage.format = VK_FORMAT_R8G8B8_SRGB;
				createImageView.format = createImage.format;
				break;
			}

			case PixelFormat_RGBA8:
			{
				createImage.format = VK_FORMAT_R8G8B8A8_SRGB;
				createImageView.format = createImage.format;
				break;
			}

			default:
				throw std::runtime_error("Unsupported pixel format " + PixelFormatInfo::GetName(pixelFormat));
		}
	}
}
