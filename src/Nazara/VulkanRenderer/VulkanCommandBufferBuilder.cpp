// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanWindowFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipeline.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanTextureFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/VulkanUploadPool.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	void VulkanCommandBufferBuilder::BeginDebugRegion(const std::string_view& regionName, const Color& color)
	{
		// Ensure \0 at the end of string
		StackArray<char> regionNameEOS = NazaraStackArrayNoInit(char, regionName.size() + 1);
		std::memcpy(regionNameEOS.data(), regionName.data(), regionName.size());
		regionNameEOS[regionName.size()] = '\0';

		m_commandBuffer.BeginDebugRegion(regionNameEOS.data(), color);
	}

	void VulkanCommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect, const ClearValues* clearValues, std::size_t clearValueCount)
	{
		const VulkanRenderPass& vkRenderPass = static_cast<const VulkanRenderPass&>(renderPass);
		const VulkanFramebuffer& vkFramebuffer = static_cast<const VulkanFramebuffer&>(framebuffer);

		std::size_t attachmentCount = vkRenderPass.GetAttachmentCount();

		StackArray<VkClearValue> vkClearValues = NazaraStackArray(VkClearValue, attachmentCount);
		for (std::size_t i = 0; i < attachmentCount; ++i)
		{
			const auto& values = (i < clearValueCount) ? clearValues[i] : CommandBufferBuilder::ClearValues{};
			auto& vkValues = vkClearValues[i];

			if (PixelFormatInfo::GetContent(vkRenderPass.GetAttachment(i).format) == PixelFormatContent::ColorRGBA)
			{
				vkValues.color.float32[0] = values.color.r / 255.f;
				vkValues.color.float32[1] = values.color.g / 255.f;
				vkValues.color.float32[2] = values.color.b / 255.f;
				vkValues.color.float32[3] = values.color.a / 255.f;
			}
			else
			{
				vkValues.depthStencil.depth = values.depth;
				vkValues.depthStencil.stencil = values.stencil;
			}
		}

		VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
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

	void VulkanCommandBufferBuilder::BindIndexBuffer(const AbstractBuffer& indexBuffer, UInt64 offset)
	{
		const VulkanBuffer& vkBuffer = static_cast<const VulkanBuffer&>(indexBuffer);

		m_commandBuffer.BindIndexBuffer(vkBuffer.GetBuffer(), offset, VK_INDEX_TYPE_UINT16); //< Fuck me right?
	}

	void VulkanCommandBufferBuilder::BindPipeline(const RenderPipeline& pipeline)
	{
		if (!m_currentRenderPass)
			throw std::runtime_error("BindPipeline must be called in a RenderPass");

		const VulkanRenderPipeline& vkBinding = static_cast<const VulkanRenderPipeline&>(pipeline);

		m_commandBuffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vkBinding.Get(*m_currentRenderPass, m_currentSubpassIndex));
	}

	void VulkanCommandBufferBuilder::BindShaderBinding(UInt32 set, const ShaderBinding& binding)
	{
		const VulkanShaderBinding& vkBinding = static_cast<const VulkanShaderBinding&>(binding);
		const VulkanRenderPipelineLayout& pipelineLayout = vkBinding.GetOwner();

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.GetPipelineLayout(), set, vkBinding.GetDescriptorSet());
	}

	void VulkanCommandBufferBuilder::BindShaderBinding(const RenderPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding)
	{
		const VulkanRenderPipelineLayout& vkPipelineLayout = static_cast<const VulkanRenderPipelineLayout&>(pipelineLayout);
		const VulkanShaderBinding& vkBinding = static_cast<const VulkanShaderBinding&>(binding);

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineLayout.GetPipelineLayout(), set, vkBinding.GetDescriptorSet());
	}

	void VulkanCommandBufferBuilder::BindVertexBuffer(UInt32 binding, const AbstractBuffer& vertexBuffer, UInt64 offset)
	{
		const VulkanBuffer& vkBuffer = static_cast<const VulkanBuffer&>(vertexBuffer);

		m_commandBuffer.BindVertexBuffer(binding, vkBuffer.GetBuffer(), offset);
	}

	void VulkanCommandBufferBuilder::CopyBuffer(const RenderBufferView& source, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		VulkanBuffer& sourceBuffer = *static_cast<VulkanBuffer*>(source.GetBuffer());
		VulkanBuffer& targetBuffer = *static_cast<VulkanBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(sourceBuffer.GetBuffer(), targetBuffer.GetBuffer(), size, sourceOffset + source.GetOffset(), targetOffset + target.GetOffset());
	}

	void VulkanCommandBufferBuilder::CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		const auto& vkAllocation = static_cast<const VulkanUploadPool::VulkanAllocation&>(allocation);
		VulkanBuffer& targetBuffer = *static_cast<VulkanBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(vkAllocation.buffer, targetBuffer.GetBuffer(), size, vkAllocation.offset + sourceOffset, target.GetOffset() + targetOffset);
	}

	void VulkanCommandBufferBuilder::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		m_commandBuffer.Draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void VulkanCommandBufferBuilder::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		m_commandBuffer.DrawIndexed(indexCount, instanceCount, firstVertex, 0, firstInstance);
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

	void VulkanCommandBufferBuilder::NextSubpass()
	{
		m_commandBuffer.NextSubpass();
		m_currentSubpassIndex++;
	}

	void VulkanCommandBufferBuilder::PreTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0U, VK_ACCESS_TRANSFER_READ_BIT);
	}

	void VulkanCommandBufferBuilder::PostTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_UNIFORM_READ_BIT);
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
		const VulkanTexture& vkTexture = static_cast<const VulkanTexture&>(texture);

		m_commandBuffer.ImageBarrier(ToVulkan(srcStageMask), ToVulkan(dstStageMask), VkDependencyFlags(0), ToVulkan(srcAccessMask), ToVulkan(dstAccessMask), ToVulkan(oldLayout), ToVulkan(newLayout), vkTexture.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT);
	}
}
