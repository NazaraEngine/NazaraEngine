// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/Core/ImageUtils.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/QueueHandle.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <vk_mem_alloc.h>
#include <stdexcept>

namespace Nz
{
	VulkanTexture::VulkanTexture(VulkanDevice& device, const TextureInfo& textureInfo) :
	m_device(device),
	m_image(VK_NULL_HANDLE),
	m_allocation(nullptr),
	m_textureInfo(textureInfo)
	{
		m_textureInfo.levelCount = std::min(m_textureInfo.levelCount, ImageUtils::GetMaxLevel(m_textureInfo.type, m_textureInfo.width, m_textureInfo.height, m_textureInfo.depth));
		m_textureViewInfo = m_textureInfo;

		VkImageViewCreateInfo createInfoView = {};
		createInfoView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		InitViewForFormat(m_textureInfo.pixelFormat, createInfoView);

		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.format = createInfoView.format;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = ToVulkan(m_textureInfo.usageFlags);

		switch (m_textureInfo.type)
		{
			case ImageType::E1D:
				NazaraAssertMsg(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureInfo.height == 1, "Height must be one");
				NazaraAssertMsg(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureInfo.layerCount == 1, "Array count must be one");

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D;
				break;

			case ImageType::E1D_Array:
				NazaraAssertMsg(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureInfo.height == 1, "Height must be one");
				NazaraAssertMsg(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureInfo.layerCount > 0, "Array count must be over zero");

				createInfo.imageType = VK_IMAGE_TYPE_1D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				break;

			case ImageType::E2D:
				NazaraAssertMsg(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureInfo.layerCount == 1, "Array count must be one");

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				break;

			case ImageType::E2D_Array:
				NazaraAssertMsg(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureInfo.layerCount > 0, "Array count must be over zero");

				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				break;

			case ImageType::E3D:
				NazaraAssertMsg(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureInfo.depth > 0, "Depth must be over zero");
				NazaraAssertMsg(m_textureInfo.layerCount == 1, "Array count must be one");

				createInfo.imageType = VK_IMAGE_TYPE_3D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_3D;
				break;

			case ImageType::Cubemap:
				NazaraAssertMsg(m_textureInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureInfo.layerCount > 0 && m_textureInfo.layerCount % 6 == 0, "Array count must be a multiple of 6");

				createInfo.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
				createInfo.imageType = VK_IMAGE_TYPE_2D;
				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				break;
		}

		createInfo.extent.width = m_textureInfo.width;
		createInfo.extent.height = m_textureInfo.height;
		createInfo.extent.depth = m_textureInfo.depth;
		createInfo.arrayLayers = m_textureInfo.layerCount;
		createInfo.mipLevels = m_textureInfo.levelCount;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkResult result = vmaCreateImage(m_device.GetMemoryAllocator(), &createInfo, &allocInfo, &m_image, &m_allocation, nullptr);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate image: " + TranslateVulkanError(result));

		CallOnExit releaseImage([&]{ vmaDestroyImage(m_device.GetMemoryAllocator(), m_image, m_allocation); });

		// Create default view (viewing the whole texture)
		m_subresourceRange = {
			ToVulkan(PixelFormatInfo::GetContent(m_textureInfo.pixelFormat)),
			0,                      //< baseMipLevel
			createInfo.mipLevels,   //< levelCount
			0,                      //< baseArrayLayer
			createInfo.arrayLayers  //< layerCount
		};

		createInfoView.image = m_image;
		createInfoView.subresourceRange = m_subresourceRange;

		if (!m_imageView.Create(m_device, createInfoView))
			throw std::runtime_error("Failed to create default image view: " + TranslateVulkanError(m_imageView.GetLastErrorCode()));

		releaseImage.Reset();
	}

	VulkanTexture::VulkanTexture(VulkanDevice& device, const TextureInfo& textureInfo, const void* initialData, bool buildMipmaps, unsigned int srcWidth, unsigned int srcHeight) :
	VulkanTexture(device, textureInfo)
	{
		NazaraAssertMsg(initialData, "missing initial data");

		Vk::AutoCommandBuffer initCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Graphics);
		if (!initCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			throw std::runtime_error("failed to allocate command buffer");

		initCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, BuildSubresourceRange(0, m_textureInfo.levelCount, 0, m_textureInfo.layerCount));

