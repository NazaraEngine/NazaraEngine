// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/ContextParameters.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	NzContextParameters invalidContextParameters;
	NzRenderTargetParameters invalidRTParameters;
}

NzRenderWindow::NzRenderWindow() :
m_context(nullptr)
{
}

NzRenderWindow::NzRenderWindow(NzVideoMode mode, const NzString& title, nzUInt32 style, const NzContextParameters& parameters) :
m_context(nullptr)
{
	Create(mode, title, style, parameters);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create render window");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzRenderWindow::NzRenderWindow(NzWindowHandle handle, const NzContextParameters& parameters) :
m_context(nullptr)
{
	Create(handle, parameters);

	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Failed to create render window");
		throw std::runtime_error("Constructor failed");
	}
	#endif
}

NzRenderWindow::~NzRenderWindow()
{
}

bool NzRenderWindow::CanActivate() const
{
	return m_impl != nullptr && m_context != nullptr;
}

bool NzRenderWindow::Create(NzVideoMode mode, const NzString& title, nzUInt32 style, const NzContextParameters& parameters)
{
    m_parameters = parameters;
    return NzWindow::Create(mode, title, style);
}

bool NzRenderWindow::Create(NzWindowHandle handle, const NzContextParameters& parameters)
{
    m_parameters = parameters;
    return NzWindow::Create(handle);
}

void NzRenderWindow::Display()
{
	if (m_context)
		m_context->SwapBuffers();
}

void NzRenderWindow::EnableVerticalSync(bool enabled)
{
    if (m_context)
    {
#if defined(NAZARA_PLATFORM_WINDOWS)
        if (!m_context->SetActive(true))
        {
            NazaraError("Unable to activate context");
            return;
        }

        if (wglSwapInterval)
            wglSwapInterval(enabled ? 1 : 0);
        else
#elif defined(NAZARA_PLATFORM_LINUX)
        if (!m_context->SetActive(true))
        {
            NazaraError("Unable to activate context");
            return;
        }

        if (glXSwapInterval)
            glXSwapInterval(enabled ? 1 : 0);
        else
#else
	#error Vertical Sync is not supported on this platform
#endif
            NazaraError("Vertical Sync is not supported on this platform");
    }
    else
        NazaraError("No context");
}

NzRenderTargetParameters NzRenderWindow::GetRenderTargetParameters() const
{
	if (m_context)
	{
		const NzContextParameters& parameters = m_context->GetParameters();
		return NzRenderTargetParameters(parameters.antialiasingLevel, parameters.depthBits, parameters.stencilBits);
	}
	else
	{
		NazaraError("Window not created/context not initialized");
		return NzRenderTargetParameters();
	}
}

NzContextParameters NzRenderWindow::GetContextParameters() const
{
	if (m_context)
		return m_context->GetParameters();
	else
	{
		NazaraError("Window not created/context not initialized");
		return NzContextParameters();
	}
}

bool NzRenderWindow::HasContext() const
{
	return true;
}

bool NzRenderWindow::Activate()
{
	return m_context->SetActive(true);
}

void NzRenderWindow::OnClose()
{
	delete m_context;
}

bool NzRenderWindow::OnCreate()
{
	m_parameters.doubleBuffered = true;
    m_parameters.window = GetHandle();

    m_context = new NzContext;
    if (!m_context->Create(m_parameters))
    {
        NazaraError("Failed not create context");
        delete m_context;

        return false;
    }

    EnableVerticalSync(false);

	#if NAZARA_RENDERER_ACTIVATE_RENDERWINDOW_ON_CREATION
	if (!SetActive(true)) // Les fenêtres s'activent à la création
		NazaraWarning("Failed to activate window");
	#endif

    return true;
}
