// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLRENDERPIPELINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Program.hpp>
#include <Nazara/Renderer/RenderPipeline.hpp>
#include <NazaraUtils/MovablePtr.hpp>
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
