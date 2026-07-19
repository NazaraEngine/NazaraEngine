// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <NazaraUtils/Constants.hpp>
#include <span>
#include <variant>

namespace Nz
{
	class AbstractViewer;
	class ElementRendererRegistry;
	class FrameGraph;
	class FramePass;
	class FramePipeline;
	class GpuResources;

	class NAZARA_GRAPHICS_API FramePipelinePass
	{
		public:
			struct FrameData;
			struct PassData;
			struct PassInputOuputs;

			FramePipelinePass() = default;
			FramePipelinePass(const FramePipelinePass&) = delete;
			FramePipelinePass(FramePipelinePass&&) = delete;
			virtual ~FramePipelinePass();

			virtual void Prepare(FrameData& frameData);

			virtual FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) = 0;

			FramePipelinePass& operator=(const FramePipelinePass&) = delete;
			FramePipelinePass& operator=(FramePipelinePass&&) = delete;

			struct DontClear {};
			struct ViewerClearValue {};

			struct FrameData
			{
				const Bitset<UInt64>* visibleLights;
				const Frustumf& frustum;
				GpuResources& renderResources;
			};

			struct PassData
			{
				AbstractViewer* viewer;
				ElementRendererRegistry& elementRegistry;
				FramePipeline& pipeline;
			};

			struct PassInputData
			{
				std::size_t attachmentIndex;
			};

			struct PassOutputData
			{
				std::size_t attachmentIndex;
				std::variant<DontClear, ViewerClearValue, Color> clearColor;
			};

			struct PassInputOuputs
			{
				std::span<const PassInputData> inputAttachments;
				std::span<const PassOutputData> outputAttachments;
				std::size_t depthStencilInput = InvalidAttachmentIndex;
				std::size_t depthStencilOutput = InvalidAttachmentIndex;
				std::variant<DontClear, ViewerClearValue, float> clearDepth;
			};

			static constexpr std::size_t InvalidAttachmentIndex = MaxValue();
	};
}

#include <Nazara/Graphics/FramePipelinePass.inl>

#endif // NAZARA_GRAPHICS_FRAMEPIPELINEPASS_HPP
