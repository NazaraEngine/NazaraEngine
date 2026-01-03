// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMMANDBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderBinding.hpp>
#include <Nazara/Renderer/CommandBuffer.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <optional>
#include <span>
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

			inline void BeginDebugRegion(std::string_view regionName, const Color& color);

			inline void BindComputePipeline(const OpenGLComputePipeline* pipeline);
			inline void BindComputeShaderBinding(const OpenGLRenderPipelineLayout& pipelineLayout, UInt32 set, const OpenGLShaderBinding* binding, std::span<const UInt32> dynamicOffsets);
			inline void BindIndexBuffer(GLuint indexBuffer, IndexType indexType, UInt64 offset = 0);
			inline void BindRenderPipeline(const OpenGLRenderPipeline* pipeline);
			inline void BindRenderShaderBinding(const OpenGLRenderPipelineLayout& pipelineLayout, UInt32 set, const OpenGLShaderBinding* binding, std::span<const UInt32> dynamicOffsets);
			inline void BindVertexBuffer(UInt32 binding, GLuint vertexBuffer, UInt64 offset = 0);

			inline void BlitTexture(const OpenGLTexture& source, const Boxui& sourceBox, const OpenGLTexture& target, const Boxui& targetBox, SamplerFilter filter = SamplerFilter::Nearest);
			inline void BlitTextureToWindow(const OpenGLTexture& source, const Boxui& sourceBox, const Boxui& targetBox, SamplerFilter filter = SamplerFilter::Nearest);

			inline void BuildMipmaps(OpenGLTexture& texture, UInt8 baseLevel, UInt8 levelCount);

			inline void CopyBuffer(GLuint source, GLuint target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);
			inline void CopyBuffer(const UploadPool::Allocation& allocation, GLuint target, UInt64 size, UInt64 sourceOffset = 0, UInt64 targetOffset = 0);
			inline void CopyTexture(const OpenGLTexture& source, const Boxui& sourceBox, const OpenGLTexture& target, const Vector3ui& targetPoint);

			inline void Dispatch(UInt32 numGroupsX, UInt32 numGroupsY, UInt32 numGroupsZ);

			inline void Draw(UInt32 vertexCount, UInt32 instanceCount = 1, UInt32 firstVertex = 0, UInt32 firstInstance = 0);
			inline void DrawIndexed(UInt32 indexCount, UInt32 instanceCount = 1, UInt32 firstIndex = 0, UInt32 vertexOffset = 0, UInt32 firstInstance = 0);

			inline void EndDebugRegion();

			void Execute();

			inline void InsertDebugLabel(std::string_view label, const Color& color);

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

#define NAZARA_OPENGL_FOREACH_COMMANDS(cb, lastCb) \
	cb(BeginDebugRegionCommand) \
	cb(BlitTextureCommand) \
	cb(BlitTextureToWindowCommand) \
	cb(BuildTextureMipmapsCommand) \
	cb(CopyBufferCommand) \
	cb(CopyBufferFromMemoryCommand) \
	cb(CopyTextureCommand) \
	cb(DispatchCommand) \
	cb(DrawCommand) \
	cb(DrawIndexedCommand) \
	cb(EndDebugRegionCommand) \
	cb(InsertDebugLabelCommand) \
	cb(MemoryBarrier) \
	lastCb(SetFrameBufferCommand) \

#define NAZARA_OPENGL_COMMAND_CALLBACK(Command) struct Command;
			NAZARA_OPENGL_FOREACH_COMMANDS(NAZARA_OPENGL_COMMAND_CALLBACK, NAZARA_OPENGL_COMMAND_CALLBACK)
#undef NAZARA_OPENGL_COMMAND_CALLBACK

			using CommandList = TypeList<

#define NAZARA_OPENGL_COMMAND_CALLBACK(Command) Command,
#define NAZARA_OPENGL_COMMAND_CALLBACK_LAST(Command) Command
				NAZARA_OPENGL_FOREACH_COMMANDS(NAZARA_OPENGL_COMMAND_CALLBACK, NAZARA_OPENGL_COMMAND_CALLBACK_LAST)
