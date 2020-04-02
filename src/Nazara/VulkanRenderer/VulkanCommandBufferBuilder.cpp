// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/VulkanUploadPool.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	void VulkanCommandBufferBuilder::BeginDebugRegion(const std::string_view& regionName, const Nz::Color& color)
	{
		// Ensure \0 at the end of string
		StackArray<char> regionNameEOS = NazaraStackArrayNoInit(char, regionName.size() + 1);
		std::memcpy(regionNameEOS.data(), regionName.data(), regionName.size());
		regionNameEOS[regionName.size()] = '\0';

		m_commandBuffer.BeginDebugRegion(regionNameEOS.data(), color);
	}

	void VulkanCommandBufferBuilder::BindIndexBuffer(Nz::AbstractBuffer* indexBuffer, UInt64 offset)
	{
		VulkanBuffer& vkBuffer = *static_cast<VulkanBuffer*>(indexBuffer);

		m_commandBuffer.BindIndexBuffer(vkBuffer.GetBuffer(), offset, VK_INDEX_TYPE_UINT16); //< Fuck me right?
	}

	void VulkanCommandBufferBuilder::BindShaderBinding(ShaderBinding& binding)
	{
		VulkanShaderBinding& vkBinding = static_cast<VulkanShaderBinding&>(binding);

		VulkanRenderPipelineLayout& pipelineLayout = vkBinding.GetOwner();

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.GetPipelineLayout(), 0U, vkBinding.GetDescriptorSet());
	}

	void VulkanCommandBufferBuilder::BindVertexBuffer(UInt32 binding, Nz::AbstractBuffer* vertexBuffer, UInt64 offset)
	{
		VulkanBuffer& vkBuffer = *static_cast<VulkanBuffer*>(vertexBuffer);

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

	void VulkanCommandBufferBuilder::PreTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0U, VK_ACCESS_TRANSFER_READ_BIT);
	}

	void VulkanCommandBufferBuilder::PostTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_UNIFORM_READ_BIT);
	}

	void VulkanCommandBufferBuilder::SetScissor(Nz::Recti scissorRegion)
	{
		m_commandBuffer.SetScissor(scissorRegion);
	}

	void VulkanCommandBufferBuilder::SetViewport(Nz::Recti viewportRegion)
	{
		m_commandBuffer.SetViewport(Nz::Rectf(viewportRegion), 0.f, 1.f);
	}
}
