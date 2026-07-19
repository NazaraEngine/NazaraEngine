// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMPUTEPIPELINE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMPUTEPIPELINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Program.hpp>
#include <Nazara/Renderer/GpuComputePipeline.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLComputePipeline : public GpuComputePipeline
	{
		public:
			OpenGLComputePipeline(OpenGLDevice& device, GpuComputePipelineInfo pipelineInfo);
			~OpenGLComputePipeline() = default;

			void Apply(const GL::Context& context) const;

			inline const GpuComputePipelineInfo& GetPipelineInfo() const override;

			void UpdateDebugName(std::string_view name) override;

		private:
			GpuComputePipelineInfo m_pipelineInfo;
			GL::Program m_program;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLComputePipeline.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMPUTEPIPELINE_HPP
