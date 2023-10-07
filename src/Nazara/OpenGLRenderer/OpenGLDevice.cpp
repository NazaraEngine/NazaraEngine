// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/OpenGLBuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandBufferBuilder.hpp>
#include <Nazara/OpenGLRenderer/OpenGLCommandPool.hpp>
#include <Nazara/OpenGLRenderer/OpenGLComputePipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPass.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipeline.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderPipelineLayout.hpp>
#include <Nazara/OpenGLRenderer/OpenGLShaderModule.hpp>
#include <Nazara/OpenGLRenderer/OpenGLSwapchain.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTexture.hpp>
#include <Nazara/OpenGLRenderer/OpenGLTextureSampler.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/Renderer/CommandPool.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <array>
#include <stdexcept>

namespace Nz
{
	OpenGLDevice::OpenGLDevice(GL::Loader& loader, const Renderer::Config& config) :
	m_loader(loader)
	{
		GL::ContextParams params;
		params.type = loader.GetPreferredContextType();
		params.validationLevel = config.validationLevel;

#ifdef NAZARA_OPENGLRENDERER_DEBUG
		params.wrapErrorHandling = true;
#endif

		m_referenceContext = loader.CreateContext(this, params);
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

		// Features
		if (m_referenceContext->IsExtensionSupported(GL::Extension::TextureFilterAnisotropic))
			m_deviceInfo.features.anisotropicFiltering = true;

		if (m_referenceContext->IsExtensionSupported(GL::Extension::ComputeShader))
			m_deviceInfo.features.computeShaders = true;

		if (m_referenceContext->IsExtensionSupported(GL::Extension::DepthClamp))
			m_deviceInfo.features.depthClamping = true;

		if (m_referenceContext->glPolygonMode) //< not supported in core OpenGL ES, but supported in OpenGL or with GL_NV_polygon_mode extension
			m_deviceInfo.features.nonSolidFaceFilling = true;

		if (m_referenceContext->IsExtensionSupported(GL::Extension::StorageBuffers))
			m_deviceInfo.features.storageBuffers = true;

		if (m_referenceContext->IsExtensionSupported(GL::Extension::ShaderImageLoadStore))
		{
			m_deviceInfo.features.textureReadWrite = true;
			m_deviceInfo.features.textureWriteWithoutFormat = true;
		}

		if (m_referenceContext->IsExtensionSupported(GL::Extension::ShaderImageLoadFormatted))
			m_deviceInfo.features.textureReadWithoutFormat = true;

		if (m_referenceContext->IsExtensionSupported(GL::Extension::TextureView))
			m_deviceInfo.features.unrestrictedTextureViews = true;

		// Limits
		m_deviceInfo.limits.maxUniformBufferSize = m_referenceContext->GetInteger<UInt64>(GL_MAX_UNIFORM_BLOCK_SIZE);
		m_deviceInfo.limits.minUniformBufferOffsetAlignment = RoundToPow2(m_referenceContext->GetInteger<UInt64>(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT));

		if (m_deviceInfo.features.storageBuffers)
		{
			m_deviceInfo.limits.maxStorageBufferSize = m_referenceContext->GetInteger<UInt64>(GL_MAX_SHADER_STORAGE_BLOCK_SIZE);
			m_deviceInfo.limits.minStorageBufferOffsetAlignment = RoundToPow2(m_referenceContext->GetInteger<UInt64>(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT));
		}

		if (m_deviceInfo.features.computeShaders)
		{
			m_deviceInfo.limits.maxComputeSharedMemorySize = m_referenceContext->GetInteger<UInt64>(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE);
			m_deviceInfo.limits.maxComputeWorkGroupInvocations = m_referenceContext->GetInteger<UInt32>(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS);

			m_deviceInfo.limits.maxComputeWorkGroupCount = {
				m_referenceContext->GetInteger<UInt32>(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0),
				m_referenceContext->GetInteger<UInt32>(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1),
				m_referenceContext->GetInteger<UInt32>(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2)
			};

			m_deviceInfo.limits.maxComputeWorkGroupSize = {
				m_referenceContext->GetInteger<UInt32>(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0),
				m_referenceContext->GetInteger<UInt32>(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1),
				m_referenceContext->GetInteger<UInt32>(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2)
			};
		}

		m_contexts.insert(m_referenceContext.get());
	}

