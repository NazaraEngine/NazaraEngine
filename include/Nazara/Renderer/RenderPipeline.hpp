// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERPIPELINE_HPP
#define NAZARA_RENDERPIPELINE_HPP

#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/RenderStates.hpp>

namespace Nz
{
	struct RenderPipelineInfo : RenderStates
	{
		struct VertexBufferData
		{
			std::size_t binding;
			VertexDeclarationConstRef declaration;
		};

		std::shared_ptr<RenderPipelineLayout> pipelineLayout;
		std::vector<std::shared_ptr<ShaderStageImpl>> shaderStages;
		std::vector<VertexBufferData> vertexBuffers;
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
