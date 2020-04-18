// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderStage.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLDevice::OpenGLDevice(GL::Loader& loader) :
	m_loader(loader)
	{
		m_referenceContext = loader.CreateContext({});
		if (!m_referenceContext)
			throw std::runtime_error("failed to create reference context");
	}

	OpenGLDevice::~OpenGLDevice() = default;

	std::unique_ptr<GL::Context> OpenGLDevice::CreateContext(const GL::ContextParams& params) const
	{
		return m_loader.CreateContext(params, m_referenceContext.get());
	}

	std::unique_ptr<GL::Context> OpenGLDevice::CreateContext(const GL::ContextParams& params, WindowHandle handle) const
	{
		return m_loader.CreateContext(params, handle, m_referenceContext.get());
	}

	std::unique_ptr<AbstractBuffer> OpenGLDevice::InstantiateBuffer(BufferType type)
	{
		return {};
	}

	std::unique_ptr<CommandPool> OpenGLDevice::InstantiateCommandPool(QueueType queueType)
	{
		return {};
	}

	std::unique_ptr<RenderPipeline> OpenGLDevice::InstantiateRenderPipeline(RenderPipelineInfo pipelineInfo)
	{
		return {};
	}

	std::shared_ptr<RenderPipelineLayout> OpenGLDevice::InstantiateRenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo)
	{
		return {};
	}

	std::shared_ptr<ShaderStageImpl> OpenGLDevice::InstantiateShaderStage(ShaderStageType type, ShaderLanguage lang, const void* source, std::size_t sourceSize)
	{
		auto shaderStage = std::make_shared<OpenGLShaderStage>();
		if (!shaderStage->Create(*this, type, lang, source, sourceSize))
			return {};

		return shaderStage;
	}

	std::unique_ptr<Texture> OpenGLDevice::InstantiateTexture(const TextureInfo& params)
	{
		return {};
	}

	std::unique_ptr<TextureSampler> OpenGLDevice::InstantiateTextureSampler(const TextureSamplerInfo& params)
	{
		return {};
	}
}
