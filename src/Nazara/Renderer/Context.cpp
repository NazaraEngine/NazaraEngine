// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <vector>

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
	thread_local NzContext* currentContext = nullptr;
	thread_local NzContext* threadContext = nullptr;

	std::vector<NzContext*> contexts;

	void CALLBACK DebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, void* userParam)
	{
		NazaraUnused(length);

		NzStringStream ss;
		ss << "OpenGL debug message (ID: 0x" << NzString::Number(id, 16) << "):\n";
		ss << "Sent by context: " << userParam;
		ss << "\n-Source: ";
		switch (source)
		{
			case GL_DEBUG_SOURCE_API:
				ss << "OpenGL";
				break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
				ss << "Operating system";
				break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER:
				ss << "Shader compiler";
				break;

			case GL_DEBUG_SOURCE_THIRD_PARTY:
				ss << "Shader compiler";
				break;

			case GL_DEBUG_SOURCE_APPLICATION:
				ss << "Application";
				break;

			case GL_DEBUG_SOURCE_OTHER:
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
			case GL_DEBUG_TYPE_ERROR:
				ss << "Error";
				break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
				ss << "Deprecated behavior";
				break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
				ss << "Undefined behavior";
				break;

			case GL_DEBUG_TYPE_PORTABILITY:
				ss << "Portability";
				break;

			case GL_DEBUG_TYPE_PERFORMANCE:
				ss << "Performance";
				break;

			case GL_DEBUG_TYPE_OTHER:
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
			case GL_DEBUG_SEVERITY_HIGH:
				ss << "High";
				break;

			case GL_DEBUG_SEVERITY_MEDIUM:
				ss << "Medium";
				break;

			case GL_DEBUG_SEVERITY_LOW:
				ss << "Low";
				break;

			default:
				// Peut être rajouté par une extension
				ss << "Unknown";
				break;
		}
		ss << '\n';

		ss << "Message: " << message << '\n';

		NazaraNotice(ss);
	}
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

	if (!SetActive(true))
	{
		NazaraError("Failed to activate context");

		m_impl->Destroy();
		delete m_impl;
		m_impl = nullptr;

		return false;
	}

	if (m_parameters.antialiasingLevel > 0)
		glEnable(GL_MULTISAMPLE);

	if (NzOpenGL::IsSupported(nzOpenGLExtension_DebugOutput) && m_parameters.debugMode)
	{
		glDebugMessageCallback(&DebugCallback, this);

		#ifdef NAZARA_DEBUG
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		#endif
	}

	NotifyCreated();

	return true;
}

void NzContext::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();
		NzOpenGL::OnContextDestruction(this);

		if (currentContext == this)
		{
			NzContextImpl::Desactivate();
			currentContext = nullptr;
		}

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

	NzOpenGL::OnContextChange(currentContext);

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

bool NzContext::EnsureContext()
{
	if (!currentContext)
	{
		if (!threadContext)
		{
			NzContext* context = new NzContext;
			if (!context->Create())
			{
				NazaraError("Failed to create context");
				delete context;

				return false;
			}

			contexts.push_back(context);

			threadContext = context;
		}

		if (!threadContext->SetActive(true))
		{
			NazaraError("Failed to active thread context");
			return false;
		}
	}

	return true;
}

NzContext* NzContext::GetCurrent()
{
	return currentContext;
}

NzContext* NzContext::GetReference()
{
	return s_reference;
}

NzContext* NzContext::GetThreadContext()
{
	EnsureContext();

	return threadContext;
}

bool NzContext::Initialize()
{
	NzContextParameters parameters;
	parameters.shared = false; // Difficile de partager le contexte de référence avec lui-même

	s_reference = new NzContext;
	if (!s_reference->Create(parameters))
	{
		delete s_reference;
		s_reference = nullptr;

		return false;
	}

	// Le contexte de référence doit rester désactivé pour le partage
	s_reference->SetActive(false);

	NzContextParameters::defaultShareContext = s_reference;

	return true;
}

void NzContext::Uninitialize()
{
	for (NzContext* context : contexts)
		delete context;

	contexts.clear(); // On supprime tous les contextes créés

	delete s_reference;
	s_reference = nullptr;
}

NzContext* NzContext::s_reference = nullptr;