	OpenGLDevice::~OpenGLDevice()
	{
		// Free reference context first as it will unregister itself from m_contexts
		m_referenceContext.reset();
	}

	std::shared_ptr<GL::Context> OpenGLDevice::CreateContext([[maybe_unused]] GL::ContextParams params) const
	{
#ifdef NAZARA_PLATFORM_WEB
		// WebGL doesn't support context sharing and we support only one canvas, return the reference context
		return m_referenceContext;
#else
		params.type = m_referenceContext->GetParams().type;

		auto contextPtr = m_loader.CreateContext(this, params, m_referenceContext.get());
		m_contexts.insert(contextPtr.get());

		return contextPtr;
#endif
	}

	std::shared_ptr<GL::Context> OpenGLDevice::CreateContext([[maybe_unused]] GL::ContextParams params, [[maybe_unused]] WindowHandle handle) const
	{
#ifdef NAZARA_PLATFORM_WEB
		// WebGL doesn't support context sharing and we support only one canvas, return the reference context
		return m_referenceContext;
#else
		params.type = m_referenceContext->GetParams().type;

		auto contextPtr = m_loader.CreateContext(this, params, handle, m_referenceContext.get());
		m_contexts.insert(contextPtr.get());

		return contextPtr;
#endif
	}

	const RenderDeviceInfo& OpenGLDevice::GetDeviceInfo() const
	{
		return m_deviceInfo;
	}

	const RenderDeviceFeatures& OpenGLDevice::GetEnabledFeatures() const
	{
		//FIXME
		return m_deviceInfo.features;
	}

	std::shared_ptr<RenderBuffer> OpenGLDevice::InstantiateBuffer(BufferType type, UInt64 size, BufferUsageFlags usageFlags, const void* initialData)
	{
		return std::make_shared<OpenGLBuffer>(*this, type, size, usageFlags, initialData);
	}

	std::shared_ptr<CommandPool> OpenGLDevice::InstantiateCommandPool(QueueType /*queueType*/)
	{
		return std::make_shared<OpenGLCommandPool>();
	}

	std::shared_ptr<ComputePipeline> OpenGLDevice::InstantiateComputePipeline(ComputePipelineInfo pipelineInfo)
	{
		return std::make_shared<OpenGLComputePipeline>(*this, std::move(pipelineInfo));
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

	std::shared_ptr<ShaderModule> OpenGLDevice::InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, const nzsl::Ast::Module& shaderModule, const nzsl::ShaderWriter::States& states)
	{
		return std::make_shared<OpenGLShaderModule>(*this, shaderStages, shaderModule, states);
	}

	std::shared_ptr<ShaderModule> OpenGLDevice::InstantiateShaderModule(nzsl::ShaderStageTypeFlags shaderStages, ShaderLanguage lang, const void* source, std::size_t sourceSize, const nzsl::ShaderWriter::States& states)
	{
		return std::make_shared<OpenGLShaderModule>(*this, shaderStages, lang, source, sourceSize, states);
	}

	std::shared_ptr<Swapchain> OpenGLDevice::InstantiateSwapchain(WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters)
	{
		return std::make_shared<OpenGLSwapchain>(*this, windowHandle, windowSize, parameters);
	}

	std::shared_ptr<Texture> OpenGLDevice::InstantiateTexture(const TextureInfo& params)
	{
		return std::make_shared<OpenGLTexture>(*this, params);
	}

	std::shared_ptr<Texture> OpenGLDevice::InstantiateTexture(const TextureInfo& params, const void* initialData, bool buildMipmaps, unsigned int srcWidth, unsigned int srcHeight)
	{
		return std::make_shared<OpenGLTexture>(*this, params, initialData, buildMipmaps, srcWidth, srcHeight);
	}

	std::shared_ptr<TextureSampler> OpenGLDevice::InstantiateTextureSampler(const TextureSamplerInfo& params)
	{
		return std::make_shared<OpenGLTextureSampler>(*this, params);
	}