		std::unique_ptr<VulkanBuffer> uploadBuffer;

		Boxui wholeRegion(0, 0, 0, m_textureInfo.width, m_textureInfo.height, m_textureInfo.depth);
		ImageUtils::ArrayToRegion(m_textureInfo.type, 0, m_textureInfo.layerCount, wholeRegion);

		Update(initCommandBuffer, uploadBuffer, initialData, wholeRegion, srcWidth, srcHeight, 0);

		if (buildMipmaps && m_textureInfo.levelCount > 1)
		{
			Vector3i32 mipSize(SafeCast<Int32>(m_textureInfo.width), SafeCast<Int32>(m_textureInfo.height), SafeCast<Int32>(m_textureInfo.depth));
			Vector3i32 prevMipSize = mipSize;

			for (UInt32 i = 1; i < m_textureInfo.levelCount; ++i)
			{
				mipSize /= 2;
				mipSize.Maximize({ 1, 1, 1 });

				VkImageBlit blitRegion = {
					BuildSubresourceLayers(i - 1),
					{ //< srcOffsets
						{ 0, 0, 0 },
						{ prevMipSize.x, prevMipSize.y, prevMipSize.z }
					},
					BuildSubresourceLayers(i),
					{ //< dstOffsets
						{ 0, 0, 0 },
						{ mipSize.x, mipSize.y, mipSize.z }
					},
				};

				VkImageSubresourceRange prevMipmapRange = BuildSubresourceRange(i - 1, 1, 0, m_textureInfo.layerCount);

				initCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, prevMipmapRange);

				initCommandBuffer->BlitImage(m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, blitRegion, VK_FILTER_LINEAR);

				initCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, prevMipmapRange);