#undef NAZARA_OPENGL_COMMAND_CALLBACK_LAST
#undef NAZARA_OPENGL_COMMAND_CALLBACK

			>;

			void ApplyBindings(const GL::Context& context, const ShaderBindings& bindings);
			void ApplyStates(const GL::Context& context, const DrawStates& states);

			inline void Execute(const GL::Context* context, const BeginDebugRegionCommand& command);
			inline void Execute(const GL::Context* context, const BlitTextureCommand& command);
			inline void Execute(const GL::Context* context, const BlitTextureToWindowCommand& command);
			inline void Execute(const GL::Context* context, const BuildTextureMipmapsCommand& command);
			inline void Execute(const GL::Context* context, const CopyBufferCommand& command);
			inline void Execute(const GL::Context* context, const CopyBufferFromMemoryCommand& command);
			inline void Execute(const GL::Context* context, const CopyTextureCommand& command);
			inline void Execute(const GL::Context* context, const DispatchCommand& command);
			inline void Execute(const GL::Context* context, const DrawCommand& command);
			inline void Execute(const GL::Context* context, const DrawIndexedCommand& command);
			inline void Execute(const GL::Context* context, const EndDebugRegionCommand& command);
			inline void Execute(const GL::Context* context, const InsertDebugLabelCommand& command);
			inline void Execute(const GL::Context* context, const MemoryBarrier& command);
			inline void Execute(const GL::Context*& context, const SetFrameBufferCommand& command);

			void Release() override;

			struct BeginDebugRegionCommand
			{
				std::string regionName;
				Color color;
			};

			struct BlitTextureCommand
			{
				const OpenGLTexture* source;
				const OpenGLTexture* target;
				Boxui sourceBox;
				Boxui targetBox;
				SamplerFilter filter;
			};

			struct BlitTextureToWindowCommand
			{
				const OpenGLTexture* source;
				Boxui sourceBox;
				Boxui targetBox;
				SamplerFilter filter;
			};

			struct BuildTextureMipmapsCommand
			{
				OpenGLTexture* texture;
				UInt8 baseLevel;
				UInt8 levelCount;
			};

			struct ComputeStates
			{
				const OpenGLComputePipeline* pipeline = nullptr;
			};

			struct CopyBufferCommand
			{
				GLuint source;
				GLuint target;
				UInt64 size;
				UInt64 sourceOffset;
				UInt64 targetOffset;
			};

			struct CopyTextureCommand
			{
				const OpenGLTexture* source;
				const OpenGLTexture* target;
				Boxui sourceBox;
				Vector3ui targetPoint;
			};

			struct CopyBufferFromMemoryCommand
			{
				const void* memory;
				GLuint target;
				UInt64 size;
				UInt64 targetOffset;
			};

			struct ShaderBinding
			{
				HybridVector<UInt32, 4> dynamicOffsets;
				const OpenGLRenderPipelineLayout* pipelineLayout;
				const OpenGLShaderBinding* shaderBinding;
			};

			struct ShaderBindings
			{
				std::vector<ShaderBinding> shaderBindings;
			};

			struct DispatchCommand
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

			struct DrawCommand
			{
				DrawStates states;
				ShaderBindings bindings;
				UInt32 firstInstance;
				UInt32 firstVertex;
				UInt32 instanceCount;
				UInt32 vertexCount;
			};

			struct DrawIndexedCommand
			{
				DrawStates states;
				ShaderBindings bindings;
				UInt32 baseVertex;
				UInt32 firstIndex;
				UInt32 firstInstance;
				UInt32 indexCount;
				UInt32 instanceCount;
			};

			struct EndDebugRegionCommand
			{
			};

			struct InsertDebugLabelCommand
			{
				std::string label;
				Color color;
			};

			struct MemoryBarrier
			{
				GLbitfield barriers;
			};

			struct SetFrameBufferCommand
			{
				std::array<CommandBufferBuilder::ClearValues, 16> clearValues; //< TODO: Remove hard limit?
				const OpenGLFramebuffer* framebuffer;
				const OpenGLRenderPass* renderpass;
			};

			using CommandData = TypeListInstantiate<CommandList, std::variant>;

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
