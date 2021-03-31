// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderPipeline::OpenGLRenderPipeline(OpenGLDevice& device, RenderPipelineInfo pipelineInfo) :
	m_pipelineInfo(std::move(pipelineInfo)),
	m_isYFlipped(false)
	{
		if (!m_program.Create(device))
			throw std::runtime_error("failed to create program");

		for (const auto& shaderModulePtr : m_pipelineInfo.shaderModules)
		{
			OpenGLShaderModule& shaderModule = static_cast<OpenGLShaderModule&>(*shaderModulePtr);
			for (const GL::Shader& shader : shaderModule.GetShaders())
				m_program.AttachShader(shader.GetObjectId());
		}

		m_program.Link();

		std::string errLog;
		if (!m_program.GetLinkStatus(&errLog))
			throw std::runtime_error("failed to link program: " + errLog);

		m_flipYUniformLocation = m_program.GetUniformLocation(GlslWriter::GetFlipYUniformName());
		if (m_flipYUniformLocation != -1)
			m_program.Uniform(m_flipYUniformLocation, 1.f);
	}

	void OpenGLRenderPipeline::Apply(const GL::Context& context) const
	{
		context.UpdateStates(m_pipelineInfo);
		context.BindProgram(m_program.GetObjectId()); //< Bind program after states
	}

	void OpenGLRenderPipeline::FlipY(bool shouldFlipY) const
	{
		if (m_isYFlipped != shouldFlipY)
		{
			m_program.Uniform(m_flipYUniformLocation, (shouldFlipY) ? -1.f : 1.f);
			m_isYFlipped = shouldFlipY;
		}
	}
}
