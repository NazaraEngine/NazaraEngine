// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <NZSL/GlslWriter.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <NZSL/Ast/Module.hpp>
#include <cassert>
#include <stdexcept>

namespace Nz
{
	OpenGLRenderPipeline::OpenGLRenderPipeline(OpenGLDevice& device, RenderPipelineInfo pipelineInfo) :
	m_pipelineInfo(std::move(pipelineInfo)),
	m_isViewportFlipped(false)
	{
		ValidatePipelineInfo(device, m_pipelineInfo);

		OpenGLRenderPipelineLayout& pipelineLayout = SafeCast<OpenGLRenderPipelineLayout&>(*m_pipelineInfo.pipelineLayout);

		if (!m_program.Create(device))
			throw std::runtime_error("failed to create program");

		const GL::Context* activeContext = GL::Context::GetCurrentContext();
		assert(activeContext);

		// Enable pipeline states before compiling and linking the program, for drivers which embed some pipeline states into the shader binary (to avoid recompilation later)
		activeContext->UpdateStates(m_pipelineInfo, false);

		nzsl::ShaderStageTypeFlags stageFlags;
		std::vector<OpenGLShaderModule::ExplicitBinding> explicitBindings;

		for (const auto& shaderModulePtr : m_pipelineInfo.shaderModules)
		{
			OpenGLShaderModule& shaderModule = SafeCast<OpenGLShaderModule&>(*shaderModulePtr);
			stageFlags |= shaderModule.Attach(m_program, pipelineLayout.GetShaderParameters(), &explicitBindings);
		}

		// OpenGL ES programs must have both vertex and fragment shaders or a compute shader or a mesh and fragment shader.
		if (device.GetReferenceContext().GetParams().type == GL::ContextType::OpenGL_ES)
		{
			auto GenerateIfMissing = [&](nzsl::ShaderStageType stage)
			{
				if (!stageFlags.Test(stage))
				{
					nzsl::Ast::Module dummyModule(100);
					dummyModule.rootNode = nzsl::ShaderBuilder::MultiStatement();
					dummyModule.rootNode->statements.push_back(nzsl::ShaderBuilder::DeclareFunction(stage, "main", {}, {}));

					OpenGLShaderModule shaderModule(device, stage, dummyModule);
					stageFlags |= shaderModule.Attach(m_program, pipelineLayout.GetShaderParameters(), &explicitBindings);
				}
			};

			GenerateIfMissing(nzsl::ShaderStageType::Fragment);
			GenerateIfMissing(nzsl::ShaderStageType::Vertex);
		}

		m_program.Link();

		std::string errLog;
		if (!m_program.GetLinkStatus(&errLog))
			throw std::runtime_error("failed to link program: " + errLog);

		m_flipYUniformLocation = m_program.GetUniformLocation(nzsl::GlslWriter::GetFlipYUniformName().data());
		if (m_flipYUniformLocation != -1)
			m_program.Uniform(m_flipYUniformLocation, 1.f);

		for (const auto& explicitBinding : explicitBindings)
		{
			if (explicitBinding.isBlock)
			{
				GLuint blockIndex = m_program.GetUniformBlockIndex(explicitBinding.name);
				if (blockIndex == GL_INVALID_INDEX)
					continue;

				m_program.UniformBlockBinding(blockIndex, explicitBinding.binding);
			}
			else
			{
				int location = m_program.GetUniformLocation(explicitBinding.name);
				if (location == -1)
					continue;

				m_program.Uniform(location, SafeCast<int>(explicitBinding.binding));
			}
		}
	}

	void OpenGLRenderPipeline::Apply(const GL::Context& context, bool flipViewport) const
	{
		context.UpdateStates(m_pipelineInfo, flipViewport);
		context.BindProgram(m_program.GetObjectId()); //< Bind program after states (for shader caching)
		if (m_flipYUniformLocation != -1 && m_isViewportFlipped != flipViewport)
		{
			m_program.Uniform(m_flipYUniformLocation, (flipViewport) ? -1.f : 1.f);
			m_isViewportFlipped = flipViewport;
		}
	}

	void OpenGLRenderPipeline::UpdateDebugName(std::string_view name)
	{
		m_program.SetDebugName(name);
	}
}
