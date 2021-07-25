// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COMMANDBUFFERBUILDER_HPP
#define NAZARA_COMMANDBUFFERBUILDER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <string_view>

namespace Nz
{
	class Framebuffer;
	class RenderPass;
	class RenderPipeline;
	class RenderPipelineLayout;
	class ShaderBinding;
	class Texture;

	class NAZARA_RENDERER_API CommandBufferBuilder
	{
		public:
			struct ClearValues;

			CommandBufferBuilder() = default;
			CommandBufferBuilder(const CommandBufferBuilder&) = delete;
			CommandBufferBuilder(CommandBufferBuilder&&) = default;
			virtual ~CommandBufferBuilder();

			virtual void BeginDebugRegion(const std::string_view& regionName, const Color& color) = 0;
			virtual void BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect, const ClearValues* clearValues, std::size_t clearValueCount) = 0;
			inline void BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect);
			inline void BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, const Recti& renderRect, std::initializer_list<ClearValues> clearValues);

			virtual void BindIndexBuffer(const AbstractBuffer& indexBuffer, UInt64 offset = 0) = 0;
			virtual void BindPipeline(const RenderPipeline& pipeline) = 0;
			virtual void BindShaderBinding(UInt32 set, const ShaderBinding& binding) = 0;
			virtual void BindShaderBinding(const RenderPipelineLayout& pipelineLayout, UInt32 set, const ShaderBinding& binding) = 0;
			virtual void BindVertexBuffer(UInt32 binding, const AbstractBuffer& vertexBuffer, UInt64 offset = 0) = 0;

			inline void CopyBuffer(const RenderBufferView& source, const RenderBufferView& target);
			virtual void CopyBuffer(const RenderBufferView& source, const RenderBufferView& target, UInt64 size, UInt64 fromOffset = 0, UInt64 toOffset = 0) = 0;
			inline void CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target);
			virtual void CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target, UInt64 size, UInt64 fromOffset = 0, UInt64 toOffset = 0) = 0;

			virtual void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0) = 0;
			virtual void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0) = 0;

			virtual void EndDebugRegion() = 0;
			virtual void EndRenderPass() = 0;

			virtual void NextSubpass() = 0;

			virtual void PreTransferBarrier() = 0;
			virtual void PostTransferBarrier() = 0;

			virtual void SetScissor(const Recti& scissorRegion) = 0;
			virtual void SetViewport(const Recti& viewportRegion) = 0;

			virtual void TextureBarrier(PipelineStageFlags srcStageMask, PipelineStageFlags dstStageMask, MemoryAccessFlags srcAccessMask, MemoryAccessFlags dstAccessMask, TextureLayout oldLayout, TextureLayout newLayout, const Texture& texture) = 0;

			CommandBufferBuilder& operator=(const CommandBufferBuilder&) = delete;
			CommandBufferBuilder& operator=(CommandBufferBuilder&&) = default;

			struct ClearValues
			{
				Color color = Color::Black;
				float depth = 1.f;
				UInt32 stencil = 0;
			};
	};
}

#include <Nazara/Renderer/CommandBufferBuilder.inl>

#endif // NAZARA_COMMANDBUFFERBUILDER_HPP
