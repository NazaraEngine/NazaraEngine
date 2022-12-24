// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLCOMPUTEPIPELINE_HPP
#define NAZARA_OPENGLRENDERER_OPENGLCOMPUTEPIPELINE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Program.hpp>
#include <Nazara/Renderer/ComputePipeline.hpp>
#include <Nazara/Utils/MovablePtr.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLComputePipeline : public ComputePipeline
	{
		public:
			OpenGLComputePipeline(OpenGLDevice& device, ComputePipelineInfo pipelineInfo);
			~OpenGLComputePipeline() = default;

			void Apply(const GL::Context& context) const;

			inline const ComputePipelineInfo& GetPipelineInfo() const override;

			void UpdateDebugName(std::string_view name) override;

		private:
			ComputePipelineInfo m_pipelineInfo;
			GL::Program m_program;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLComputePipeline.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLCOMPUTEPIPELINE_HPP
