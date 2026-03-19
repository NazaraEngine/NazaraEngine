// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPASS_HPP
#define NAZARA_GRAPHICS_FRAMEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <NazaraUtils/Constants.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <functional>
#include <optional>
#include <string>

namespace Nz
{
	class BakedFrameGraph;
	class CommandBufferBuilder;
	class FrameGraph;
	class RenderResources;

	enum class FramePassExecution
	{
		Execute,
		Skip,
		UpdateAndExecute
	};

	struct FramePassEnvironment
	{
		BakedFrameGraph& frameGraph;
		RenderResources& renderResources;
		Recti renderRect;
	};

	class NAZARA_GRAPHICS_API FramePass
	{
		public:
			using CommandCallback = std::function<void(CommandBufferBuilder& builder, const FramePassEnvironment& env)>;
			using ExecutionCallback = std::function<FramePassExecution()>;
			struct DepthStencilClear;
			struct DepthStencilInput;
			struct AttachmentInput;
			struct AttachmentOutput;
			using AttachmentInputs = Nz::HybridVector<AttachmentInput, 2>;
			using AttachmentOutputs = Nz::HybridVector<AttachmentOutput, 2>;

			inline FramePass(FrameGraph& owner, std::size_t passId, std::string name);
			FramePass(const FramePass&) = delete;
			FramePass(FramePass&&) noexcept = default;
			~FramePass() = default;

			inline std::size_t AddInputAttachment(std::size_t attachmentId);
			inline std::size_t AddInputBuffer(std::size_t bufferId, BufferUsageFlags bufferUsage, MemoryAccessFlags accessFlags, PipelineStageFlags stageFlags);

			inline std::size_t AddInputIndexBuffer(std::size_t bufferId, BufferUsageFlags bufferUsage = BufferUsage::IndexBuffer, MemoryAccessFlags accessFlags = MemoryAccess::IndexBufferRead, PipelineStageFlags stageFlags = PipelineStage::VertexInput);
			inline std::size_t AddInputStorageBuffer(std::size_t bufferId, BufferUsageFlags bufferUsage = BufferUsage::StorageBuffer, MemoryAccessFlags accessFlags = MemoryAccess::ShaderRead, PipelineStageFlags stageFlags = PipelineStage::ComputeShader);
			inline std::size_t AddInputVertexBuffer(std::size_t bufferId, BufferUsageFlags bufferUsage = BufferUsage::VertexBuffer, MemoryAccessFlags accessFlags = MemoryAccess::VertexBufferRead, PipelineStageFlags stageFlags = PipelineStage::VertexInput);

			inline std::size_t AddOutputAttachment(std::size_t attachmentId);
			inline std::size_t AddOutputBuffer(std::size_t bufferId, BufferUsageFlags bufferUsage, MemoryAccessFlags accessFlags, PipelineStageFlags stageFlags);

			template<typename F> void ForEachAttachment(F&& func, bool singleDSInputOutputCall = true) const;

			inline const AttachmentInputs& GetAttachmentInputs() const;
			inline const AttachmentOutputs& GetAttachmentOutputs() const;
			inline const CommandCallback& GetCommandCallback() const;
			inline const std::optional<DepthStencilClear>& GetDepthStencilClear() const;
			inline const std::optional<DepthStencilInput>& GetDepthStencilInput() const;
			inline std::size_t GetDepthStencilOutput() const;
			inline const ExecutionCallback& GetExecutionCallback() const;
			inline const std::string& GetName() const;
			inline std::size_t GetPassId() const;

			inline void SetAttachmentInputAccess(std::size_t inputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags);
			inline void SetAttachmentInputAssumedLayout(std::size_t inputIndex, TextureLayout layout);
			inline void SetAttachmentInputUsage(std::size_t inputIndex, TextureUsageFlags usageFlags);
			inline void SetAttachmentOutputAccess(std::size_t inputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags);
			inline void SetAttachmentOutputUsage(std::size_t inputIndex, TextureUsageFlags usageFlags);
			inline void SetAttachmentReadInput(std::size_t inputIndex, bool doesRead);

			inline void SetCommandCallback(CommandCallback callback);
			inline void SetClearColor(std::size_t outputIndex, const std::optional<Color>& color);
			inline void SetDepthStencilClear(float depth, UInt32 stencil);
			inline void SetDepthStencilInput(std::size_t attachmentId, TextureUsage attachmentUsage = TextureUsage::DepthStencilAttachment);
			inline void SetDepthStencilOutput(std::size_t attachmentId);
			inline void SetExecutionCallback(ExecutionCallback callback);

			FramePass& operator=(const FramePass&) = delete;
			FramePass& operator=(FramePass&&) = delete;

			static constexpr std::size_t InvalidAttachmentId = MaxValue();

			struct AttachmentInput
			{
				std::optional<TextureLayout> assumedLayout;
				std::size_t attachmentId;
				MemoryAccessFlags accessFlags = MemoryAccess::ShaderRead;
				PipelineStageFlags stageFlags = PipelineStage::FragmentShader;
				TextureLayout layout = TextureLayout::ColorInput;
				TextureUsageFlags textureUsageFlags = TextureUsage::ShaderSampling;
				bool doesRead = true;
			};

			struct AttachmentOutput
			{
				std::optional<Color> clearColor;
				std::size_t attachmentId;
				MemoryAccessFlags accessFlags = MemoryAccess::ColorWrite;
				PipelineStageFlags stageFlags = PipelineStage::ColorOutput;
				TextureLayout layout = TextureLayout::ColorOutput;
				TextureUsageFlags textureUsageFlags = TextureUsage::ColorAttachment;
			};

			struct BufferInput
			{
				BufferUsageFlags bufferUsage;
				MemoryAccessFlags accessFlags = MemoryAccess::ShaderRead;
				PipelineStageFlags stageFlags = PipelineStage::FragmentShader;
			};

			struct BufferOutput
			{
				BufferUsageFlags bufferUsage;
				MemoryAccessFlags accessFlags = MemoryAccess::ShaderWrite;
				PipelineStageFlags stageFlags = PipelineStage::ComputeShader;
			};

			struct DepthStencilClear
			{
				float depth;
				UInt32 stencil;
			};

			struct DepthStencilInput
			{
				std::size_t attachmentId;
				TextureUsage textureUsageFlags = TextureUsage::DepthStencilAttachment;
			};

		private:
			std::optional<DepthStencilClear> m_depthStencilClear;
			std::optional<DepthStencilInput> m_depthStencilInput;
			std::size_t m_depthStencilOutput;
			std::size_t m_passId;
			std::string m_name;
			AttachmentInputs m_attachmentInputs;
			AttachmentOutputs m_attachmentOutputs;
			CommandCallback m_commandCallback;
			ExecutionCallback m_executionCallback;
	};
}

#include <Nazara/Graphics/FramePass.inl>

#endif // NAZARA_GRAPHICS_FRAMEPASS_HPP
