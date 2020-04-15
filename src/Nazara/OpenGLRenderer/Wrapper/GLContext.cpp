// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/GLContext.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	GLContext::~GLContext() = default;

	bool GLContext::LoadCoreFunctions(Loader& loader)
	{
		if (!Activate())
		{
			NazaraError("failed to activate context");
			return false;
		}

		auto LoadSymbol = [&](auto& func, const char* funcName)
		{
			func = reinterpret_cast<std::decay_t<decltype(func)>>(loader.LoadFunction(funcName));
			if (!func)
				throw std::runtime_error("failed to load core function " + std::string(funcName));
		};

		try
		{
#define NAZARA_OPENGLRENDERER_FUNC(name, sig) LoadSymbol(name, #name);
			NAZARA_OPENGLRENDERER_FOREACH_GLES_FUNC(NAZARA_OPENGLRENDERER_FUNC)
#undef NAZARA_OPENGLRENDERER_FUNC
		}
		catch (const std::exception& e)
		{
			NazaraError(e.what());
			return false;
		}

		return true;
	}
}
