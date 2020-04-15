// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLMultipleFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLSingleFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/OpenGLRenderer/OpenGLUploadPool.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	void OpenGLCommandBufferBuilder::BeginDebugRegion(const std::string_view& regionName, const Nz::Color& color)
	{
		// Ensure \0 at the end of string
		StackArray<char> regionNameEOS = NazaraStackArrayNoInit(char, regionName.size() + 1);
		std::memcpy(regionNameEOS.data(), regionName.data(), regionName.size());
		regionNameEOS[regionName.size()] = '\0';

		m_commandBuffer.BeginDebugRegion(regionNameEOS.data(), color);
	}

	void OpenGLCommandBufferBuilder::BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, Nz::Recti renderRect, std::initializer_list<ClearValues> clearValues)
	{
		const OpenGLRenderPass& vkRenderPass = static_cast<const OpenGLRenderPass&>(renderPass);

		const Vk::Framebuffer& vkFramebuffer = [&] () -> const Vk::Framebuffer&
		{
			const OpenGLFramebuffer& vkFramebuffer = static_cast<const OpenGLFramebuffer&>(framebuffer);
			switch (vkFramebuffer.GetType())
			{
				case OpenGLFramebuffer::Type::Multiple:
				{
					const OpenGLMultipleFramebuffer& vkMultipleFramebuffer = static_cast<const OpenGLMultipleFramebuffer&>(vkFramebuffer);
					m_framebufferCount = std::max(m_framebufferCount, vkMultipleFramebuffer.GetFramebufferCount());
					return vkMultipleFramebuffer.GetFramebuffer(m_imageIndex);
				}

				case OpenGLFramebuffer::Type::Single:
					return static_cast<const OpenGLSingleFramebuffer&>(vkFramebuffer).GetFramebuffer();
			}

			throw std::runtime_error("Unhandled framebuffer type " + std::to_string(UnderlyingCast(vkFramebuffer.GetType())));
		}();

		VkRect2D renderArea;
		renderArea.offset.x = renderRect.x;
		renderArea.offset.y = renderRect.y;
		renderArea.extent.width = renderRect.width;
		renderArea.extent.height = renderRect.height;

		StackArray<VkClearValue> vkClearValues = NazaraStackArray(VkClearValue, clearValues.size());

		std::size_t index = 0;
		for (const ClearValues& values : clearValues)
		{
			auto& vkValues = vkClearValues[index];

			if (PixelFormatInfo::GetContent(vkRenderPass.GetAttachmentFormat(index)) == PixelFormatContent_ColorRGBA)
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

			index++;
		}

		VkRenderPassBeginInfo beginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		beginInfo.renderPass = vkRenderPass.GetRenderPass();
		beginInfo.framebuffer = vkFramebuffer;
		beginInfo.renderArea.offset.x = renderRect.x;
		beginInfo.renderArea.offset.y = renderRect.y;
		beginInfo.renderArea.extent.width = renderRect.width;
		beginInfo.renderArea.extent.height = renderRect.height;
		beginInfo.clearValueCount = vkClearValues.size();
		beginInfo.pClearValues = vkClearValues.data();

		m_commandBuffer.BeginRenderPass(beginInfo);

		m_currentRenderPass = &vkRenderPass;
	}

	void OpenGLCommandBufferBuilder::BindIndexBuffer(Nz::AbstractBuffer* indexBuffer, UInt64 offset)
	{
		OpenGLBuffer& vkBuffer = *static_cast<OpenGLBuffer*>(indexBuffer);

		m_commandBuffer.BindIndexBuffer(vkBuffer.GetBuffer(), offset, VK_INDEX_TYPE_UINT16); //< Fuck me right?
	}

	void OpenGLCommandBufferBuilder::BindPipeline(const RenderPipeline& pipeline)
	{
		if (!m_currentRenderPass)
			throw std::runtime_error("BindPipeline must be called in a RenderPass");

		const OpenGLRenderPipeline& vkBinding = static_cast<const OpenGLRenderPipeline&>(pipeline);

		m_commandBuffer.BindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, vkBinding.Get(m_currentRenderPass->GetRenderPass()));
	}

	void OpenGLCommandBufferBuilder::BindShaderBinding(const ShaderBinding& binding)
	{
		const OpenGLShaderBinding& vkBinding = static_cast<const OpenGLShaderBinding&>(binding);

		const OpenGLRenderPipelineLayout& pipelineLayout = vkBinding.GetOwner();

		m_commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout.GetPipelineLayout(), 0U, vkBinding.GetDescriptorSet());
	}

	void OpenGLCommandBufferBuilder::BindVertexBuffer(UInt32 binding, Nz::AbstractBuffer* vertexBuffer, UInt64 offset)
	{
		OpenGLBuffer& vkBuffer = *static_cast<OpenGLBuffer*>(vertexBuffer);

		m_commandBuffer.BindVertexBuffer(binding, vkBuffer.GetBuffer(), offset);
	}

	void OpenGLCommandBufferBuilder::CopyBuffer(const RenderBufferView& source, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		OpenGLBuffer& sourceBuffer = *static_cast<OpenGLBuffer*>(source.GetBuffer());
		OpenGLBuffer& targetBuffer = *static_cast<OpenGLBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(sourceBuffer.GetBuffer(), targetBuffer.GetBuffer(), size, sourceOffset + source.GetOffset(), targetOffset + target.GetOffset());
	}

	void OpenGLCommandBufferBuilder::CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset, UInt64 targetOffset)
	{
		const auto& vkAllocation = static_cast<const OpenGLUploadPool::OpenGLAllocation&>(allocation);
		OpenGLBuffer& targetBuffer = *static_cast<OpenGLBuffer*>(target.GetBuffer());

		m_commandBuffer.CopyBuffer(vkAllocation.buffer, targetBuffer.GetBuffer(), size, vkAllocation.offset + sourceOffset, target.GetOffset() + targetOffset);
	}

	void OpenGLCommandBufferBuilder::Draw(UInt32 vertexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		m_commandBuffer.Draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void OpenGLCommandBufferBuilder::DrawIndexed(UInt32 indexCount, UInt32 instanceCount, UInt32 firstVertex, UInt32 firstInstance)
	{
		m_commandBuffer.DrawIndexed(indexCount, instanceCount, firstVertex, 0, firstInstance);
	}

	void OpenGLCommandBufferBuilder::EndDebugRegion()
	{
		m_commandBuffer.EndDebugRegion();
	}

	void OpenGLCommandBufferBuilder::EndRenderPass()
	{
		m_commandBuffer.EndRenderPass();
		m_currentRenderPass = nullptr;
	}

	void OpenGLCommandBufferBuilder::PreTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0U, VK_ACCESS_TRANSFER_READ_BIT);
	}

	void OpenGLCommandBufferBuilder::PostTransferBarrier()
	{
		m_commandBuffer.MemoryBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_UNIFORM_READ_BIT);
	}

	void OpenGLCommandBufferBuilder::SetScissor(Nz::Recti scissorRegion)
	{
		m_commandBuffer.SetScissor(scissorRegion);
	}

	void OpenGLCommandBufferBuilder::SetViewport(Nz::Recti viewportRegion)
	{
		m_commandBuffer.SetViewport(Nz::Rectf(viewportRegion), 0.f, 1.f);
	}
}

#endif