	bool OpenGLDevice::IsTextureFormatSupported(PixelFormat format, TextureUsage usage) const
	{
		switch (format)
		{
			case PixelFormat::Undefined:
				return false;

			// the following formats are emulated using texture swizzling which isn't supported on WebGL
			// https://registry.khronos.org/webgl/specs/latest/2.0/#5.19
			case PixelFormat::A8Unorm:
			case PixelFormat::BGR8Unorm:
			case PixelFormat::BGR8_sRGB:
			case PixelFormat::BGRA8Unorm:
			case PixelFormat::BGRA8_sRGB:
			case PixelFormat::L8Unorm:
			case PixelFormat::LA8Unorm:
#ifdef NAZARA_PLATFORM_WEB
				return false;
#else
				[[fallthrough]];
#endif

			case PixelFormat::R8Unorm:
			case PixelFormat::R8Sint:
			case PixelFormat::R8Uint:
			case PixelFormat::R16Unorm:
			case PixelFormat::R16Float:
			case PixelFormat::R16Sint:
			case PixelFormat::R16Uint:
			case PixelFormat::R32Float:
			case PixelFormat::R32Sint:
			case PixelFormat::R32Uint:
			case PixelFormat::RG8Unorm:
			case PixelFormat::RG8Sint:
			case PixelFormat::RG8Uint:
			case PixelFormat::RG16Unorm:
			case PixelFormat::RG16Float:
			case PixelFormat::RG16Sint:
			case PixelFormat::RG16Uint:
			case PixelFormat::RG32Float:
			case PixelFormat::RG32Sint:
			case PixelFormat::RG32Uint:
			case PixelFormat::RGB5A1Unorm:
			case PixelFormat::RGB8Unorm:
			case PixelFormat::RGB8_sRGB:
			case PixelFormat::RGB16Float:
			case PixelFormat::RGB16Sint:
			case PixelFormat::RGB16Uint:
			case PixelFormat::RGB32Float:
			case PixelFormat::RGB32Sint:
			case PixelFormat::RGB32Uint:
			case PixelFormat::RGBA4:
			case PixelFormat::RGBA8Unorm:
			case PixelFormat::RGBA8_sRGB:
			case PixelFormat::RGBA16Float:
			case PixelFormat::RGBA16Sint:
			case PixelFormat::RGBA16Uint:
			case PixelFormat::RGBA32Float:
			case PixelFormat::RGBA32Sint:
			case PixelFormat::RGBA32Uint:
				return usage == TextureUsage::ColorAttachment || usage == TextureUsage::InputAttachment || usage == TextureUsage::ShaderSampling || usage == TextureUsage::ShaderReadWrite || usage == TextureUsage::TransferDestination || usage == TextureUsage::TransferSource;

			case PixelFormat::BC1:
			case PixelFormat::BC2:
			case PixelFormat::BC3:
			{
				if (!m_referenceContext->IsExtensionSupported(GL::Extension::TextureCompressionS3tc))
					return false;

				return usage == TextureUsage::InputAttachment || usage == TextureUsage::ShaderSampling || usage == TextureUsage::TransferDestination || usage == TextureUsage::TransferSource;
			}

			case PixelFormat::Depth16:
			case PixelFormat::Depth16Stencil8:
			case PixelFormat::Depth24:
			case PixelFormat::Depth24Stencil8:
			case PixelFormat::Depth32F:
			case PixelFormat::Depth32FStencil8:
			case PixelFormat::Stencil1:
			case PixelFormat::Stencil4:
			case PixelFormat::Stencil8:
			case PixelFormat::Stencil16:
				return usage == TextureUsage::DepthStencilAttachment || usage == TextureUsage::ShaderSampling || usage == TextureUsage::TransferDestination || usage == TextureUsage::TransferSource;
		}

		return false;
	}

	void OpenGLDevice::WaitForIdle()
	{
		const GL::Context* activeContext = GL::Context::GetCurrentContext();
		if (!activeContext || activeContext->GetDevice() != this)
		{
			if (!GL::Context::SetCurrentContext(m_referenceContext.get()))
				throw std::runtime_error("failed to activate context");
		}

		m_referenceContext->glFinish();
	}
}
