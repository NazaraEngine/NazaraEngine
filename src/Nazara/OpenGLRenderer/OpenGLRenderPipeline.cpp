// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/Shader/GlslWriter.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <cassert>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderPipeline::OpenGLRenderPipeline(OpenGLDevice& device, RenderPipelineInfo pipelineInfo) :
	m_pipelineInfo(std::move(pipelineInfo)),
	m_isViewportFlipped(false)
	{
		if (!m_program.Create(device))
			throw std::runtime_error("failed to create program");

		ShaderStageTypeFlags stageFlags;

		for (const auto& shaderModulePtr : m_pipelineInfo.shaderModules)
		{
			OpenGLShaderModule& shaderModule = static_cast<OpenGLShaderModule&>(*shaderModulePtr);
			for (const auto& shaderEntry : shaderModule.GetShaders())
			{
				m_program.AttachShader(shaderEntry.shader.GetObjectId());
				stageFlags |= shaderEntry.stage;
			}
		}

		// OpenGL ES programs must have both vertex and fragment shaders or a compute shader or a mesh and fragment shader.
		if (device.GetReferenceContext().GetParams().type == GL::ContextType::OpenGL_ES)
		{
			auto GenerateIfMissing = [&](ShaderStageType stage)
			{
				if (!stageFlags.Test(stage))
				{
					ShaderAst::StatementPtr dummyAst = ShaderBuilder::DeclareFunction(stage, "main", {}, {});
					OpenGLShaderModule shaderModule(device, stage, dummyAst);
					for (const auto& shaderEntry : shaderModule.GetShaders())
					{
						m_program.AttachShader(shaderEntry.shader.GetObjectId());
						stageFlags |= shaderEntry.stage;
					}
				}
			};

			GenerateIfMissing(ShaderStageType::Fragment);
			GenerateIfMissing(ShaderStageType::Vertex);
		}

		m_program.Link();

		std::string errLog;
		if (!m_program.GetLinkStatus(&errLog))
			throw std::runtime_error("failed to link program: " + errLog);

		m_flipYUniformLocation = m_program.GetUniformLocation(GlslWriter::GetFlipYUniformName());
		if (m_flipYUniformLocation != -1)
			m_program.Uniform(m_flipYUniformLocation, 1.f);
	}

	void OpenGLRenderPipeline::Apply(const GL::Context& context, bool flipViewport) const
	{
		context.UpdateStates(m_pipelineInfo, flipViewport);
		context.BindProgram(m_program.GetObjectId()); //< Bind program after states (for shader caching)
		if (m_isViewportFlipped != flipViewport)
		{
			m_program.Uniform(m_flipYUniformLocation, (flipViewport) ? -1.f : 1.f);
			m_isViewportFlipped = flipViewport;
		}
	}
}
