// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Renderer/Config.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Renderer/Win32/ContextImpl.hpp>
#elif defined(NAZARA_PLATFORM_LINUX)
	#include <Nazara/Renderer/Linux/ContextImpl.hpp>
#else
	#error Lack of implementation: Context
#endif

#include <Nazara/Renderer/Debug.hpp>

namespace
{
	///TODO: Thread-local
	NzContext* currentContext = nullptr;
	NzContext* threadContext = nullptr;

	void CALLBACK DebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam)
	{
		NazaraUnused(length);

		NzStringStream ss;
		ss << "OpenGL debug message (ID: 0x" << NzString::Number(id, 16) << "):\n";
		ss << "-Source: ";
		switch (source)
		{
			case GL_DEBUG_SOURCE_API_ARB:
				ss << "OpenGL";
				break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
				ss << "Operating system";
				break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
				ss << "Shader compiler";
				break;

			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
				ss << "Shader compiler";
				break;

			case GL_DEBUG_SOURCE_APPLICATION_ARB:
				ss << "Application";
				break;

			case GL_DEBUG_SOURCE_OTHER_ARB:
				ss << "Other";
				break;

			default:
				// Peut être rajouté par une extension
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "-Type: ";
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR_ARB:
				ss << "Error";
				break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
				ss << "Deprecated behavior";
				break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
				ss << "Undefined behavior";
				break;

			case GL_DEBUG_TYPE_PORTABILITY_ARB:
				ss << "Portability";
				break;

			case GL_DEBUG_TYPE_PERFORMANCE_ARB:
				ss << "Performance";
				break;

			case GL_DEBUG_TYPE_OTHER_ARB:
				ss << "Other";
				break;

			default:
				// Peut être rajouté par une extension
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "-Severity: ";
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH_ARB:
				ss << "High";
				break;

			case GL_DEBUG_SEVERITY_MEDIUM_ARB:
				ss << "Medium";
				break;

			case GL_DEBUG_SEVERITY_LOW_ARB:
				ss << "Low";
				break;

			default:
				// Peut être rajouté par une extension
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "Message: " << message;
		ss << "\n\nSent by context: " << userParam;

		NazaraNotice(ss);
	}
}

NzContext::NzContext() :
m_impl(nullptr)
{
}

NzContext::~NzContext()
{
	Destroy();
}

bool NzContext::Create(const NzContextParameters& parameters)
{
	Destroy();

	m_parameters = parameters;
	if (m_parameters.shared && !m_parameters.shareContext)
		m_parameters.shareContext = s_reference;

	m_impl = new NzContextImpl;
	if (!m_impl->Create(m_parameters))
	{
		NazaraError("Failed to create context implementation");
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	if (!m_impl->Activate())
	{
		NazaraError("Failed to activate context");

		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	if (m_parameters.antialiasingLevel > 0)
		glEnable(GL_MULTISAMPLE);

	if (NzOpenGL::IsSupported(NzOpenGL::DebugOutput) && m_parameters.debugMode)
	{
		glDebugMessageCallback(&DebugCallback, this);

		#ifdef NAZARA_DEBUG
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		#endif
	}

	return true;
}

void NzContext::Destroy()
{
	if (m_impl)
	{
		if (currentContext == this)
			NzContextImpl::Desactivate();

		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;
	}
}

const NzContextParameters& NzContext::GetParameters() const
{
	#ifdef NAZARA_RENDERER_SAFE
	if (!m_impl)
		NazaraError("No context has been created");
	#endif

	return m_parameters;
}

bool NzContext::IsActive() const
{
	#ifdef NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("No context has been created");
		return false;
	}
	#endif

	return currentContext == this;
}

bool NzContext::SetActive(bool active)
{
	#ifdef NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("No context has been created");
		return false;
	}
	#endif

	// Si le contexte est déjà activé/désactivé
	if ((currentContext == this) == active)
		return true;

	if (active)
	{
		if (!m_impl->Activate())
			return false;

		currentContext = this;
	}
	else
	{
		if (!NzContextImpl::Desactivate())
			return false;

		currentContext = nullptr;
	}

	return true;
}

void NzContext::SwapBuffers()
{
	#ifdef NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("No context has been created");
		return;
	}

	if (!m_parameters.doubleBuffered)
	{
		NazaraError("Context is not double buffered");
		return;
	}
	#endif

	m_impl->SwapBuffers();
}

const NzContext* NzContext::GetCurrent()
{
	return currentContext;
}

const NzContext* NzContext::GetReference()
{
	return s_reference;
}

const NzContext* NzContext::GetThreadContext()
{
	return threadContext;
}

bool NzContext::InitializeReference()
{
	NzContextParameters parameters;
//	parameters.compatibilityProfile = true;
	parameters.shared = false; // Difficile de partager le contexte de référence avec lui-même

	s_reference = new NzContext;
	if (!s_reference->Create(parameters))
	{
		delete s_reference;
		s_reference = nullptr;

		return false;
	}

	return true;
}

void NzContext::UninitializeReference()
{
	delete s_reference;
	s_reference = nullptr;
}

NzContext* NzContext::s_reference = nullptr;
