// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPASS_HPP
#define NAZARA_GRAPHICS_FRAMEPASS_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <functional>
#include <limits>
#include <optional>
#include <string>

namespace Nz
{
	class BakedFrameGraph;
	class CommandBufferBuilder;
	class FrameGraph;
	class RenderFrame;

	enum class FramePassExecution
	{
		Execute,
		Skip,
		UpdateAndExecute
	};

	struct FramePassEnvironment
	{
		BakedFrameGraph& frameGraph;
		Recti renderRect;
		RenderFrame& renderFrame;
	};

	class NAZARA_GRAPHICS_API FramePass
	{
		public:
			using CommandCallback = std::function<void(CommandBufferBuilder& builder, const FramePassEnvironment& env)>;
			using ExecutionCallback = std::function<FramePassExecution()>;
			struct DepthStencilClear;
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
			inline std::size_t GetDepthStencilInput() const;
			inline std::size_t GetDepthStencilOutput() const;
			inline const ExecutionCallback& GetExecutionCallback() const;
			inline const std::vector<Input>& GetInputs() const;
			inline const std::string& GetName() const;
			inline const std::vector<Output>& GetOutputs() const;
			inline std::size_t GetPassId() const;

			inline void SetCommandCallback(CommandCallback callback);
			inline void SetClearColor(std::size_t outputIndex, const std::optional<Color>& color);
			inline void SetDepthStencilClear(float depth, UInt32 stencil);
			inline void SetDepthStencilInput(std::size_t attachmentId);
			inline void SetDepthStencilOutput(std::size_t attachmentId);
			inline void SetExecutionCallback(ExecutionCallback callback);
			inline void SetInputLayout(std::size_t inputIndex, TextureLayout layout);
			inline void SetReadInput(std::size_t inputIndex, bool doesRead);

			FramePass& operator=(const FramePass&) = delete;
			FramePass& operator=(FramePass&&) = delete;

			static constexpr std::size_t InvalidAttachmentId = std::numeric_limits<std::size_t>::max();

			struct DepthStencilClear
			{
				float depth;
				UInt32 stencil;
			};

			struct Input
			{
				std::size_t attachmentId;
				std::optional<TextureLayout> assumedLayout;
				bool doesRead = true;
			};

			struct Output
			{
				std::size_t attachmentId;
				std::optional<Color> clearColor;
			};

		private:
			std::optional<DepthStencilClear> m_depthStencilClear;
			std::size_t m_depthStencilInput;
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
