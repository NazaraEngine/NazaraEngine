// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <vma/vk_mem_alloc.h>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanTexture::VulkanTexture(Vk::Device& device, const TextureInfo& textureInfo) :
	m_image(VK_NULL_HANDLE),
	m_allocation(nullptr),
	m_device(device),
	m_textureInfo(textureInfo)
	{
		VkImageViewCreateInfo createInfoView = {};
		createInfoView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		InitViewForFormat(textureInfo.pixelFormat, createInfoView);
		
		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.format = createInfoView.format;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = ToVulkan(textureInfo.usageFlags);

		switch (textureInfo.type)
		{
			case ImageType::E1D:
				NazaraAssert(textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(textureInfo.height == 1, "Height must be one");
				NazaraAssert(textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(textureInfo.layerCount == 1, "Array count must be one");

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D;
				break;

			case ImageType::E1D_Array:
				NazaraAssert(textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(textureInfo.height == 1, "Height must be one");
				NazaraAssert(textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(textureInfo.layerCount > 0, "Array count must be over zero");

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				break;

			case ImageType::E2D:
				NazaraAssert(textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(textureInfo.layerCount == 1, "Array count must be one");

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				break;

			case ImageType::E2D_Array:
				NazaraAssert(textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(textureInfo.layerCount > 0, "Array count must be over zero");

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				break;

			case ImageType::E3D:
				NazaraAssert(textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(textureInfo.depth > 0, "Depth must be over zero");
				NazaraAssert(textureInfo.layerCount == 1, "Array count must be one");

				createInfo.imageType = VK_IMAGE_TYPE_3D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_3D;
				break;

			case ImageType::Cubemap:
				NazaraAssert(textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(textureInfo.layerCount > 0 && textureInfo.layerCount % 6 == 0, "Array count must be a multiple of 6");

				createInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				break;
		}

		createInfo.extent.width = textureInfo.width;
		createInfo.extent.height = textureInfo.height;
		createInfo.extent.depth = textureInfo.depth;
		createInfo.arrayLayers = textureInfo.layerCount;
		createInfo.mipLevels = textureInfo.levelCount;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkResult result = vmaCreateImage(m_device.GetMemoryAllocator(), &createInfo, &allocInfo, &m_image, &m_allocation, nullptr);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate image: " + TranslateVulkanError(result));

		CallOnExit releaseImage([&]{ vmaDestroyImage(m_device.GetMemoryAllocator(), m_image, m_allocation); });

		// Create default view (viewing the whole texture)
		m_imageRange = {
			ToVulkan(PixelFormatInfo::GetContent(textureInfo.pixelFormat)),
			0,                      //< baseMipLevel
			createInfo.mipLevels,   //< levelCount
			0,                      //< baseArrayLayer
			createInfo.arrayLayers  //< layerCount
		};

		createInfoView.image = m_image;
		createInfoView.subresourceRange = m_imageRange;

		if (!m_imageView.Create(m_device, createInfoView))
			throw std::runtime_error("Failed to create default image view: " + TranslateVulkanError(m_imageView.GetLastErrorCode()));

		releaseImage.Reset();
	}

	VulkanTexture::VulkanTexture(std::shared_ptr<VulkanTexture> parentTexture, const TextureViewInfo& viewInfo) :
	m_parentTexture(std::move(parentTexture)),
	m_image(m_parentTexture->m_image),
	m_allocation(nullptr),
	m_device(m_parentTexture->m_device)
	{
		m_textureInfo = ApplyView(m_parentTexture->m_textureInfo, viewInfo);

		NazaraAssert(viewInfo.layerCount <= m_parentTexture->m_textureInfo.layerCount - viewInfo.baseArrayLayer, "layer count exceeds number of layers");
		NazaraAssert(viewInfo.levelCount <= m_parentTexture->m_textureInfo.levelCount - viewInfo.baseMipLevel, "level count exceeds number of levels");

		m_viewInfo = viewInfo;
		m_imageRange = {
			ToVulkan(PixelFormatInfo::GetContent(viewInfo.reinterpretFormat)),
			viewInfo.baseMipLevel,
			viewInfo.levelCount,
			viewInfo.baseArrayLayer,
			viewInfo.layerCount
		};

		VkImageViewCreateInfo createInfoView = {};
		createInfoView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfoView.image = m_image;
		createInfoView.subresourceRange = m_imageRange;
		
		switch (m_textureInfo.type)
		{
			case ImageType::E1D:
				NazaraAssert(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(m_textureInfo.height == 1, "Height must be one");
				NazaraAssert(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(m_textureInfo.layerCount == 1, "Array count must be one");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D;
				break;

			case ImageType::E1D_Array:
				NazaraAssert(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(m_textureInfo.height == 1, "Height must be one");
				NazaraAssert(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(m_textureInfo.layerCount > 0, "Array count must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				break;

			case ImageType::E2D:
				NazaraAssert(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(m_textureInfo.layerCount == 1, "Array count must be one");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				break;

			case ImageType::E2D_Array:
				NazaraAssert(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(m_textureInfo.layerCount > 0, "Array count must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				break;

			case ImageType::E3D:
				NazaraAssert(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(m_textureInfo.depth > 0, "Depth must be over zero");
				NazaraAssert(m_textureInfo.layerCount == 1, "Array count must be one");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_3D;
				break;

			case ImageType::Cubemap:
				NazaraAssert(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssert(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssert(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssert(m_textureInfo.layerCount > 0 && m_textureInfo.layerCount % 6 == 0, "Array count must be a multiple of 6");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				break;

			default:
				break;
		}

		InitViewForFormat(viewInfo.reinterpretFormat, createInfoView);

		if (!m_imageView.Create(m_device, createInfoView))
			throw std::runtime_error("Failed to create image view: " + TranslateVulkanError(m_imageView.GetLastErrorCode()));
	}

	VulkanTexture::~VulkanTexture()
	{
		if (m_allocation)
			vmaDestroyImage(m_device.GetMemoryAllocator(), m_image, m_allocation);
	}

	bool VulkanTexture::Copy(const Texture& source, const Boxui& srcBox, const Vector3ui& dstPos)
	{
		const VulkanTexture& sourceTexture = static_cast<const VulkanTexture&>(source);

		Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Graphics);
		if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			return false;

		VkImageSubresourceLayers subresourceLayers = { //< FIXME
			VK_IMAGE_ASPECT_COLOR_BIT,
			0, //< mipLevel
			0, //< baseArrayLayer
			1  //< layerCount
		};

		VkImageSubresourceRange subresourceRange = { //< FIXME
			VK_IMAGE_ASPECT_COLOR_BIT,
			0, //< baseMipLevel
			1, //< levelCount
			subresourceLayers.baseArrayLayer, //< baseArrayLayer
			subresourceLayers.layerCount      //< layerCount
		};

		VkImageCopy region = {
			subresourceLayers,
			VkOffset3D { static_cast<Int32>(srcBox.x), static_cast<Int32>(srcBox.y), static_cast<Int32>(srcBox.z) },
			subresourceLayers,
			VkOffset3D { static_cast<Int32>(dstPos.x), static_cast<Int32>(dstPos.y), static_cast<Int32>(dstPos.z) },
			VkExtent3D { static_cast<UInt32>(srcBox.width), static_cast<UInt32>(srcBox.height), static_cast<UInt32>(srcBox.depth) }
		};

		copyCommandBuffer->SetImageLayout(sourceTexture.GetImage(), VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, subresourceRange);
		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

		copyCommandBuffer->CopyImage(sourceTexture.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region);

		copyCommandBuffer->SetImageLayout(sourceTexture.GetImage(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

		if (!copyCommandBuffer->End())
			return false;

		Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetDefaultFamilyIndex(QueueType::Graphics), 0);
		if (!transferQueue.Submit(copyCommandBuffer))
			return false;

		transferQueue.WaitIdle();

		return true;
	}

	std::shared_ptr<Texture> VulkanTexture::CreateView(const TextureViewInfo& viewInfo)
	{
		if (m_parentTexture)
		{
			assert(m_viewInfo);
			NazaraAssert(viewInfo.layerCount <= m_viewInfo->layerCount - viewInfo.baseArrayLayer, "layer count exceeds number of layers");
			NazaraAssert(viewInfo.levelCount <= m_viewInfo->levelCount - viewInfo.baseMipLevel, "level count exceeds number of levels");

			TextureViewInfo ajustedView = viewInfo;
			ajustedView.baseArrayLayer += m_viewInfo->baseArrayLayer;
			ajustedView.baseMipLevel += m_viewInfo->baseMipLevel;

			return m_parentTexture->CreateView(ajustedView);
		}

		return std::make_shared<VulkanTexture>(std::static_pointer_cast<VulkanTexture>(shared_from_this()), viewInfo);
	}

	PixelFormat VulkanTexture::GetFormat() const
	{
		return m_textureInfo.pixelFormat;
	}

	UInt8 VulkanTexture::GetLevelCount() const
	{
		return m_textureInfo.levelCount;
	}

	VulkanTexture* VulkanTexture::GetParentTexture() const
	{
		return m_parentTexture.get();
	}

	Vector3ui VulkanTexture::GetSize(UInt8 level) const
	{
		return Vector3ui(GetLevelSize(m_textureInfo.width, level), GetLevelSize(m_textureInfo.height, level), GetLevelSize(m_textureInfo.depth, level));
	}

	ImageType VulkanTexture::GetType() const
	{
		return m_textureInfo.type;
	}

	bool VulkanTexture::Update(const void* ptr, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		std::size_t textureSize = box.width * box.height * box.depth * PixelFormatInfo::GetBytesPerPixel(m_textureInfo.pixelFormat);
		if (m_textureInfo.type == ImageType::Cubemap)
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

		if (srcWidth == 0)
			srcWidth = box.width;

		if (srcHeight == 0)
			srcHeight = box.height;

		if (srcWidth == box.width && srcHeight == box.height)
			std::memcpy(allocationInfo.pMappedData, ptr, textureSize);
		else
		{
			unsigned int dstWidth = box.width;
			unsigned int dstHeight = box.height;

			unsigned int bpp = PixelFormatInfo::GetBytesPerPixel(m_textureInfo.pixelFormat);
			unsigned int lineStride = box.width * bpp;
			unsigned int dstLineStride = dstWidth * bpp;
			unsigned int dstFaceStride = dstLineStride * dstHeight;
			unsigned int srcLineStride = srcWidth * bpp;
			unsigned int srcFaceStride = srcLineStride * srcHeight;

			const UInt8* source = static_cast<const UInt8*>(ptr);
			UInt8* destination = static_cast<UInt8*>(allocationInfo.pMappedData);
			for (unsigned int i = 0; i < box.depth; ++i)
			{
				UInt8* dstFacePtr = destination;
				const UInt8* srcFacePtr = source;
				for (unsigned int y = 0; y < box.height; ++y)
				{
					std::memcpy(dstFacePtr, srcFacePtr, lineStride);

					dstFacePtr += dstLineStride;
					srcFacePtr += srcLineStride;
				}

				destination += dstFaceStride;
				source += srcFaceStride;
			}
		}

		Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Graphics);
		if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			return false;

		VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		if (PixelFormatInfo::GetContent(m_textureInfo.pixelFormat) == PixelFormatContent::Depth)
			aspect = VK_IMAGE_ASPECT_DEPTH_BIT;

		VkImageSubresourceLayers subresourceLayers = { //< FIXME
			aspect,
			level, //< mipLevel
			m_imageRange.baseArrayLayer, //< baseArrayLayer
			UInt32((m_textureInfo.type == ImageType::Cubemap) ? 6 : 1) //< layerCount
		};

		VkImageSubresourceRange subresourceRange = { //< FIXME
			aspect,
			m_imageRange.baseMipLevel, //< baseMipLevel
			1, //< levelCount
			subresourceLayers.baseArrayLayer, //< baseArrayLayer
			subresourceLayers.layerCount      //< layerCount
		};

		VkBufferImageCopy region = {
			0,
			0,
			0,
			subresourceLayers,
			{ // imageOffset
				SafeCast<Int32>(box.x), SafeCast<Int32>(box.y), SafeCast<Int32>(box.z)
			},
			{ // imageExtent
				box.width, box.height, box.depth
			}
		};

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

		copyCommandBuffer->CopyBufferToImage(stagingBuffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region);

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

		if (!copyCommandBuffer->End())
			return false;

		Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetDefaultFamilyIndex(QueueType::Graphics), 0);
		if (!transferQueue.Submit(copyCommandBuffer))
			return false;

		transferQueue.WaitIdle();

		return true;
	}

	void VulkanTexture::UpdateDebugName(std::string_view name)
	{
		return m_device.SetDebugName(VK_OBJECT_TYPE_IMAGE, static_cast<UInt64>(reinterpret_cast<std::uintptr_t>(m_image)), name);
	}

	void VulkanTexture::InitViewForFormat(PixelFormat pixelFormat, VkImageViewCreateInfo& createImageView)
	{
		// TODO: Fill this switch
		switch (pixelFormat)
		{
			// Regular formats
			case PixelFormat::BGR8:
			case PixelFormat::BGR8_SRGB:
			case PixelFormat::BGRA8:
			case PixelFormat::BGRA8_SRGB:
			case PixelFormat::Depth16:
			case PixelFormat::Depth16Stencil8:
			case PixelFormat::Depth24Stencil8:
			case PixelFormat::Depth32F:
			case PixelFormat::Depth32FStencil8:
			case PixelFormat::RGB8:
			case PixelFormat::RGB8_SRGB:
			case PixelFormat::RGBA8:
			case PixelFormat::RGBA8_SRGB:
			case PixelFormat::RGBA16F:
			case PixelFormat::RGBA32F:
			{
				createImageView.format = ToVulkan(pixelFormat);
				createImageView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_G,
					VK_COMPONENT_SWIZZLE_B,
					VK_COMPONENT_SWIZZLE_A
				};

				break;
			}

			// "emulated" formats
			case PixelFormat::A8:
			{
				createImageView.format = VK_FORMAT_R8_UNORM;
				createImageView.components = {
					VK_COMPONENT_SWIZZLE_ONE,
					VK_COMPONENT_SWIZZLE_ONE,
					VK_COMPONENT_SWIZZLE_ONE,
					VK_COMPONENT_SWIZZLE_R
				};
				break;
			}

			case PixelFormat::L8:
			{
				createImageView.format = VK_FORMAT_R8_UNORM;
				createImageView.components = {
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_R,
					VK_COMPONENT_SWIZZLE_ONE
				};
				break;
			}

			case PixelFormat::LA8:
			{
				createImageView.format = VK_FORMAT_R8G8_UNORM;
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

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
#include <Nazara/Core/AntiX11.hpp>
#endif
