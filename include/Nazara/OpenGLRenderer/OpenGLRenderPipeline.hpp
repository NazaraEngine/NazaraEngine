// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Program.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <Nazara/Utils/MovablePtr.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLRenderPipeline : public RenderPipeline
	{
		public:
			OpenGLRenderPipeline(OpenGLDevice& device, RenderPipelineInfo pipelineInfo);
			~OpenGLRenderPipeline() = default;

			void Apply(const GL::Context& context, bool flipViewport) const;

			inline const RenderPipelineInfo& GetPipelineInfo() const override;

			void UpdateDebugName(std::string_view name) override;

		private:
			RenderPipelineInfo m_pipelineInfo;
			GL::Program m_program;
			GLint m_flipYUniformLocation;
			mutable bool m_isViewportFlipped;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP
