// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANCOMMANDBUFFERBUILDER_HPP
#define NAZARA_VULKANRENDERER_VULKANCOMMANDBUFFERBUILDER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/GpuCommandBufferBuilder.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandBuffer.hpp>

namespace Nz
{
	class VulkanRenderPass;

	class NAZARA_VULKANRENDERER_API VulkanCommandBufferBuilder final : public GpuCommandBufferBuilder
	{
		public:
			inline VulkanCommandBufferBuilder(Vk::CommandBuffer& commandBuffer);
			VulkanCommandBufferBuilder(const VulkanCommandBufferBuilder&) = delete;
			VulkanCommandBufferBuilder(VulkanCommandBufferBuilder&&) noexcept = default;
			~VulkanCommandBufferBuilder() = default;

			void BeginDebugRegion(std::string_view regionName, const Color& color) override;
			void BeginRenderPass(const Framebuffer& framebuffer, const GpuRenderPass& renderPass, const Recti& renderRect, const ClearValues* clearValues, std::size_t clearValueCount) override;

			void BindComputePipeline(const GpuComputePipeline& pipeline) override;
			void BindComputeShaderBinding(UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) override;
			void BindComputeShaderBinding(const GpuPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) override;
			void BindIndexBuffer(const GpuBuffer& indexBuffer, IndexType indexType, UInt64 offset = 0) override;
			void BindRenderPipeline(const GpuRenderPipeline& pipeline) override;
			void BindRenderShaderBinding(UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) override;
			void BindRenderShaderBinding(const GpuPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) override;
			void BindVertexBuffer(UInt32 binding, const GpuBuffer& vertexBuffer, UInt64 offset = 0) override;

			void BlitTexture(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Texture& toTexture, const Boxui& toBox, TextureLayout toLayout, SamplerFilter filter) override;
			void BlitTextureToSwapchain(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Swapchain& swapchain, std::size_t imageIndex) override;

			void BuildMipmaps(Texture& texture, UInt8 baseLevel, UInt8 levelCount, PipelineStageFlags srcStageMask, PipelineStageFlags dstStageMask, MemoryAccessFlags srcAccessMask, MemoryAccessFlags dstAccessMask, TextureLayout oldLayout, TextureLayout newLayout) override;

			void CopyBuffer(const GpuBufferView& source, const GpuBufferView& target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0) override;
			void CopyBuffer(const GpuUploadPool::Allocation& allocation, const GpuBufferView& target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0) override;
			void CopyTexture(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Texture& toTexture, const Vector3ui& toPos, TextureLayout toLayout) override;

			void Dispatch(UInt32 workgroupX, UInt32 workgroupY, UInt32 workgroupZ) override;

			void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0) override;
			void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstIndex = 0, UInt32 vertexOffset = 0, UInt32 firstInstance = 0) override;
			void DrawIndirect(const GpuBuffer& buffer, UInt64 offset, UInt32 drawCount, UInt32 stride) override;
			void DrawIndirectCount(const GpuBuffer& buffer, UInt64 offset, const GpuBuffer& countBuffer, UInt64 countBufferOffset, UInt32 maxDrawCount, UInt32 stride) override;
			void DrawIndexedIndirect(const GpuBuffer& buffer, UInt64 offset, UInt32 drawCount, UInt32 stride) override;
			void DrawIndexedIndirectCount(const GpuBuffer& buffer, UInt64 offset, const GpuBuffer& countBuffer, UInt64 countBufferOffset, UInt32 maxDrawCount, UInt32 stride) override;

			void EndDebugRegion() override;
			void EndRenderPass() override;

			void ExecuteCommands(std::span<const GpuCommandBuffer*> commandBuffers) override;

			inline Vk::CommandBuffer& GetCommandBuffer();

			void InsertDebugLabel(std::string_view label, const Color& color) override;

			void PipelineBarrier(std::span<const MemoryBarrierInfo> memoryBarriers, std::span<const BufferBarrierInfo> bufferBarriers, std::span<const TextureBarrierInfo> textureBarriers) override;

			void NextSubpass() override;

			void PushConstants(const GpuPipelineLayout& pipelineLayout, UInt32 offset, UInt32 size, const void* data) override;

			void SetScissor(const Recti& scissorRegion) override;
			void SetViewport(const Recti& viewportRegion) override;

			VulkanCommandBufferBuilder& operator=(const VulkanCommandBufferBuilder&) = delete;
			VulkanCommandBufferBuilder& operator=(VulkanCommandBufferBuilder&&) = delete;

		private:
			Vk::CommandBuffer& m_commandBuffer;
			const VulkanRenderPass* m_currentRenderPass;
			std::size_t m_currentSubpassIndex;
	};
}

#include <Nazara/VulkanRenderer/VulkanCommandBufferBuilder.inl>

#endif // NAZARA_VULKANRENDERER_VULKANCOMMANDBUFFERBUILDER_HPP
