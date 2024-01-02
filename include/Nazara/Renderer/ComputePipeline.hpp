// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_COMPUTEPIPELINE_HPP
#define NAZARA_RENDERER_COMPUTEPIPELINE_HPP

#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/ShaderModule.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class RenderPipelineLayout;
	class ShaderModule;

	struct ComputePipelineInfo
	{
		std::shared_ptr<RenderPipelineLayout> pipelineLayout;
		std::shared_ptr<ShaderModule> shaderModule;
	};

	class NAZARA_RENDERER_API ComputePipeline
	{
		public:
			ComputePipeline() = default;
			virtual ~ComputePipeline();

			virtual const ComputePipelineInfo& GetPipelineInfo() const = 0;

			virtual void UpdateDebugName(std::string_view name) = 0;
	};
}

#include <Nazara/Renderer/ComputePipeline.inl>

#endif // NAZARA_RENDERER_COMPUTEPIPELINE_HPP
