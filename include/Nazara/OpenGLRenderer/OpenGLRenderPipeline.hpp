// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Program.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderPipeline : public RenderPipeline
	{
		public:
			OpenGLRenderPipeline(OpenGLDevice& device, RenderPipelineInfo pipelineInfo);
			~OpenGLRenderPipeline() = default;

			void Apply(const GL::Context& context) const;

			inline const RenderPipelineInfo& GetPipelineInfo() const;

		private:
			RenderPipelineInfo m_pipelineInfo;
			GL::Program m_program;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP
