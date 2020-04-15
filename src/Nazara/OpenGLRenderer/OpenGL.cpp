// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGL.hpp>
#include <Nazara/Core/DynLib.hpp>
#include <Nazara/Core/Error.hpp>

#ifdef NAZARA_PLATFORM_WINDOWS
#include <Nazara/OpenGLRenderer/Wrapper/Win32/WGLLoader.hpp>
#endif

#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	struct OpenGLImpl
	{
		DynLib opengl32Lib;
	};

	static std::unique_ptr<OpenGLImpl> s_impl;

	bool OpenGL::Initialize()
	{
		if (s_impl)
			return true;

		auto impl = std::make_unique<OpenGLImpl>();
		if (!impl->opengl32Lib.Load("opengl32" NAZARA_DYNLIB_EXTENSION))
		{
			NazaraError("Failed to load opengl32 library, is OpenGL installed on your system?");
			return false;
		}

		std::unique_ptr<GL::Loader> loader;

#ifdef NAZARA_PLATFORM_WINDOWS
		try
		{
			loader = std::make_unique<GL::WGLLoader>(impl->opengl32Lib);
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

		s_impl = std::move(impl);
		return true;
	}

	bool OpenGL::IsInitialized()
	{
		return s_impl != nullptr;
	}

	void OpenGL::Uninitialize()
	{
		if (!s_impl)
			return;

		s_impl.reset();
	}
}

