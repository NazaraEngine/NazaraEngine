// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanComputePipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/VulkanSwapchain.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/VulkanTextureFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanUploadPool.hpp>
#include <Nazara/VulkanRenderer/VulkanWindowFramebuffer.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/StackArray.hpp>

namespace Nz
{
	void VulkanCommandBufferBuilder::BeginDebugRegion(std::string_view regionName, const Color& color)
	{
		// Ensure \0 at the end of string
		StackArray<char> regionNameEOS = NazaraStackArrayNoInit(char, regionName.size() + 1);
		std::memcpy(regionNameEOS.data(), regionName.data(), regionName.size());
		regionNameEOS[regionName.size()] = '\0';

		m_commandBuffer.BeginDebugRegion(regionNameEOS.data(), color);
	}

	void VulkanCommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect, const ClearValues* clearValues, std::size_t clearValueCount)
	{
		const VulkanRenderPass& vkRenderPass = SafeCast<const VulkanRenderPass&>(renderPass);
		const VulkanFramebuffer& vkFramebuffer = SafeCast<const VulkanFramebuffer&>(framebuffer);

		std::size_t attachmentCount = vkRenderPass.GetAttachmentCount();

		StackArray<VkClearValue> vkClearValues;

		if (clearValueCount > 0)
		{
			vkClearValues = NazaraStackArray(VkClearValue, attachmentCount);
			for (std::size_t i = 0; i < attachmentCount; ++i)
			{
				const auto& values = (i < clearValueCount) ? clearValues[i] : CommandBufferBuilder::ClearValues{};
				auto& vkValues = vkClearValues[i];

				if (PixelFormatInfo::GetContent(vkRenderPass.GetAttachment(i).format) == PixelFormatContent::ColorRGBA)
				{
					vkValues.color.float32[0] = values.color.r;
					vkValues.color.float32[1] = values.color.g;
					vkValues.color.float32[2] = values.color.b;
					vkValues.color.float32[3] = values.color.a;
				}
				else
				{
					vkValues.depthStencil.depth = values.depth;
					vkValues.depthStencil.stencil = values.stencil;
				}
			}
		}

		VkRenderPassBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		beginInfo.renderPass = vkRenderPass.GetRenderPass();
		beginInfo.framebuffer = vkFramebuffer.GetFramebuffer();
		beginInfo.renderArea.offset.x = renderRect.x;
		beginInfo.renderArea.offset.y = renderRect.y;
		beginInfo.renderArea.extent.width = renderRect.width;
		beginInfo.renderArea.extent.height = renderRect.height;
		beginInfo.clearValueCount = UInt32(vkClearValues.size());
		beginInfo.pClearValues = vkClearValues.data();

		m_commandBuffer.BeginRenderPass(beginInfo);

		m_currentRenderPass = &vkRenderPass;
		m_currentSubpassIndex = 0;
	}

	void VulkanCommandBufferBuilder::BindComputePipeline(const ComputePipeline& pipeline)
	{
		const VulkanComputePipeline& vkPipeline = SafeCast<const VulkanComputePipeline&>(pipeline);

		m_commandBuffer.BindPipeline(VK_PIPELINE_BIND_POINT_COMPUTE, vkPipeline.GetPipeline());
	}

	void VulkanCommandBufferBuilder::BindComputeShaderBinding(UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets)
	{
		const VulkanShaderBinding& vkBinding = SafeCast<const VulkanShaderBinding&>(binding);
		const VulkanRenderPipelineLayout& pipelineLayout = vkBinding.GetOwner();

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout.GetPipelineLayout(), set, vkBinding.GetDescriptorSet(), SafeCaster(dynamicOffsets.size()), dynamicOffsets.data());
	}

	void VulkanCommandBufferBuilder::BindComputeShaderBinding(const RenderPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets)
	{
		const VulkanRenderPipelineLayout& vkPipelineLayout = SafeCast<const VulkanRenderPipelineLayout&>(pipelineLayout);
		const VulkanShaderBinding& vkBinding = SafeCast<const VulkanShaderBinding&>(binding);

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_COMPUTE, vkPipelineLayout.GetPipelineLayout(), set, vkBinding.GetDescriptorSet(), SafeCaster(dynamicOffsets.size()), dynamicOffsets.data());
	}

	void VulkanCommandBufferBuilder::BindIndexBuffer(const RenderBuffer& indexBuffer, IndexType indexType, UInt64 offset)
	{
		const VulkanBuffer& vkBuffer = SafeCast<const VulkanBuffer&>(indexBuffer);

		m_commandBuffer.BindIndexBuffer(vkBuffer.GetBuffer(), offset, ToVulkan(indexType));
	}

	void VulkanCommandBufferBuilder::BindRenderPipeline(const RenderPipeline& pipeline)
	{
		if (!m_currentRenderPass)
			throw std::runtime_error("BindPipeline must be called in a RenderPass");

		const VulkanRenderPipeline& vkPipeline = SafeCast<const VulkanRenderPipeline&>(pipeline);

		m_commandBuffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline.Get(*m_currentRenderPass, m_currentSubpassIndex));
	}

	void VulkanCommandBufferBuilder::BindRenderShaderBinding(UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets)
	{
		const VulkanShaderBinding& vkBinding = SafeCast<const VulkanShaderBinding&>(binding);
		const VulkanRenderPipelineLayout& pipelineLayout = vkBinding.GetOwner();

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.GetPipelineLayout(), set, vkBinding.GetDescriptorSet(), SafeCaster(dynamicOffsets.size()), dynamicOffsets.data());
	}

	void VulkanCommandBufferBuilder::BindRenderShaderBinding(const RenderPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets)
	{
		const VulkanRenderPipelineLayout& vkPipelineLayout = SafeCast<const VulkanRenderPipelineLayout&>(pipelineLayout);
		const VulkanShaderBinding& vkBinding = SafeCast<const VulkanShaderBinding&>(binding);

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout.GetPipelineLayout(), set, vkBinding.GetDescriptorSet(), SafeCaster(dynamicOffsets.size()), dynamicOffsets.data());
	}

	void VulkanCommandBufferBuilder::BindVertexBuffer(UInt32 binding, const RenderBuffer& vertexBuffer, UInt64 offset)
	{
		const VulkanBuffer& vkBuffer = SafeCast<const VulkanBuffer&>(vertexBuffer);

		m_commandBuffer.BindVertexBuffer(binding, vkBuffer.GetBuffer(), offset);
	}

	void VulkanCommandBufferBuilder::BlitTexture(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Texture& toTexture, const Boxui& toBox, TextureLayout toLayout, SamplerFilter filter)
	{
		const VulkanTexture& vkFromTexture = SafeCast<const VulkanTexture&>(fromTexture);
		const VulkanTexture& vkToTexture = SafeCast<const VulkanTexture&>(toTexture);

		unsigned int fromBaseLayer, fromLayerCount;
		ImageUtils::RegionToArray(vkFromTexture.GetType(), fromBox, fromBaseLayer, fromLayerCount);

		unsigned int toBaseLayer, toLayerCount;
		ImageUtils::RegionToArray(vkToTexture.GetType(), toBox, toBaseLayer, toLayerCount);

		VkImageBlit region = {
			vkFromTexture.BuildSubresourceLayers(0, fromBaseLayer, fromLayerCount),
			{
				{
					SafeCast<Int32>(fromBox.x),
					SafeCast<Int32>(fromBox.y),
					SafeCast<Int32>(fromBox.z)
				},
				{
					SafeCast<Int32>(fromBox.x + fromBox.width),
					SafeCast<Int32>(fromBox.y + fromBox.height),
					SafeCast<Int32>(fromBox.z + fromBox.depth)
				}
			},
			vkToTexture.BuildSubresourceLayers(0, toBaseLayer, toLayerCount),
			{
				{
					SafeCast<Int32>(toBox.x),
					SafeCast<Int32>(toBox.y),
					SafeCast<Int32>(toBox.z)
				},
				{
					SafeCast<Int32>(toBox.x + toBox.width),
					SafeCast<Int32>(toBox.y + toBox.height),
					SafeCast<Int32>(toBox.z + toBox.depth)
				}
			},
		};

		m_commandBuffer.BlitImage(vkFromTexture.GetImage(), ToVulkan(fromLayout), vkToTexture.GetImage(), ToVulkan(toLayout), region, ToVulkan(filter));
	}

	void VulkanCommandBufferBuilder::BlitTextureToSwapchain(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Swapchain& swapchain, std::size_t imageIndex)
	{
		const VulkanTexture& vkFromTexture = SafeCast<const VulkanTexture&>(fromTexture);
		const VulkanSwapchain& vkSwapchain = SafeCast<const VulkanSwapchain&>(swapchain);

		VkImage swapchainImage = vkSwapchain.GetImage(imageIndex);

		VkImageSubresourceRange swapchainImageRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		};

		Boxi fromBoxInt = Boxi(fromBox);
		Vector2i swapchainSize = Vector2i(vkSwapchain.GetSize());

		VkImageBlit swapchainBlit = {
			.srcSubresource = vkFromTexture.BuildSubresourceLayers(0),
			.srcOffsets = {
				{ fromBoxInt.x, fromBoxInt.y, fromBoxInt.z },
				{ fromBoxInt.x + fromBoxInt.width, fromBoxInt.y + fromBoxInt.height, fromBoxInt.z + fromBoxInt.depth }
			},
			.dstSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.dstOffsets = {
				{ 0, 0, 0 },
				{ swapchainSize.x, swapchainSize.y, 1 }
			},
		};

		m_commandBuffer.SetImageLayout(swapchainImage, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, swapchainImageRange);

		m_commandBuffer.BlitImage(vkFromTexture.GetImage(), ToVulkan(fromLayout), vkSwapchain.GetImage(imageIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, swapchainBlit, VK_FILTER_LINEAR);

		m_commandBuffer.SetImageLayout(swapchainImage, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, swapchainImageRange);
	}

	void VulkanCommandBufferBuilder::BuildMipmaps(Texture& texture, UInt8 baseLevel, UInt8 levelCount, PipelineStageFlags srcStageMask, PipelineStageFlags dstStageMask, MemoryAccessFlags srcAccessMask, MemoryAccessFlags dstAccessMask, TextureLayout oldLayout, TextureLayout newLayout)
	{
		VulkanTexture& vkTexture = SafeCast<VulkanTexture&>(texture);
		VkImage vkImage = vkTexture.GetImage();

		const TextureInfo& textureInfo = vkTexture.GetTextureInfo();

		levelCount = std::min(levelCount, textureInfo.levelCount);

		Vector3i32 mipSize(SafeCast<Int32>(textureInfo.width), SafeCast<Int32>(textureInfo.height), SafeCast<Int32>(textureInfo.depth));
		Vector3i32 prevMipSize = mipSize;

		if (baseLevel != 0)
		{
			mipSize.x >>= baseLevel;
			mipSize.y >>= baseLevel;
			mipSize.z >>= baseLevel;
			mipSize.Maximize({ 1, 1, 1 });
		}

		// Transition all mips to transfer dst, except for the base level
		m_commandBuffer.ImageBarrier(ToVulkan(srcStageMask), VK_PIPELINE_STAGE_TRANSFER_BIT, 0, ToVulkan(srcAccessMask), VK_ACCESS_TRANSFER_READ_BIT, ToVulkan(oldLayout), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImage, vkTexture.BuildSubresourceRange(baseLevel, 1));
		m_commandBuffer.ImageBarrier(ToVulkan(srcStageMask), VK_PIPELINE_STAGE_TRANSFER_BIT, 0, ToVulkan(srcAccessMask), VK_ACCESS_TRANSFER_WRITE_BIT, ToVulkan(oldLayout), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, vkImage, vkTexture.BuildSubresourceRange(baseLevel + 1, levelCount - 1));

		for (UInt8 i = 1; i < levelCount; ++i)
		{
			mipSize /= 2;
			mipSize.Maximize({ 1, 1, 1 });

			// Transition previous mip to transfer src, as it will serve as a source for the next blit (base mip is already in transfer src)
			if (i != 1)
			{
				VkImageSubresourceRange prevMipmapRange = vkTexture.BuildSubresourceRange(baseLevel + i - 1, 1);
				m_commandBuffer.ImageBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImage, prevMipmapRange);
			}

			// Blit previous mipmap to next mipmap
			VkImageBlit blitRegion = {
				vkTexture.BuildSubresourceLayers(baseLevel + i - 1),
				{ //< srcOffsets
					{ 0, 0, 0 },
					{ prevMipSize.x, prevMipSize.y, prevMipSize.z }
				},
				vkTexture.BuildSubresourceLayers(baseLevel + i),
				{ //< dstOffsets
					{ 0, 0, 0 },
					{ mipSize.x, mipSize.y, mipSize.z }
				},
			};

			m_commandBuffer.BlitImage(vkImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, blitRegion, VK_FILTER_LINEAR);

			prevMipSize = mipSize;
		}

		// Transition all mips (which are now in transfer src, except for the last one which is still in transfer dst) to the target layout
		m_commandBuffer.ImageBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, ToVulkan(dstStageMask), 0, VK_ACCESS_TRANSFER_READ_BIT, ToVulkan(dstAccessMask), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, ToVulkan(newLayout), vkImage, vkTexture.BuildSubresourceRange(baseLevel, levelCount - 1));
		m_commandBuffer.ImageBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, ToVulkan(dstStageMask), 0, VK_ACCESS_TRANSFER_WRITE_BIT, ToVulkan(dstAccessMask), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, ToVulkan(newLayout), vkImage, vkTexture.BuildSubresourceRange(levelCount - 1, 1));
	}

	void VulkanCommandBufferBuilder::CopyBuffer(const RenderBufferView& source, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		VulkanBuffer& sourceBuffer = *SafeCast<VulkanBuffer*>(source.GetBuffer());
		VulkanBuffer& targetBuffer = *SafeCast<VulkanBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(sourceBuffer.GetBuffer(), targetBuffer.GetBuffer(), size, sourceOffset + source.GetOffset(), targetOffset + target.GetOffset());
	}

	void VulkanCommandBufferBuilder::CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		const auto& vkAllocation = SafeCast<const VulkanUploadPool::VulkanAllocation&>(allocation);
		VulkanBuffer& targetBuffer = *SafeCast<VulkanBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(vkAllocation.buffer, targetBuffer.GetBuffer(), size, vkAllocation.offset + sourceOffset, target.GetOffset() + targetOffset);
	}

	void VulkanCommandBufferBuilder::CopyTexture(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Texture& toTexture, const Vector3ui& toPos, TextureLayout toLayout)
	{
		const VulkanTexture& vkFromTexture = SafeCast<const VulkanTexture&>(fromTexture);
		const VulkanTexture& vkToTexture = SafeCast<const VulkanTexture&>(toTexture);

		unsigned int fromBaseLayer, fromLayerCount;
		ImageUtils::RegionToArray(vkFromTexture.GetType(), fromBox, fromBaseLayer, fromLayerCount);

		unsigned int toBaseLayer, toLayerCount;
		ImageUtils::RegionToArray(vkToTexture.GetType(), Boxui(toPos.x, toPos.y, toPos.z, fromBox.width, fromBox.height, fromBox.depth), toBaseLayer, toLayerCount);

		VkImageCopy region = {
			vkFromTexture.BuildSubresourceLayers(0, fromBaseLayer, fromLayerCount),
			{
				SafeCast<Int32>(fromBox.x),
				SafeCast<Int32>(fromBox.y),
				SafeCast<Int32>(fromBox.z)
			},
			vkToTexture.BuildSubresourceLayers(0, toBaseLayer, toLayerCount),
			{
				SafeCast<Int32>(toPos.x),
				SafeCast<Int32>(toPos.y),
				SafeCast<Int32>(toPos.z),
			},
			{
				SafeCast<UInt32>(fromBox.width),
				SafeCast<UInt32>(fromBox.height),
				SafeCast<UInt32>(fromBox.depth)
			}
		};

		m_commandBuffer.CopyImage(vkFromTexture.GetImage(), ToVulkan(fromLayout), vkToTexture.GetImage(), ToVulkan(toLayout), region);
	}

	void VulkanCommandBufferBuilder::Dispatch(UInt32 workgroupX, UInt32 workgroupY, UInt32 workgroupZ)
	{
		m_commandBuffer.Dispatch(workgroupX, workgroupY, workgroupZ);
	}

	void VulkanCommandBufferBuilder::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		m_commandBuffer.Draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void VulkanCommandBufferBuilder::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstIndex, UInt32 vertexOffset, UInt32 firstInstance)
	{
		m_commandBuffer.DrawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void VulkanCommandBufferBuilder::EndDebugRegion()
	{
		m_commandBuffer.EndDebugRegion();
	}

	void VulkanCommandBufferBuilder::EndRenderPass()
	{
		m_commandBuffer.EndRenderPass();
		m_currentRenderPass = nullptr;
	}

	void VulkanCommandBufferBuilder::InsertDebugLabel(std::string_view label, const Color& color)
	{
		// Ensure \0 at the end of string
		StackArray<char> labelEOS = NazaraStackArrayNoInit(char, label.size() + 1);
		std::memcpy(labelEOS.data(), label.data(), label.size());
		labelEOS[label.size()] = '\0';

		m_commandBuffer.InsertDebugLabel(labelEOS.data(), color);
	}

	void VulkanCommandBufferBuilder::MemoryBarrier(PipelineStageFlags srcStageMask, PipelineStageFlags dstStageMask, MemoryAccessFlags srcAccessMask, MemoryAccessFlags dstAccessMask)
	{
		m_commandBuffer.MemoryBarrier(ToVulkan(srcStageMask), ToVulkan(dstStageMask), ToVulkan(srcAccessMask), ToVulkan(dstAccessMask));
	}

	void VulkanCommandBufferBuilder::NextSubpass()
	{
		m_commandBuffer.NextSubpass();
		m_currentSubpassIndex++;
	}

	void VulkanCommandBufferBuilder::PreTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0U, VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT);
	}

	void VulkanCommandBufferBuilder::PostTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT);
	}

	void VulkanCommandBufferBuilder::PushConstants(const RenderPipelineLayout& pipelineLayout, UInt32 offset, UInt32 size, const void* data)
	{
		const VulkanRenderPipelineLayout& vkPipelineLayout = SafeCast<const VulkanRenderPipelineLayout&>(pipelineLayout);

		m_commandBuffer.PushConstants(vkPipelineLayout.GetPipelineLayout(), VK_SHADER_STAGE_ALL, offset, size, data);
	}

	void VulkanCommandBufferBuilder::SetScissor(const Recti& scissorRegion)
	{
		m_commandBuffer.SetScissor(scissorRegion);
	}

	void VulkanCommandBufferBuilder::SetViewport(const Recti& viewportRegion)
	{
		m_commandBuffer.SetViewport(Rectf(viewportRegion), 0.f, 1.f);
	}

	void VulkanCommandBufferBuilder::TextureBarrier(PipelineStageFlags srcStageMask, PipelineStageFlags dstStageMask, MemoryAccessFlags srcAccessMask, MemoryAccessFlags dstAccessMask, TextureLayout oldLayout, TextureLayout newLayout, const Texture& texture)
	{
		const VulkanTexture& vkTexture = SafeCast<const VulkanTexture&>(texture);

		m_commandBuffer.ImageBarrier(ToVulkan(srcStageMask), ToVulkan(dstStageMask), VkDependencyFlags(0), ToVulkan(srcAccessMask), ToVulkan(dstAccessMask), ToVulkan(oldLayout), ToVulkan(newLayout), vkTexture.GetImage(), vkTexture.GetSubresourceRange());
	}
}
