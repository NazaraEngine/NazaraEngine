// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP
#define NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ParameterList.hpp>
#include <Nazara/Graphics/BaseElementRenderPipelinePass.hpp>

namespace Nz
{
	class NAZARA_GRAPHICS_API RasterPipelinePass : public BaseElementRenderPipelinePass
	{
		public:
			inline RasterPipelinePass(PassData& passData, std::string passName, const ParameterList& parameters);
			inline RasterPipelinePass(PassData& passData, std::string passName, std::size_t materialPassIndex, UInt32 renderMask = MaxValue());
			RasterPipelinePass(const RasterPipelinePass&) = delete;
			RasterPipelinePass(RasterPipelinePass&&) = delete;
			~RasterPipelinePass() = default;

			FramePass& RegisterToFrameGraph(FrameGraph& frameGraph, const PassInputOuputs& inputOuputs) override;

			RasterPipelinePass& operator=(const RasterPipelinePass&) = delete;
			RasterPipelinePass& operator=(RasterPipelinePass&&) = delete;

			static std::size_t GetMaterialPassIndex(const ParameterList& parameters);
			static UInt32 GetRenderMask(const ParameterList& parameters);

		private:
			std::string m_passName;
			AbstractViewer* m_viewer;
			FramePipeline& m_pipeline;
	};
}

#include <Nazara/Graphics/RasterPipelinePass.inl>

#endif // NAZARA_GRAPHICS_RASTERPIPELINEPASS_HPP
