// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/OpenGLSwapchain.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/Swapchain.hpp>
#include <cassert>
#include <sstream>

#if defined(NAZARA_PLATFORM_WINDOWS) || defined(NAZARA_PLATFORM_LINUX)
#include <Nazara/OpenGLRenderer/Wrapper/EGL/EGLLoader.hpp>
#endif

#if defined(NAZARA_PLATFORM_WEB)
#include <Nazara/OpenGLRenderer/Wrapper/Web/WebLoader.hpp>
#endif

#ifdef NAZARA_PLATFORM_WINDOWS
#include <Nazara/OpenGLRenderer/Wrapper/WGL/WGLLoader.hpp>
#endif

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderer::~OpenGLRenderer()
	{
		// Free device before loader
		m_device.reset();
	}

	std::shared_ptr<RenderDevice> OpenGLRenderer::InstanciateRenderDevice([[maybe_unused]] std::size_t deviceIndex, const RenderDeviceFeatures& enabledFeatures)
	{
		assert(deviceIndex == 0);

		// For now, since we have to create a device to know its features, supported features are always reported as enabled
		// We still call ValidateFeatures in order to trigger warnings if requested features are not supported
		// TODO: Report disabled features as disabled (make OpenGLDeviceProxy?)
		RenderDeviceFeatures validatedFeatures = enabledFeatures;
		OpenGLDevice::ValidateFeatures(m_device->GetEnabledFeatures(), validatedFeatures);

		return m_device;
	}

	bool OpenGLRenderer::Prepare(const Renderer::Config& config)
	{
		std::unique_ptr<GL::Loader> loader = SelectLoader(config);
		if (!loader)
		{
			NazaraError("failed to initialize OpenGL loader");
			return false;
		}

		m_loader = std::move(loader);

		m_device = std::make_shared<OpenGLDevice>(*m_loader, config);
		m_deviceInfos.emplace_back(m_device->GetDeviceInfo());

		return true;
	}

	std::unique_ptr<GL::Loader> OpenGLRenderer::SelectLoader(const Renderer::Config& config)
	{
#ifdef NAZARA_PLATFORM_WINDOWS
		try
		{
			return std::make_unique<GL::WGLLoader>(config);
		}
		catch (const std::exception& e)
		{
			NazaraWarningFmt("failed to load WGL: {0}", e.what());
		}
#endif

#if defined(NAZARA_PLATFORM_WINDOWS) || defined(NAZARA_PLATFORM_LINUX)
		try
		{
			return std::make_unique<GL::EGLLoader>(config);
		}
		catch (const std::exception& e)
		{
			NazaraWarningFmt("failed to load EGL: {0}", e.what());
		}
#endif

#if defined(NAZARA_PLATFORM_WEB)
		try
		{
			return std::make_unique<GL::WebLoader>();
		}
		catch (const std::exception& e)
		{
			NazaraWarningFmt("failed to load WebGL: {0}", e.what());
		}
#endif

		return {};
	}

	RenderAPI OpenGLRenderer::QueryAPI() const
	{
		return (m_device->GetReferenceContext().GetParams().type == GL::ContextType::OpenGL) ? RenderAPI::OpenGL : RenderAPI::OpenGL_ES;
	}

	std::string OpenGLRenderer::QueryAPIString() const
	{
		const auto& params = m_device->GetReferenceContext().GetParams();

		std::ostringstream ss;
		ss << "OpenGL";
		if (params.type == GL::ContextType::OpenGL_ES)
			ss << " ES";

		ss << " renderer " << params.glMajorVersion << "." << params.glMinorVersion;

		return ss.str();
	}

	UInt32 OpenGLRenderer::QueryAPIVersion() const
	{
		return 300;
	}

	const std::vector<RenderDeviceInfo>& OpenGLRenderer::QueryRenderDevices() const
	{
		return m_deviceInfos;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
