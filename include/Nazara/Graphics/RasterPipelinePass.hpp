// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <string>

namespace Nz
{
	class ComputePipeline;
	class ElementRendererRegistry;
	class RenderPipelineLayout;
	class RenderQueue;
	class UberShader;

	class NAZARA_GRAPHICS_API RasterPipelinePass : public FramePipelinePass
	{
		public:
			inline RasterPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters);
			RasterPipelinePass(PassData& passData, std::string passName, const std::vector<std::string_view>& renderQueueNames, UInt32 renderMask = MaxValue());
			RasterPipelinePass(const RasterPipelinePass&) = delete;
			RasterPipelinePass(RasterPipelinePass&&) = delete;
			~RasterPipelinePass() = default;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			RasterPipelinePass& operator=(const RasterPipelinePass&) = delete;
			RasterPipelinePass& operator=(RasterPipelinePass&&) = delete;

			static UInt32 GetRenderMask(const ParameterList& parameters);
			static std::vector<std::string_view> GetRenderQueues(const ParameterList& parameters);

		private:
			void BuildCullingPipeline();
			std::size_t GetRenderQueueHash() const;

			std::shared_ptr<ComputePipeline> m_computePipeline;
			std::shared_ptr<RenderPipelineLayout> m_computePipelineLayout;
			std::shared_ptr<UberShader> m_frustumCullingShader;
			std::size_t m_renderQueueHash;
			std::string m_passName;
			std::vector<std::unique_ptr<ElementRendererData>> m_elementRendererData;
			HybridVector<RenderQueue*, 2> m_renderQueues;
			AbstractViewer* m_viewer;
			ElementRendererRegistry& m_elementRegistry;
			FramePipeline& m_pipeline;
			UInt32 m_renderMask;
	};
}

#include <Nazara/Graphics/RasterPipelinePass.inl>

#endif // NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP
