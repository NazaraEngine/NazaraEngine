// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Loader.hpp>
#include <stdexcept>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	Context::~Context() = default;

	bool Context::Initialize(const ContextParams& params)
	{
		if (!Activate())
		{
			NazaraError("failed to activate context");
			return false;
		}

		const Loader& loader = GetLoader();

		auto LoadSymbol = [&](auto& func, const char* funcName)
		{
			func = reinterpret_cast<std::decay_t<decltype(func)>>(loader.LoadFunction(funcName));
			if (!func && !ImplementFallback(funcName) && !func) //< Not a mistake
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

		// Retrieve OpenGL version
		auto DecodeDigit = [](char c) -> int
		{
			if (c >= '0' && c <= '9')
				return c - '0';
			else
				return -1;
		};

		std::string_view versionString = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		if (versionString.size() > 2 && DecodeDigit(versionString[0]) >= 0 && versionString[1] == '.' && DecodeDigit(versionString[2]) >= 0)
		{
			m_params.glMajorVersion = DecodeDigit(versionString[0]);
			m_params.glMinorVersion = DecodeDigit(versionString[2]);
		}
		else
			NazaraWarning("Failed to decode OpenGL version: " + std::string(versionString));

		// Load extensions
		std::string_view extensionList = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		SplitString(extensionList, " ", [&](std::string_view extension)
		{
			m_supportedExtensions.emplace(extension);
			return true;
		});

		// If we requested an OpenGL ES context but cannot create one, check for some compatibility extensions
		if (params.type == ContextType::OpenGL_ES && m_params.type != params.type)
		{
			if (m_supportedExtensions.count("GL_ARB_ES3_2_compatibility"))
			{
				m_params.type = ContextType::OpenGL_ES;
				m_params.glMajorVersion = 3;
				m_params.glMinorVersion = 2;
			}
			else if (m_supportedExtensions.count("GL_ARB_ES3_1_compatibility"))
			{
				m_params.type = ContextType::OpenGL_ES;
				m_params.glMajorVersion = 3;
				m_params.glMinorVersion = 1;
			}
			else if (m_supportedExtensions.count("GL_ARB_ES3_compatibility"))
			{
				m_params.type = ContextType::OpenGL_ES;
				m_params.glMajorVersion = 3;
				m_params.glMinorVersion = 0;
			}
			else
				NazaraWarning("desktop support for OpenGL ES is missing, falling back to OpenGL...");
		}

		return true;
	}
}
