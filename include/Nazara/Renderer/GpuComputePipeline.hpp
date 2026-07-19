// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_COMPUTEPIPELINE_HPP
#define NAZARA_RENDERER_COMPUTEPIPELINE_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Renderer/GpuPipelineLayout.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>

namespace Nz
{
	class GpuPipelineLayout;
	class ShaderModule;

	struct GpuComputePipelineInfo
	{
		std::shared_ptr<GpuPipelineLayout> pipelineLayout;
		std::shared_ptr<ShaderModule> shaderModule;
	};

	class NAZARA_RENDERER_API GpuComputePipeline
	{
		public:
			GpuComputePipeline() = default;
			virtual ~GpuComputePipeline();

			virtual const GpuComputePipelineInfo& GetPipelineInfo() const = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;
	};
}

#include <Nazara/Renderer/GpuComputePipeline.inl>

#endif // NAZARA_RENDERER_COMPUTEPIPELINE_HPP
