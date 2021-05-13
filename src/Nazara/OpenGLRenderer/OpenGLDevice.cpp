// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLDevice::OpenGLDevice(GL::Loader& loader) :
	m_loader(loader)
	{
		m_referenceContext = loader.CreateContext(this, {});
		if (!m_referenceContext)
			throw std::runtime_error("failed to create reference context");

		if (!GL::Context::SetCurrentContext(m_referenceContext.get()))
			throw std::runtime_error("failed to activate reference context");

		const GLubyte* vendorStr = m_referenceContext->glGetString(GL_VENDOR);
		const GLubyte* rendererStr = m_referenceContext->glGetString(GL_RENDERER);

		m_deviceInfo.name = "OpenGL Device (";

		if (vendorStr)
			m_deviceInfo.name.append(reinterpret_cast<const char*>(vendorStr));

		if (rendererStr)
		{
			if (vendorStr)
				m_deviceInfo.name += " - ";

			m_deviceInfo.name.append(reinterpret_cast<const char*>(rendererStr));
		}

		m_deviceInfo.name += ')';

		m_deviceInfo.type = RenderDeviceType::Unknown;

		GLint minUboOffsetAlignment;
		m_referenceContext->glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &minUboOffsetAlignment);

		assert(minUboOffsetAlignment >= 1);
		m_deviceInfo.limits.minUniformBufferOffsetAlignment = static_cast<UInt64>(minUboOffsetAlignment);

		m_contexts.insert(m_referenceContext.get());
	}

	OpenGLDevice::~OpenGLDevice()
	{
		m_referenceContext.reset();
	}

	std::unique_ptr<GL::Context> OpenGLDevice::CreateContext(const GL::ContextParams& params) const
	{
		auto contextPtr = m_loader.CreateContext(this, params, m_referenceContext.get());
		m_contexts.insert(contextPtr.get());

		return contextPtr;
	}

	std::unique_ptr<GL::Context> OpenGLDevice::CreateContext(const GL::ContextParams& params, WindowHandle handle) const
	{
		auto contextPtr = m_loader.CreateContext(this, params, handle, m_referenceContext.get());
		m_contexts.insert(contextPtr.get());

		return contextPtr;
	}

	const RenderDeviceInfo& OpenGLDevice::GetDeviceInfo() const
	{
		return m_deviceInfo;
	}

	std::shared_ptr<AbstractBuffer> OpenGLDevice::InstantiateBuffer(BufferType type)
	{
		return std::make_shared<OpenGLBuffer>(*this, type);
	}

	std::shared_ptr<CommandPool> OpenGLDevice::InstantiateCommandPool(QueueType /*queueType*/)
	{
		return std::make_shared<OpenGLCommandPool>();
	}

	std::shared_ptr<Framebuffer> OpenGLDevice::InstantiateFramebuffer(unsigned int /*width*/, unsigned int /*height*/, const std::shared_ptr<RenderPass>& /*renderPass*/, const std::vector<std::shared_ptr<Texture>>& attachments)
	{
		return std::make_shared<OpenGLFboFramebuffer>(*this, attachments);
	}

	std::shared_ptr<RenderPass> OpenGLDevice::InstantiateRenderPass(std::vector<RenderPass::Attachment> attachments, std::vector<RenderPass::SubpassDescription> subpassDescriptions, std::vector<RenderPass::SubpassDependency> subpassDependencies)
	{
		return std::make_shared<OpenGLRenderPass>(std::move(attachments), std::move(subpassDescriptions), std::move(subpassDependencies));
	}

	std::shared_ptr<RenderPipeline> OpenGLDevice::InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo)
	{
		return std::make_shared<OpenGLRenderPipeline>(*this, std::move(pipelineInfo));
	}

	std::shared_ptr<RenderPipelineLayout> OpenGLDevice::InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo)
	{
		return std::make_shared<OpenGLRenderPipelineLayout>(std::move(pipelineLayoutInfo));
	}

	std::shared_ptr<ShaderModule> OpenGLDevice::InstantiateShaderModule(ShaderStageTypeFlags shaderStages, ShaderAst::StatementPtr& shaderAst, const ShaderWriter::States& states)
	{
		return std::make_shared<OpenGLShaderModule>(*this, shaderStages, shaderAst, states);
	}

	std::shared_ptr<ShaderModule> OpenGLDevice::InstantiateShaderModule(ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const ShaderWriter::States& states)
	{
		return std::make_shared<OpenGLShaderModule>(*this, shaderStages, lang, source, sourceSize, states);
	}

	std::shared_ptr<Texture> OpenGLDevice::InstantiateTexture(const TextureInfo& params)
	{
		return std::make_shared<OpenGLTexture>(*this, params);
	}

	std::shared_ptr<TextureSampler> OpenGLDevice::InstantiateTextureSampler(const TextureSamplerInfo& params)
	{
		return std::make_shared<OpenGLTextureSampler>(*this, params);
	}
	
}
