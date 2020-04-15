// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLRenderer.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/RenderSurface.hpp>
#include <Nazara/Renderer/RenderWindowImpl.hpp>
#include <Nazara/OpenGLRenderer/OpenGL.hpp>
#include <cassert>
#include <sstream>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	OpenGLRenderer::~OpenGLRenderer()
	{
		OpenGL::Uninitialize();
	}

	std::unique_ptr<RenderSurface> OpenGLRenderer::CreateRenderSurfaceImpl()
	{
		return {};
	}

	std::unique_ptr<RenderWindowImpl> OpenGLRenderer::CreateRenderWindowImpl()
	{
		return {};
	}

	std::shared_ptr<RenderDevice> OpenGLRenderer::InstanciateRenderDevice(std::size_t deviceIndex)
	{
		//assert(deviceIndex < m_physDevices.size());
		//return OpenGL::SelectDevice(m_physDevices[deviceIndex]);
		return {};
	}

	bool OpenGLRenderer::IsBetterThan(const RendererImpl* other) const
	{
		if (other->QueryAPI() == RenderAPI::OpenGL && QueryAPIVersion() > other->QueryAPIVersion())
			return true;

		return false; //< OpenGL is mostly a fallback to other renderers
	}

	bool OpenGLRenderer::Prepare(const ParameterList& parameters)
	{
		return OpenGL::Initialize();
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
