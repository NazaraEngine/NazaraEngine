// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_PIPELINEVIEWER_HPP
#define NAZARA_GRAPHICS_PIPELINEVIEWER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/FramePipelinePass.hpp>
#include <NazaraUtils/FunctionRef.hpp>
#include <vector>

namespace Nz
{
	class FramePass;

	class NAZARA_GRAPHICS_API PipelineViewer : public AbstractViewer
	{
		public:
			PipelineViewer() = default;
			PipelineViewer(const PipelineViewer&) = delete;
			PipelineViewer(PipelineViewer&&) = delete;
			~PipelineViewer() = default;

			virtual std::vector<std::unique_ptr<FramePipelinePass>> BuildPasses(FramePipelinePass::PassData& passData) const = 0;

			virtual std::size_t RegisterPasses(const std::vector<std::unique_ptr<FramePipelinePass>>& passes, FrameGraph& frameGraph, std::optional<unsigned int> viewerIndex, const FunctionRef<void(std::size_t passIndex, FramePass& framePass, FramePipelinePassFlags flags)>& passCallback = nullptr) const = 0;

			PipelineViewer& operator=(const PipelineViewer&) = delete;
			PipelineViewer& operator=(PipelineViewer&&) = delete;
	};
}

#include <Nazara/Graphics/PipelineViewer.inl>

#endif // NAZARA_GRAPHICS_PIPELINEVIEWER_HPP
