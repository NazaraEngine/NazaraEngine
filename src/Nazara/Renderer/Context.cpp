// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <memory>
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
	thread_local const NzContext* s_currentContext = nullptr;
	thread_local const NzContext* s_threadContext = nullptr;

	void CALLBACK DebugCallback(unsigned int source, unsigned int type, unsigned int id, unsigned int severity, int length, const char* message, const void* userParam)
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
				ss << "Third party";
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
		m_parameters.shareContext = s_reference.get();

	std::unique_ptr<NzContextImpl> impl(new NzContextImpl);
	if (!impl->Create(m_parameters))
	{
		NazaraError("Failed to create context implementation");
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

	if (m_parameters.debugMode && NzOpenGL::IsSupported(nzOpenGLExtension_DebugOutput))
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
		SetActive(false);

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

	return s_currentContext == this;
}

bool NzContext::SetActive(bool active) const
{
	#ifdef NAZARA_RENDERER_SAFE
	if (!m_impl)
	{
		NazaraError("No context has been created");
		return false;
	}
	#endif

	// Si le contexte est déjà activé/désactivé
	if ((s_currentContext == this) == active)
		return true;

	if (active)
	{
		if (!m_impl->Activate())
			return false;

		s_currentContext = this;
	}
	else
	{
		if (!NzContextImpl::Desactivate())
			return false;

		s_currentContext = nullptr;
	}

	NzOpenGL::OnContextChanged(s_currentContext);

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
	if (!s_currentContext)
	{
		if (!s_threadContext)
		{
			std::unique_ptr<NzContext> context(new NzContext);
			if (!context->Create())
			{
				NazaraError("Failed to create context");
				return false;
			}

			s_threadContext = context.get();

			s_contexts.emplace_back(std::move(context));
		}

		if (!s_threadContext->SetActive(true))
		{
			NazaraError("Failed to active thread context");
			return false;
		}
	}

	return true;
}

const NzContext* NzContext::GetCurrent()
{
	return s_currentContext;
}

const NzContext* NzContext::GetReference()
{
	return s_reference.get();
}

const NzContext* NzContext::GetThreadContext()
{
	EnsureContext();

	return s_threadContext;
}

bool NzContext::Initialize()
{
	NzContextParameters parameters;
	parameters.shared = false; // Difficile de partager le contexte de référence avec lui-même

	std::unique_ptr<NzContext> reference(new NzContext);
	if (!reference->Create(parameters))
	{
		NazaraError("Failed to create reference context");
		return false;
	}

	// Le contexte de référence doit rester désactivé pour le partage
	if (!reference->SetActive(false))
	{
		NazaraError("Failed to desactive reference context");
		return false;
	}

	s_reference = std::move(reference);

	// Le contexte de référence est partagé par défaut avec les autres contextes
	NzContextParameters::defaultShareContext = s_reference.get();
	return true;
}

void NzContext::Uninitialize()
{
	s_contexts.clear(); // On supprime tous les contextes créés
	s_reference.reset();
}

std::unique_ptr<NzContext> NzContext::s_reference;
std::vector<std::unique_ptr<NzContext>> NzContext::s_contexts;
