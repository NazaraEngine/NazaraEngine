// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderStage.hpp>
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

	std::shared_ptr<AbstractBuffer> OpenGLDevice::InstantiateBuffer(BufferType type)
	{
		return std::make_shared<OpenGLBuffer>(*this, type);
	}

	std::shared_ptr<CommandPool> OpenGLDevice::InstantiateCommandPool(QueueType /*queueType*/)
	{
		return std::make_shared<OpenGLCommandPool>();
	}

	std::shared_ptr<RenderPipeline> OpenGLDevice::InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo)
	{
		return std::make_shared<OpenGLRenderPipeline>(*this, std::move(pipelineInfo));
	}

	std::shared_ptr<RenderPipelineLayout> OpenGLDevice::InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo)
	{
		return std::make_shared<OpenGLRenderPipelineLayout>(std::move(pipelineLayoutInfo));
	}

	std::shared_ptr<ShaderStage> OpenGLDevice::InstantiateShaderStage(const ShaderAst& shaderAst, const ShaderWriter::States& states)
	{
		return std::make_shared<OpenGLShaderStage>(*this, shaderAst, states);
	}

	std::shared_ptr<ShaderStage> OpenGLDevice::InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		return std::make_shared<OpenGLShaderStage>(*this, type, lang, source, sourceSize);
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
