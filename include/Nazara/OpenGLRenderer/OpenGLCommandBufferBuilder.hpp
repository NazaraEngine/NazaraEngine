// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFERBUILDER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFERBUILDER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>

namespace Nz
{
	class OpenGLRenderPass;

	class NAZARA_OPENGLRENDERER_API OpenGLCommandBufferBuilder final : public CommandBufferBuilder
	{
		public:
			OpenGLCommandBufferBuilder() = default;
			OpenGLCommandBufferBuilder(const OpenGLCommandBufferBuilder&) = delete;
			OpenGLCommandBufferBuilder(OpenGLCommandBufferBuilder&&) noexcept = default;
			~OpenGLCommandBufferBuilder() = default;

			void BeginDebugRegion(const std::string_view& regionName, const Nz::Color& color) override;
			void BeginRenderPass(const Framebuffer& framebuffer, const RenderPass& renderPass, Nz::Recti renderRect, std::initializer_list<ClearValues> clearValues) override;

			void BindIndexBuffer(AbstractBuffer* indexBuffer, UInt64 offset = 0) override;
			void BindPipeline(const RenderPipeline& pipeline) override;
			void BindShaderBinding(const ShaderBinding& binding) override;
			void BindVertexBuffer(UInt32 binding, Nz::AbstractBuffer* vertexBuffer, UInt64 offset = 0) override;

			void CopyBuffer(const RenderBufferView& source, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0) override;
			void CopyBuffer(const UploadPool::Allocation& allocation, const RenderBufferView& target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0) override;

			void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0) override;
			void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0) override;

			void EndDebugRegion() override;
			void EndRenderPass() override;

			void PreTransferBarrier() override;
			void PostTransferBarrier() override;

			void SetScissor(Nz::Recti scissorRegion) override;
			void SetViewport(Nz::Recti viewportRegion) override;

			OpenGLCommandBufferBuilder& operator=(const OpenGLCommandBufferBuilder&) = delete;
			OpenGLCommandBufferBuilder& operator=(OpenGLCommandBufferBuilder&&) = delete;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFERBUILDER_HPP
