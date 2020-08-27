// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderStage.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderPipeline::OpenGLRenderPipeline(OpenGLDevice& device, RenderPipelineInfo pipelineInfo) :
	m_pipelineInfo(std::move(pipelineInfo))
	{
		if (!m_program.Create(device))
			throw std::runtime_error("failed to create program");

		for (const auto& shaderStagePtr : m_pipelineInfo.shaderStages)
		{
			OpenGLShaderStage& shaderStage = static_cast<OpenGLShaderStage&>(*shaderStagePtr);
			m_program.AttachShader(shaderStage.GetShader().GetObjectId());
		}

		m_program.Link();

		std::string errLog;
		if (!m_program.GetLinkStatus(&errLog))
			throw std::runtime_error("failed to link program: " + errLog);
	}

	void OpenGLRenderPipeline::Apply(const GL::Context& context) const
	{
		context.UpdateStates(m_pipelineInfo);
		context.BindProgram(m_program.GetObjectId()); //< Bind program after states
	}
}