				prevMipSize = mipSize;
			}

			VkImageSubresourceRange lastMipmapRange = BuildSubresourceRange(m_textureInfo.levelCount - 1, 1, 0, m_textureInfo.layerCount);

			initCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, lastMipmapRange);
		}
		else
		{
			VkImageSubresourceRange subresourceRange = BuildSubresourceRange(0, m_textureInfo.levelCount, 0, m_textureInfo.layerCount);

			initCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);
		}

		if (!initCommandBuffer->End())
			throw std::runtime_error("failed to end command buffer");

		Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetDefaultFamilyIndex(QueueType::Graphics), 0);
		if (!transferQueue.Submit(initCommandBuffer))
			throw std::runtime_error("failed to submit command buffer");

		transferQueue.WaitIdle();
	}

	VulkanTexture::VulkanTexture(std::shared_ptr<VulkanTexture> parentTexture, const TextureViewInfo& viewInfo) :
	m_parentTexture(std::move(parentTexture)),
	m_device(m_parentTexture->m_device),
	m_image(m_parentTexture->m_image),
	m_allocation(nullptr),
	m_textureInfo(m_parentTexture->m_textureInfo)
	{
		m_textureViewInfo = ApplyView(m_parentTexture->m_textureViewInfo, viewInfo);

		NazaraAssertMsg(viewInfo.layerCount <= m_parentTexture->m_textureViewInfo.layerCount - viewInfo.baseArrayLayer, "layer count exceeds number of layers");
		NazaraAssertMsg(viewInfo.levelCount <= m_parentTexture->m_textureViewInfo.levelCount - viewInfo.baseMipLevel, "level count exceeds number of levels");

		m_viewInfo = viewInfo;
		m_subresourceRange = {
			ToVulkan(PixelFormatInfo::GetContent(m_textureViewInfo.pixelFormat)),
			viewInfo.baseMipLevel,
			viewInfo.levelCount,
			viewInfo.baseArrayLayer,
			viewInfo.layerCount
		};

		VkImageViewCreateInfo createInfoView = {};
		createInfoView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfoView.image = m_image;
		createInfoView.subresourceRange = m_subresourceRange;

		switch (m_textureViewInfo.type)
		{
			case ImageType::E1D:
				NazaraAssertMsg(m_textureViewInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureViewInfo.height == 1, "Height must be one");
				NazaraAssertMsg(m_textureViewInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureViewInfo.layerCount == 1, "Array count must be one");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D;
				break;

			case ImageType::E1D_Array:
				NazaraAssertMsg(m_textureViewInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureViewInfo.height == 1, "Height must be one");
				NazaraAssertMsg(m_textureViewInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureViewInfo.layerCount > 0, "Array count must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
				break;

			case ImageType::E2D:
				NazaraAssertMsg(m_textureViewInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureViewInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureViewInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureViewInfo.layerCount == 1, "Array count must be one");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D;
				break;

			case ImageType::E2D_Array:
				NazaraAssertMsg(m_textureViewInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureViewInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureViewInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureViewInfo.layerCount > 0, "Array count must be over zero");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
				break;

			case ImageType::E3D:
				NazaraAssertMsg(m_textureViewInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureViewInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureViewInfo.depth > 0, "Depth must be over zero");
				NazaraAssertMsg(m_textureViewInfo.layerCount == 1, "Array count must be one");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_3D;
				break;

			case ImageType::Cubemap:
				NazaraAssertMsg(m_textureViewInfo.width > 0, "Width must be over zero");
				NazaraAssertMsg(m_textureViewInfo.height > 0, "Height must be over zero");
				NazaraAssertMsg(m_textureViewInfo.depth == 1, "Depth must be one");
				NazaraAssertMsg(m_textureViewInfo.layerCount > 0 && m_textureViewInfo.layerCount % 6 == 0, "Array count must be a multiple of 6");

				createInfoView.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
				break;

			default:
				break;
		}

		InitViewForFormat(m_textureViewInfo.pixelFormat, createInfoView);

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
		const VulkanTexture& sourceTexture = SafeCast<const VulkanTexture&>(source);

		Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Graphics);
		if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			return false;

		VkImageSubresourceLayers srcSubresourceLayers = sourceTexture.BuildSubresourceLayers(0);
		VkImageSubresourceRange srcSubresourceRange = sourceTexture.BuildSubresourceRange(0, 1);
		VkImageSubresourceLayers dstSubresourceLayers = BuildSubresourceLayers(0);
		VkImageSubresourceRange dstSubresourceRange = BuildSubresourceRange(0, 1);

		VkImageCopy region = {
			.srcSubresource = srcSubresourceLayers,
			.srcOffset = VkOffset3D { SafeCast<Int32>(srcBox.x), SafeCast<Int32>(srcBox.y), SafeCast<Int32>(srcBox.z) },
			.dstSubresource = dstSubresourceLayers,
			.dstOffset = VkOffset3D { SafeCast<Int32>(dstPos.x), SafeCast<Int32>(dstPos.y), SafeCast<Int32>(dstPos.z) },
			.extent = VkExtent3D { SafeCast<UInt32>(srcBox.width), SafeCast<UInt32>(srcBox.height), SafeCast<UInt32>(srcBox.depth) }
		};

		copyCommandBuffer->SetImageLayout(sourceTexture.GetImage(), VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, srcSubresourceRange);
		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstSubresourceRange);

		copyCommandBuffer->CopyImage(sourceTexture.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region);

		copyCommandBuffer->SetImageLayout(sourceTexture.GetImage(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, srcSubresourceRange);
		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, dstSubresourceRange);

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
			NazaraAssertMsg(viewInfo.layerCount <= m_viewInfo->layerCount - viewInfo.baseArrayLayer, "layer count exceeds number of layers");
			NazaraAssertMsg(viewInfo.levelCount <= m_viewInfo->levelCount - viewInfo.baseMipLevel, "level count exceeds number of levels");

			TextureViewInfo ajustedView = viewInfo;
			ajustedView.baseArrayLayer += m_viewInfo->baseArrayLayer;
			ajustedView.baseMipLevel += m_viewInfo->baseMipLevel;

			return m_parentTexture->CreateView(ajustedView);
		}

		return std::make_shared<VulkanTexture>(std::static_pointer_cast<VulkanTexture>(shared_from_this()), viewInfo);
	}

	RenderDevice* VulkanTexture::GetDevice()
	{
		return &m_device;
	}

	const RenderDevice* VulkanTexture::GetDevice() const
	{
		return &m_device;
	}

	bool VulkanTexture::Update(const void* ptr, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		Vk::AutoCommandBuffer copyCommandBuffer = m_device.AllocateCommandBuffer(QueueType::Graphics);
		if (!copyCommandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
			return false;

		unsigned int baseLayer, layerCount;
		ImageUtils::RegionToArray(m_textureViewInfo.type, box, baseLayer, layerCount);

		VkImageSubresourceRange subresourceRange = BuildSubresourceRange(level, 1, baseLayer, layerCount);

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresourceRange);

		std::unique_ptr<VulkanBuffer> uploadBuffer;
		Update(copyCommandBuffer, uploadBuffer, ptr, box, srcWidth, srcHeight, level);

		copyCommandBuffer->SetImageLayout(m_image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, subresourceRange);

		if (!copyCommandBuffer->End())
			return false;

		Vk::QueueHandle transferQueue = m_device.GetQueue(m_device.GetDefaultFamilyIndex(QueueType::Graphics), 0);
		if (!transferQueue.Submit(copyCommandBuffer))
			return false;

		transferQueue.WaitIdle();

		return true;
	}

	bool VulkanTexture::Update(Vk::CommandBuffer& commandBuffer, std::unique_ptr<VulkanBuffer>& uploadBuffer, const void* ptr, const Boxui& box, unsigned int srcWidth, unsigned int srcHeight, UInt8 level)
	{
		std::size_t memorySize = box.width * box.height * box.depth * PixelFormatInfo::GetBytesPerPixel(m_textureViewInfo.pixelFormat);

		uploadBuffer = std::make_unique<VulkanBuffer>(m_device, BufferType::Upload, memorySize, BufferUsage::DirectMapping);
		void* mappedUploadBuffer = uploadBuffer->Map(0, memorySize);

		if (srcWidth == 0)
			srcWidth = box.width;

		if (srcHeight == 0)
			srcHeight = box.height;

		if (srcWidth == box.width && srcHeight == box.height)
			std::memcpy(mappedUploadBuffer, ptr, memorySize);
		else
		{
			unsigned int dstWidth = box.width;
			unsigned int dstHeight = box.height;

			unsigned int bpp = PixelFormatInfo::GetBytesPerPixel(m_textureViewInfo.pixelFormat);
			unsigned int lineStride = box.width * bpp;
			unsigned int dstLineStride = dstWidth * bpp;
			unsigned int dstFaceStride = dstLineStride * dstHeight;
			unsigned int srcLineStride = srcWidth * bpp;
			unsigned int srcFaceStride = srcLineStride * srcHeight;

			const UInt8* source = static_cast<const UInt8*>(ptr);
			UInt8* destination = static_cast<UInt8*>(mappedUploadBuffer);
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

		uploadBuffer->Unmap();

		unsigned int baseLayer, layerCount;
		Boxui copyBox = ImageUtils::RegionToArray(m_textureViewInfo.type, box, baseLayer, layerCount);

		VkImageSubresourceLayers subresourceLayers = BuildSubresourceLayers(level, baseLayer, layerCount);

		VkBufferImageCopy region = {
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = subresourceLayers,
			.imageOffset = {
				SafeCast<Int32>(copyBox.x), SafeCast<Int32>(copyBox.y), SafeCast<Int32>(copyBox.z)
			},
			.imageExtent = {
				copyBox.width, copyBox.height, copyBox.depth
			}
		};

		commandBuffer.CopyBufferToImage(uploadBuffer->GetBuffer(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region);

		return true;
	}

	void VulkanTexture::UpdateDebugName(std::string_view name)
	{
		if (!m_parentTexture)
			m_device.SetDebugName(VK_OBJECT_TYPE_IMAGE, VulkanHandleToInteger(m_image), name);

		m_device.SetDebugName(VK_OBJECT_TYPE_IMAGE_VIEW, VulkanHandleToInteger(static_cast<VkImageView>(m_imageView)), name);
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
			case PixelFormat::R8:
			case PixelFormat::RG8:
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
				throw std::runtime_error("Unsupported pixel format " + std::string(PixelFormatInfo::GetName(pixelFormat)));
		}
	}
}

// vma includes vulkan.h which includes system headers
#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
#include <Nazara/Core/AntiX11.hpp>
#endif
