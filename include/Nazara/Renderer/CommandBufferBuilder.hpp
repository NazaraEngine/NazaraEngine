// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_COMMANDBUFFERBUILDER_HPP
#define NAZARA_RENDERER_COMMANDBUFFERBUILDER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <span>
#include <string_view>

namespace Nz
{
	class ComputePipeline;
	class Framebuffer;
	class RenderPass;
	class RenderPipeline;
	class RenderPipelineLayout;
	class ShaderBinding;
	class Swapchain;
	class Texture;

	class NAZARA_RENDERER_API CommandBufferBuilder
	{
		public:
			struct BufferBarrierInfo;
			struct ClearValues;
			struct MemoryBarrierInfo;
			struct TextureBarrierInfo;

			CommandBufferBuilder() = default;
			CommandBufferBuilder(const CommandBufferBuilder&) = delete;
			CommandBufferBuilder(CommandBufferBuilder&&) = default;
			virtual ~CommandBufferBuilder();

			virtual void BeginDebugRegion(std::string_view regionName, const Color& color) = 0;
			virtual void BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect, const ClearValues* clearValues, std::size_t clearValueCount) = 0;
			inline void BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect);
			inline void BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect, std::initializer_list<ClearValues> clearValues);

			virtual void BindComputePipeline(const ComputePipeline& pipeline) = 0;
			virtual void BindComputeShaderBinding(UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) = 0;
			virtual void BindComputeShaderBinding(const RenderPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) = 0;
			virtual void BindIndexBuffer(const RenderBuffer& indexBuffer, IndexType indexType, UInt64 offset = 0) = 0;
			virtual void BindRenderPipeline(const RenderPipeline& pipeline) = 0;
			virtual void BindRenderShaderBinding(UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) = 0;
			virtual void BindRenderShaderBinding(const RenderPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding, std::span<const UInt32> dynamicOffsets = {}) = 0;
			virtual void BindVertexBuffer(UInt32 binding, const RenderBuffer& vertexBuffer, UInt64 offset = 0) = 0;

			virtual void BlitTexture(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Texture& toTexture, const Boxui& toBox, TextureLayout toLayout, SamplerFilter filter) = 0;
			virtual void BlitTextureToSwapchain(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Swapchain& swapchain, std::size_t imageIndex) = 0;

			inline void BufferBarrier(const BufferBarrierInfo& barrierInfo);

			virtual void BuildMipmaps(Texture& texture, UInt8 baseLevel, UInt8 levelCount, PipelineStageFlags srcStageMask, PipelineStageFlags dstStageMask, MemoryAccessFlags srcAccessMask, MemoryAccessFlags dstAccessMask, TextureLayout oldLayout, TextureLayout newLayout) = 0;

			inline void CopyBuffer(const RenderBufferView& source, const RenderBufferView& target);
			virtual void CopyBuffer(const RenderBufferView& source, const RenderBufferView& target, UInt64 size, UInt64 fromOffset = 0, UInt64 toOffset = 0) = 0;
			inline void CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target);
			virtual void CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target, UInt64 size, UInt64 fromOffset = 0, UInt64 toOffset = 0) = 0;
			virtual void CopyTexture(const Texture& fromTexture, const Boxui& fromBox, TextureLayout fromLayout, const Texture& toTexture, const Vector3ui& toPos, TextureLayout toLayout) = 0;

			virtual void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0) = 0;
			virtual void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstIndex = 0, UInt32 vertexOffset = 0, UInt32 firstInstance = 0) = 0;

			virtual void Dispatch(UInt32 workgroupX, UInt32 workgroupY, UInt32 workgroupZ) = 0;

			virtual void EndDebugRegion() = 0;
			virtual void EndRenderPass() = 0;

			virtual void InsertDebugLabel(std::string_view label, const Color& color) = 0;

			inline void MemoryBarrier(const MemoryBarrierInfo& barrierInfo);

			virtual void NextSubpass() = 0;

			virtual void PipelineBarrier(std::span<const MemoryBarrierInfo> memoryBarriers, std::span<const BufferBarrierInfo> bufferBarriers, std::span<const TextureBarrierInfo> textureBarriers) = 0;

			virtual void PushConstants(const RenderPipelineLayout& pipelineLayout, UInt32 offset, UInt32 size, const void* data) = 0;

			virtual void SetScissor(const Recti& scissorRegion) = 0;
			virtual void SetViewport(const Recti& viewportRegion) = 0;

			inline void TextureBarrier(const TextureBarrierInfo& barrierInfo);

			CommandBufferBuilder& operator=(const CommandBufferBuilder&) = delete;
			CommandBufferBuilder& operator=(CommandBufferBuilder&&) = default;

			struct MemoryBarrierInfo
			{
				PipelineStageFlags srcStageMask;
				PipelineStageFlags dstStageMask;
				MemoryAccessFlags srcAccessMask;
				MemoryAccessFlags dstAccessMask;
			};

			struct BufferBarrierInfo
			{
				PipelineStageFlags srcStageMask;
				PipelineStageFlags dstStageMask;
				MemoryAccessFlags srcAccessMask;
				MemoryAccessFlags dstAccessMask;
				const RenderBuffer* buffer;
			};

			struct TextureBarrierInfo
			{
				PipelineStageFlags srcStageMask;
				PipelineStageFlags dstStageMask;
				MemoryAccessFlags srcAccessMask;
				MemoryAccessFlags dstAccessMask;
				TextureLayout oldLayout;
				TextureLayout newLayout;
				const Texture* texture;
			};

			struct ClearValues
			{
				Color color = Color::Black();
				float depth = 1.f;
				UInt32 stencil = 0;
			};
	};
}

#include <Nazara/Renderer/CommandBufferBuilder.inl>

#endif // NAZARA_RENDERER_COMMANDBUFFERBUILDER_HPP
