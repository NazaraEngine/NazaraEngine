// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFERBUILDER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFERBUILDER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/Renderer/GpuCommandBufferBuilder.hpp>

namespace Nz
{
	class OpenGLCommandBuffer;
	class OpenGLTexture;

	class NAZARA_OPENGLRENDERER_API OpenGLCommandBufferBuilder final : public GpuCommandBufferBuilder
	{
		public:
			inline OpenGLCommandBufferBuilder(OpenGLCommandBuffer& commandBuffer);
			OpenGLCommandBufferBuilder(const OpenGLCommandBufferBuilder&) = delete;
			OpenGLCommandBufferBuilder(OpenGLCommandBufferBuilder&&) noexcept = default;
			~OpenGLCommandBufferBuilder() = default;

			void BeginDebugRegion(std::string_view regionName, const Color& color) override;
			void BeginRenderPass(const GpuFramebuffer& framebuffer, const GpuRenderPass& renderPass, const Recti& renderRect, const ClearValues* clearValues, std::size_t clearValueCount) override;

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

			void InsertDebugLabel(std::string_view label, const Color& color) override;

			void NextSubpass() override;

			void PipelineBarrier(std::span<const MemoryBarrierInfo> memoryBarriers, std::span<const BufferBarrierInfo> bufferBarriers, std::span<const TextureBarrierInfo> textureBarriers) override;

			void PushConstants(const GpuPipelineLayout& pipelineLayout, UInt32 offset, UInt32 size, const void* data) override;

			void SetScissor(const Recti& scissorRegion) override;
			void SetViewport(const Recti& viewportRegion) override;

			OpenGLCommandBufferBuilder& operator=(const OpenGLCommandBufferBuilder&) = delete;
			OpenGLCommandBufferBuilder& operator=(OpenGLCommandBufferBuilder&&) = delete;

		private:
			OpenGLCommandBuffer& m_commandBuffer;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFERBUILDER_HPP
