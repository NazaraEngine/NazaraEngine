// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PIPELINEVIEWER_HPP
#define NAZARA_GRAPHICS_PIPELINEVIEWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_GRAPHICS_API PipelineViewer : public AbstractViewer
	{
		public:
			PipelineViewer() = default;
			PipelineViewer(const PipelineViewer&) = delete;
			PipelineViewer(PipelineViewer&&) = delete;
			~PipelineViewer() = default;

			virtual std::vector<std::unique_ptr<FramePipelinePass>> BuildPasses(FramePipelinePass::PassData& passData) const = 0;

			virtual std::size_t RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph) const = 0;

			PipelineViewer& operator=(const PipelineViewer&) = delete;
			PipelineViewer& operator=(PipelineViewer&&) = delete;
	};
}

#include <Nazara/Graphics/PipelineViewer.inl>

#endif // NAZARA_GRAPHICS_PIPELINEVIEWER_HPP
