// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERPIPELINE_HPP
#define NAZARA_RENDERPIPELINE_HPP

#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
//#include <Nazara/Renderer/Shader.hpp>

namespace Nz
{
	struct RenderPipelineInfo : RenderStates
	{
		/*ShaderConstRef shader;*/
	};

	class NAZARA_RENDERER_API RenderPipeline
	{
		public:
			RenderPipeline() = default;
			virtual ~RenderPipeline();
	};
}

#include <Nazara/Renderer/RenderPipeline.inl>

#endif // NAZARA_RENDERPIPELINE_HPP
