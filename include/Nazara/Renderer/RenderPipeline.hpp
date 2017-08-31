// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERPIPELINE_HPP
#define NAZARA_RENDERPIPELINE_HPP

#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Renderer/RenderStates.hpp>
#include <Nazara/Renderer/Shader.hpp>

namespace Nz
{
	struct RenderPipelineInfo : RenderStates
	{
		ShaderConstRef shader;
	};

	class RenderPipeline
	{
		public:
			inline RenderPipeline();
			inline ~RenderPipeline();

			inline bool Create(const RenderPipelineInfo& pipelineInfo);
			inline void Destroy();

			inline const RenderPipelineInfo& GetInfo() const;

			inline bool IsValid() const;

		private:
			RenderPipelineInfo m_pipelineInfo;
			bool m_valid;
	};
}

#include <Nazara/Renderer/RenderPipeline.inl>

#endif // NAZARA_RENDERPIPELINE_HPP
