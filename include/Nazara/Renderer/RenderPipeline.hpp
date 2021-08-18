// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERPIPELINE_HPP
#define NAZARA_RENDERPIPELINE_HPP

#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	struct RenderPipelineInfo : RenderStates
	{
		struct VertexBufferData
		{
			std::size_t binding;
			std::shared_ptr<const VertexDeclaration> declaration;
		};

		std::shared_ptr<RenderPipelineLayout> pipelineLayout;
		std::vector<std::shared_ptr<ShaderModule>> shaderModules;
		std::vector<VertexBufferData> vertexBuffers;
	};

	class RenderDevice;

	class NAZARA_RENDERER_API RenderPipeline
	{
		public:
			RenderPipeline() = default;
			virtual ~RenderPipeline();

			virtual const RenderPipelineInfo& GetPipelineInfo() const = 0;

		protected:
			static void ValidatePipelineInfo(const RenderDevice& device, RenderPipelineInfo& pipelineInfo);
	};
}

#include <Nazara/Renderer/RenderPipeline.inl>

#endif // NAZARA_RENDERPIPELINE_HPP
