// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLComputePipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <cassert>
#include <stdexcept>

namespace Nz
{
	OpenGLComputePipeline::OpenGLComputePipeline(OpenGLDevice& device, GpuComputePipelineInfo pipelineInfo) :
	m_pipelineInfo(std::move(pipelineInfo))
	{
		if (!device.GetEnabledFeatures().computeShaders)
			throw std::runtime_error("compute shaders are not enabled on the device");

		OpenGLRenderPipelineLayout& pipelineLayout = SafeCast<OpenGLRenderPipelineLayout&>(*m_pipelineInfo.pipelineLayout);

		if (!m_program.Create(device))
			throw std::runtime_error("failed to create program");

		NazaraAssertMsg(m_pipelineInfo.shaderModule, "invalid shader module");

		OpenGLShaderModule& shaderModule = SafeCast<OpenGLShaderModule&>(*m_pipelineInfo.shaderModule);

		std::vector<OpenGLShaderModule::ExplicitBinding> explicitBindings;
		nzsl::ShaderStageTypeFlags stageFlags = shaderModule.Attach(m_program, pipelineLayout.GetShaderParameters(), &explicitBindings);
		if (!stageFlags.Test(nzsl::ShaderStageType::Compute))
			throw std::runtime_error("shader module has no compute stage");

		m_program.Link();

		std::string errLog;
		if (!m_program.GetLinkStatus(&errLog))
			throw std::runtime_error("failed to link program: " + errLog);

		OpenGLShaderModule::ApplyExplicitBindings(m_program, explicitBindings);
	}

	void OpenGLComputePipeline::Apply(const GL::Context& context) const
	{
		context.BindProgram(m_program.GetObjectId());
	}

	void OpenGLComputePipeline::UpdateDebugName(std::string_view name)
	{
		m_program.SetDebugName(name);
	}
}
