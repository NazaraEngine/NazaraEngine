// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PIPELINEPASSLIST_HPP
#define NAZARA_GRAPHICS_PIPELINEPASSLIST_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Graphics/FramePassAttachment.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <array>
#include <limits>
#include <string>

namespace Nz
{
	struct NAZARA_GRAPHICS_API PipelinePassListParams : ResourceParameters
	{
		bool IsValid() const;
	};

	class FrameGraph;
	class PipelinePassList;

	using PipelinePassListLibrary = ObjectLibrary<PipelinePassList>;
	using PipelinePassListLoader = ResourceLoader<PipelinePassList, PipelinePassListParams>;

	class NAZARA_GRAPHICS_API PipelinePassList : public Resource
	{
		public:
			using Params = PipelinePassListParams;

			PipelinePassList() = default;
			PipelinePassList(const PipelinePassList&) = delete;
			PipelinePassList(PipelinePassList&&) = delete;
			~PipelinePassList() = default;

			inline std::size_t AddAttachment(FramePassAttachment attachment);
			inline std::size_t AddAttachmentProxy(std::string name, std::size_t attachmentIndex);
			inline std::size_t AddPass(std::string name, std::size_t implIndex, ParameterList parameterList = {});
			std::size_t AddPass(std::string name, std::string_view impl, ParameterList parameterList = {});

			std::vector<std::unique_ptr<FramePipelinePass>> BuildPasses(FramePipelinePass::PassData& passData) const;

			inline void EnablePassFlags(std::size_t passIndex, FramePipelinePassFlags flags);

			std::size_t RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph, std::optional<unsigned int> viewerIndex, const FunctionRef<void(std::size_t passIndex, FramePass& framePass, FramePipelinePassFlags flags)>& passCallback = nullptr) const;

			inline void SetFinalOutput(std::size_t attachmentIndex);

			inline void SetPassDepthClearValue(std::size_t passIndex, float depthValue);
			inline void SetPassDepthClearValue(std::size_t passIndex, FramePipelinePass::ViewerClearValue);
			inline void SetPassDepthStencilInput(std::size_t passIndex, std::size_t attachmentIndex);
			inline void SetPassDepthStencilOutput(std::size_t passIndex, std::size_t attachmentIndex);
			inline void SetPassInput(std::size_t passIndex, std::size_t inputIndex, std::size_t attachmentIndex);
			inline void SetPassOutput(std::size_t passIndex, std::size_t outputIndex, std::size_t attachmentIndex);
			inline void SetPassOutputClearColor(std::size_t passIndex, std::size_t outputIndex, const Color& clearColor);
			inline void SetPassOutputClearColor(std::size_t passIndex, std::size_t outputIndex, FramePipelinePass::ViewerClearValue);

			PipelinePassList& operator=(const PipelinePassList&) = delete;
			PipelinePassList& operator=(PipelinePassList&&) = delete;

			static std::shared_ptr<PipelinePassList> LoadFromFile(const std::filesystem::path& filePath, const PipelinePassListParams& params = PipelinePassListParams());
			static std::shared_ptr<PipelinePassList> LoadFromMemory(const void* data, std::size_t size, const PipelinePassListParams& params = PipelinePassListParams());
			static std::shared_ptr<PipelinePassList> LoadFromStream(Stream& stream, const PipelinePassListParams& params = PipelinePassListParams());

			static constexpr std::size_t MaxPassAttachment = 8;

		private:
			static constexpr std::size_t NoAttachment = std::numeric_limits<std::size_t>::max();

			struct AttachmentProxy
			{
				std::string name;
				std::size_t attachmentIndex;
			};

			struct PassOutput
			{
				std::size_t attachmentIndex = NoAttachment;
				std::variant<FramePipelinePass::DontClear, FramePipelinePass::ViewerClearValue, Color> clearColor;
			};

			struct Pass
			{
				std::size_t depthStencilInput = NoAttachment;
				std::size_t depthStencilOutput = NoAttachment;
				std::size_t implIndex;
				std::string name;
				std::variant<FramePipelinePass::DontClear, FramePipelinePass::ViewerClearValue, float> clearDepth;
				FixedVector<std::size_t /*attachmentIndex*/, MaxPassAttachment> inputs;
				FixedVector<PassOutput, MaxPassAttachment> outputs;
				FramePipelinePassFlags flags;
				ParameterList parameterList;
			};

			std::size_t m_finalOutputAttachment;
			std::vector<std::variant<FramePassAttachment, AttachmentProxy>> m_attachments;
			std::vector<Pass> m_passes;
	};
}

#include <Nazara/Graphics/PipelinePassList.inl>

#endif // NAZARA_GRAPHICS_PIPELINEPASSLIST_HPP
