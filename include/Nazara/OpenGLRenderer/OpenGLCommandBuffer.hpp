// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/Renderer/CommandBuffer.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <optional>
#include <variant>
#include <vector>

namespace Nz
{
	class OpenGLCommandPool;
	class OpenGLComputePipeline;
	class OpenGLFramebuffer;
	class OpenGLRenderPass;
	class OpenGLTexture;

	class NAZARA_OPENGLRENDERER_API OpenGLCommandBuffer final : public CommandBuffer
	{
		public:
			inline OpenGLCommandBuffer();
			inline OpenGLCommandBuffer(OpenGLCommandPool& owner, std::size_t poolIndex, std::size_t bindingIndex);
			OpenGLCommandBuffer(const OpenGLCommandBuffer&) = delete;
			OpenGLCommandBuffer(OpenGLCommandBuffer&&) = delete;
			~OpenGLCommandBuffer() = default;

			inline void BeginDebugRegion(const std::string_view& regionName, const Color& color);

			inline void BindComputePipeline(const OpenGLComputePipeline* pipeline);
			inline void BindComputeShaderBinding(const OpenGLRenderPipelineLayout& pipelineLayout, UInt32 set, const OpenGLShaderBinding* binding);
			inline void BindIndexBuffer(GLuint indexBuffer, IndexType indexType, UInt64 offset = 0);
			inline void BindRenderPipeline(const OpenGLRenderPipeline* pipeline);
			inline void BindRenderShaderBinding(const OpenGLRenderPipelineLayout& pipelineLayout, UInt32 set, const OpenGLShaderBinding* binding);
			inline void BindVertexBuffer(UInt32 binding, GLuint vertexBuffer, UInt64 offset = 0);
			inline void BlitTexture(const OpenGLTexture& source, const Boxui& sourceBox, const OpenGLTexture& target, const Boxui& targetBox, SamplerFilter filter = SamplerFilter::Nearest);

			inline void CopyBuffer(GLuint source, GLuint target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);
			inline void CopyBuffer(const UploadPool::Allocation& allocation, GLuint target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);
			inline void CopyTexture(const OpenGLTexture& source, const Boxui& sourceBox, const OpenGLTexture& target, const Vector3ui& targetPoint);

			inline void Dispatch(UInt32 numGroupsX, UInt32 numGroupsY, UInt32 numGroupsZ);

			inline void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0);
			inline void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstIndex = 0, UInt32 firstInstance = 0);

			inline void EndDebugRegion();

			void Execute();

			inline std::size_t GetBindingIndex() const;
			inline std::size_t GetPoolIndex() const;
			inline const OpenGLCommandPool& GetOwner() const;

			inline void InsertMemoryBarrier(GLbitfield barriers);

			inline void SetFramebuffer(const OpenGLFramebuffer& framebuffer, const OpenGLRenderPass& renderPass, const CommandBufferBuilder::ClearValues* clearValues, std::size_t clearValueCount);
			inline void SetScissor(const Recti& scissorRegion);
			inline void SetViewport(const Recti& viewportRegion);

			void UpdateDebugName(std::string_view name) override;

			OpenGLCommandBuffer& operator=(const OpenGLCommandBuffer&) = delete;
			OpenGLCommandBuffer& operator=(OpenGLCommandBuffer&&) = delete;

		private:
			struct DrawStates;
			struct ShaderBindings;

			void ApplyBindings(const GL::Context& context, const ShaderBindings& bindings);
			void ApplyStates(const GL::Context& context, const DrawStates& states);
			void Release() override;

			struct BeginDebugRegionData
			{
				std::string regionName;
				Color color;
			};

			struct BlitTextureData
			{
				const OpenGLTexture* source;
				const OpenGLTexture* target;
				Boxui sourceBox;
				Boxui targetBox;
				SamplerFilter filter;
			};

			struct ComputeStates
			{
				const OpenGLComputePipeline* pipeline = nullptr;
			};

			struct CopyBufferData
			{
				GLuint source;
				GLuint target;
				UInt64 size;
				UInt64 sourceOffset;
				UInt64 targetOffset;
			};

			struct CopyTextureData
			{
				const OpenGLTexture* source;
				const OpenGLTexture* target;
				Boxui sourceBox;
				Vector3ui targetPoint;
			};

			struct CopyBufferFromMemoryData
			{
				const void* memory;
				GLuint target;
				UInt64 size;
				UInt64 targetOffset;
			};

			struct ShaderBindings
			{
				std::vector<std::pair<const OpenGLRenderPipelineLayout*, const OpenGLShaderBinding*>> shaderBindings;
			};

			struct DispatchData
			{
				ComputeStates states;
				ShaderBindings bindings;
				UInt32 numGroupsX;
				UInt32 numGroupsY;
				UInt32 numGroupsZ;
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
				UInt64 indexBufferOffset;
				IndexType indexBufferType;
				std::optional<Recti> scissorRegion;
				std::optional<Recti> viewportRegion;
				std::vector<VertexBuffer> vertexBuffers;
				bool shouldFlipY = false;
			};

			struct DrawData
			{
				DrawStates states;
				ShaderBindings bindings;
				UInt32 firstInstance;
				UInt32 firstVertex;
				UInt32 instanceCount;
				UInt32 vertexCount;
			};

			struct DrawIndexedData
			{
				DrawStates states;
				ShaderBindings bindings;
				UInt32 firstIndex;
				UInt32 firstInstance;
				UInt32 indexCount;
				UInt32 instanceCount;
			};

			struct EndDebugRegionData
			{
			};

			struct MemoryBarrier
			{
				GLbitfield barriers;
			};

			struct SetFrameBufferData
			{
				std::array<CommandBufferBuilder::ClearValues, 16> clearValues; //< TODO: Remove hard limit?
				const OpenGLFramebuffer* framebuffer;
				const OpenGLRenderPass* renderpass;
			};

			using CommandData = std::variant<
				BeginDebugRegionData,
				BlitTextureData,
				CopyBufferData,
				CopyBufferFromMemoryData,
				CopyTextureData,
				DispatchData,
				DrawData,
				DrawIndexedData,
				EndDebugRegionData,
				MemoryBarrier,
				SetFrameBufferData
			>;

			ComputeStates m_currentComputeStates;
			DrawStates m_currentDrawStates;
			ShaderBindings m_currentComputeShaderBindings;
			ShaderBindings m_currentGraphicsShaderBindings;
			std::optional<MemoryBarrier> m_pendingBarrier;
			std::size_t m_bindingIndex;
			std::size_t m_maxColorBufferCount;
			std::size_t m_poolIndex;
			std::vector<CommandData> m_commands;
			OpenGLCommandPool* m_owner;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLCommandBuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP
