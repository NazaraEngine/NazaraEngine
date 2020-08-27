// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/OpenGLRenderer/DummySurface.hpp>
#include <Nazara/OpenGLRenderer/OpenGLRenderWindow.hpp>
#include <cassert>
#include <sstream>

#ifdef NAZARA_PLATFORM_WINDOWS
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLLoader.hpp>
#endif

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderer::~OpenGLRenderer()
	{
		m_device.reset();
		m_loader.reset();
	}

	std::unique_ptr<RenderSurface> OpenGLRenderer::CreateRenderSurfaceImpl()
	{
		return std::make_unique<DummySurface>();
	}

	std::unique_ptr<RenderWindowImpl> OpenGLRenderer::CreateRenderWindowImpl(RenderWindow& owner)
	{
		return std::make_unique<OpenGLRenderWindow>(owner);
	}

	std::shared_ptr<RenderDevice> OpenGLRenderer::InstanciateRenderDevice(std::size_t deviceIndex)
	{
		assert(deviceIndex == 0);
		return m_device;
	}

	bool OpenGLRenderer::Prepare(const ParameterList& parameters)
	{
		if (!m_opengl32Lib.Load("opengl32" NAZARA_DYNLIB_EXTENSION))
		{
			NazaraError("Failed to load opengl32 library, is OpenGL installed on your system?");
			return false;
		}

		std::unique_ptr<GL::Loader> loader;

#ifdef NAZARA_PLATFORM_WINDOWS
		try
		{
			loader = std::make_unique<GL::WGLLoader>(m_opengl32Lib);
		}
		catch (const std::exception& e)
		{
			NazaraWarning(std::string("Failed to load WGL: ") + e.what());
		}
#endif

		if (!loader)
		{
			NazaraError("Failed to initialize OpenGL loader");
			return false;
		}

		m_loader = std::move(loader);

		m_device = std::make_shared<OpenGLDevice>(*m_loader);

		return true;
	}

	RenderAPI OpenGLRenderer::QueryAPI() const
	{
		return RenderAPI::OpenGL;
	}

	std::string OpenGLRenderer::QueryAPIString() const
	{
		std::ostringstream ss;
		ss << "OpenGL ES renderer 3.0";

		return ss.str();
	}

	UInt32 OpenGLRenderer::QueryAPIVersion() const
	{
		return 300;
	}

	std::vector<RenderDeviceInfo> OpenGLRenderer::QueryRenderDevices() const
	{
		std::vector<RenderDeviceInfo> devices;
		auto& dummyDevice = devices.emplace_back();
		dummyDevice.name = "OpenGL Default Device";
		dummyDevice.type = RenderDeviceType::Unknown;

		return devices;
	}
}
