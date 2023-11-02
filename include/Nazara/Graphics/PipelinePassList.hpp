// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PIPELINEPASSLIST_HPP
#define NAZARA_GRAPHICS_PIPELINEPASSLIST_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <array>
#include <limits>
#include <string>

namespace Nz
{
	class FrameGraph;

	class NAZARA_GRAPHICS_API PipelinePassList
	{
		public:
			PipelinePassList() = default;
			PipelinePassList(const PipelinePassList&) = delete;
			PipelinePassList(PipelinePassList&&) = delete;
			~PipelinePassList() = default;

			inline std::size_t AddAttachment(FramePassAttachment attachment);
			inline std::size_t AddPass(std::string name, std::size_t implIndex, ParameterList parameterList = {});

			std::vector<std::unique_ptr<FramePipelinePass>> BuildPasses(FramePipelinePass::PassData& passData) const;

			std::size_t RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph) const;

			inline void SetFinalOutput(std::size_t attachmentIndex);
			inline void SetPassInput(std::size_t passIndex, std::size_t inputIndex, std::size_t attachmentIndex);
			inline void SetPassOutput(std::size_t passIndex, std::size_t outputIndex, std::size_t attachmentIndex);
			inline void SetPassDepthStencilInput(std::size_t passIndex, std::size_t attachmentIndex);
			inline void SetPassDepthStencilOutput(std::size_t passIndex, std::size_t attachmentIndex);

			PipelinePassList& operator=(const PipelinePassList&) = delete;
			PipelinePassList& operator=(PipelinePassList&&) = delete;

			static constexpr std::size_t MaxPassAttachment = 8;

		private:
			static constexpr std::size_t NoAttachment = std::numeric_limits<std::size_t>::max();

			struct Pass
			{
				FixedVector<std::size_t /*attachmentIndex*/, MaxPassAttachment> inputs;
				FixedVector<std::size_t /*attachmentIndex*/, MaxPassAttachment> outputs;
				std::size_t depthStencilInput = NoAttachment;
				std::size_t depthStencilOutput = NoAttachment;
				std::size_t implIndex;
				std::string name;
				ParameterList parameterList;
			};

			std::size_t m_finalOutputAttachment;
			std::vector<FramePassAttachment> m_attachments;
			std::vector<Pass> m_passes;
	};
}

#include <Nazara/Graphics/PipelinePassList.inl>

#endif // NAZARA_GRAPHICS_PIPELINEPASSLIST_HPP
