// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/CommandBuffer.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <optional>
#include <variant>
#include <vector>

namespace Nz
{
	class OpenGLCommandPool;
	class OpenGLFramebuffer;

	class NAZARA_OPENGLRENDERER_API OpenGLCommandBuffer final : public CommandBuffer
	{
		public:
			inline OpenGLCommandBuffer();
			inline OpenGLCommandBuffer(OpenGLCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex);
			OpenGLCommandBuffer(const OpenGLCommandBuffer&) = delete;
			OpenGLCommandBuffer(OpenGLCommandBuffer&&) = delete;
			~OpenGLCommandBuffer() = default;

			inline void BeginDebugRegion(const std::string_view& regionName, const Nz::Color& color);

			inline void BindIndexBuffer(GLuint indexBuffer, UInt64 offset = 0);
			inline void BindPipeline(const OpenGLRenderPipeline* pipeline);
			inline void BindShaderBinding(const OpenGLShaderBinding* binding);
			inline void BindVertexBuffer(UInt32 binding, GLuint vertexBuffer, UInt64 offset = 0);

			inline void CopyBuffer(GLuint source, GLuint target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);
			inline void CopyBuffer(const UploadPool::Allocation& allocation, GLuint target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);

			inline void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0);
			inline void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0);

			inline void EndDebugRegion();

			void Execute();

			inline std::size_t GetBindingIndex() const;
			inline std::size_t GetPoolIndex() const;
			inline const OpenGLCommandPool& GetOwner() const;

			inline void SetFramebuffer(const OpenGLFramebuffer& framebuffer, const RenderPass& renderPass, std::initializer_list<CommandBufferBuilder::ClearValues> clearValues);
			inline void SetScissor(Nz::Recti scissorRegion);
			inline void SetViewport(Nz::Recti viewportRegion);

			OpenGLCommandBuffer& operator=(const OpenGLCommandBuffer&) = delete;
			OpenGLCommandBuffer& operator=(OpenGLCommandBuffer&&) = delete;

		private:
			struct DrawStates;

			void ApplyStates(const GL::Context& context, const DrawStates& states);
			void Release();

			struct BeginDebugRegionData
			{
				std::string regionName;
				Nz::Color color;
			};

			struct CopyBufferData
			{
				GLuint source;
				GLuint target;
				UInt64 size;
				UInt64 sourceOffset;
				UInt64 targetOffset;
			};

			struct CopyBufferFromMemoryData
			{
				const void* memory;
				GLuint target;
				UInt64 size;
				UInt64 targetOffset;
			};

			struct DrawStates
			{
				struct VertexBuffer
				{
					GLuint vertexBuffer = 0;
					UInt64 offset;
				};

				GLuint indexBuffer = 0;
				const OpenGLRenderPipeline* pipeline = nullptr;
				const OpenGLShaderBinding* shaderBindings = nullptr;
				UInt64 indexBufferOffset;
				std::optional<Nz::Recti> scissorRegion;
				std::optional<Nz::Recti> viewportRegion;
				std::vector<VertexBuffer> vertexBuffers;
			};

			struct DrawData
			{
				DrawStates states;
				UInt32 firstInstance;
				UInt32 firstVertex;
				UInt32 instanceCount;
				UInt32 vertexCount;
			};

			struct DrawIndexedData
			{
				DrawStates states;
				UInt32 firstInstance;
				UInt32 firstVertex;
				UInt32 indexCount;
				UInt32 instanceCount;
			};

			struct EndDebugRegionData
			{
			};

			struct SetFrameBufferData
			{
				std::array<CommandBufferBuilder::ClearValues, 16> clearValues; //< TODO: Remove hard limit?
				const OpenGLFramebuffer* framebuffer;
			};

			using CommandData = std::variant<
				BeginDebugRegionData,
				CopyBufferData,
				CopyBufferFromMemoryData,
				DrawData,
				DrawIndexedData,
				EndDebugRegionData,
				SetFrameBufferData
			>;

			DrawStates m_currentStates;
			std::size_t m_bindingIndex;
			std::size_t m_poolIndex;
			std::vector<CommandData> m_commands;
			OpenGLCommandPool* m_owner;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP
