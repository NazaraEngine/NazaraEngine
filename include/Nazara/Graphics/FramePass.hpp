// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPASS_HPP
#define NAZARA_GRAPHICS_FRAMEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/Constants.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Enums.hpp>
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
			struct Input;
			struct Output;

			inline FramePass(FrameGraph& owner, std::size_t passId, std::string name);
			FramePass(const FramePass&) = delete;
			FramePass(FramePass&&) noexcept = default;
			~FramePass() = default;

			inline std::size_t AddInput(std::size_t attachmentId);
			inline std::size_t AddOutput(std::size_t attachmentId);

			template<typename F> void ForEachAttachment(F&& func, bool singleDSInputOutputCall = true) const;

			inline const CommandCallback& GetCommandCallback() const;
			inline const std::optional<DepthStencilClear>& GetDepthStencilClear() const;
			inline const std::optional<DepthStencilInput>& GetDepthStencilInput() const;
			inline std::size_t GetDepthStencilOutput() const;
			inline const ExecutionCallback& GetExecutionCallback() const;
			inline const std::vector<Input>& GetInputs() const;
			inline const std::string& GetName() const;
			inline const std::vector<Output>& GetOutputs() const;
			inline std::size_t GetPassId() const;

			inline void SetCommandCallback(CommandCallback callback);
			inline void SetClearColor(std::size_t outputIndex, const std::optional<Color>& color);
			inline void SetDepthStencilClear(float depth, UInt32 stencil);
			inline void SetDepthStencilInput(std::size_t attachmentId, TextureUsage attachmentUsage = TextureUsage::DepthStencilAttachment);
			inline void SetDepthStencilOutput(std::size_t attachmentId);
			inline void SetExecutionCallback(ExecutionCallback callback);
			inline void SetInputAccess(std::size_t inputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags);
			inline void SetInputAssumedLayout(std::size_t inputIndex, TextureLayout layout);
			inline void SetInputUsage(std::size_t inputIndex, TextureUsageFlags usageFlags);
			inline void SetOutputAccess(std::size_t inputIndex, TextureLayout layout, PipelineStageFlags stageFlags, MemoryAccessFlags accessFlags);
			inline void SetOutputUsage(std::size_t inputIndex, TextureUsageFlags usageFlags);
			inline void SetReadInput(std::size_t inputIndex, bool doesRead);

			FramePass& operator=(const FramePass&) = delete;
			FramePass& operator=(FramePass&&) = delete;

			static constexpr std::size_t InvalidAttachmentId = MaxValue();

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

			struct Input
			{
				std::optional<TextureLayout> assumedLayout;
				std::size_t attachmentId;
				MemoryAccessFlags accessFlags = MemoryAccess::ShaderRead;
				PipelineStageFlags stageFlags = PipelineStage::FragmentShader;
				TextureLayout layout = TextureLayout::ColorInput;
				TextureUsageFlags textureUsageFlags = TextureUsage::ShaderSampling;
				bool doesRead = true;
			};

			struct Output
			{
				std::optional<Color> clearColor;
				std::size_t attachmentId;
				MemoryAccessFlags accessFlags = MemoryAccess::ColorWrite;
				PipelineStageFlags stageFlags = PipelineStage::ColorOutput;
				TextureLayout layout = TextureLayout::ColorOutput;
				TextureUsageFlags textureUsageFlags = TextureUsage::ColorAttachment;
			};

		private:
			std::optional<DepthStencilClear> m_depthStencilClear;
			std::optional<DepthStencilInput> m_depthStencilInput;
			std::size_t m_depthStencilOutput;
			std::size_t m_passId;
			std::string m_name;
			std::vector<Input> m_inputs;
			std::vector<Output> m_outputs;
			CommandCallback m_commandCallback;
			ExecutionCallback m_executionCallback;
	};
}

#include <Nazara/Graphics/FramePass.inl>

#endif // NAZARA_GRAPHICS_FRAMEPASS_HPP
