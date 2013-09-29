// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Thread.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <stdexcept>
#include <Nazara/Renderer/Debug.hpp>

NzRenderWindow::NzRenderWindow(NzVideoMode mode, const NzString& title, nzUInt32 style, const NzContextParameters& parameters)
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

NzRenderWindow::NzRenderWindow(NzWindowHandle handle, const NzContextParameters& parameters)
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
	// Nécessaire si NzWindow::Destroy est appelé par son destructeur
	OnWindowDestroy();
}

bool NzRenderWindow::CopyToImage(NzImage* image) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_context)
	{
		NazaraError("Window has not been created");
		return false;
	}

	if (!image)
	{
		NazaraError("Image must be valid");
		return false;
	}
	#endif

	const NzContext* currentContext = NzContext::GetCurrent();
	if (m_context != currentContext)
	{
		if (!m_context->SetActive(true))
		{
			NazaraError("Failed to activate context");
			return false;
		}
	}

	NzVector2ui size = GetSize();

	if (!image->Create(nzImageType_2D, nzPixelFormat_RGBA8, size.x, size.y, 1, 1))
	{
		NazaraError("Failed to create image");
		return false;
	}

	nzUInt8* pixels = image->GetPixels();
	glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	image->FlipVertically();

	if (m_context != currentContext)
	{
		if (currentContext)
		{
			if (!currentContext->SetActive(true))
				NazaraWarning("Failed to reset current context");
		}
		else
			m_context->SetActive(false);
	}

	return true;
}

bool NzRenderWindow::CopyToTexture(NzTexture* texture) const
{
	#if NAZARA_RENDERER_SAFE
	if (!m_context)
	{
		NazaraError("Window has not been created");
		return false;
	}

	if (!texture)
	{
		NazaraError("Texture must be valid");
		return false;
	}
	#endif

	const NzContext* currentContext = NzContext::GetCurrent();
	if (m_context != currentContext)
	{
		if (!m_context->SetActive(true))
		{
			NazaraError("Failed to activate context");
			return false;
		}
	}

	NzVector2ui size = GetSize();

	if (!texture->Create(nzImageType_2D, nzPixelFormat_RGBA8, size.x, size.y, 1, 1))
	{
		NazaraError("Failed to create texture");
		return false;
	}

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, size.x, size.y);

	if (m_context != currentContext)
	{
		if (currentContext)
		{
			if (!currentContext->SetActive(true))
				NazaraWarning("Failed to reset current context");
		}
		else
			m_context->SetActive(false);
	}

	return true;
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
	if (m_framerateLimit > 0)
	{
		int remainingTime = 1000/static_cast<int>(m_framerateLimit) - static_cast<int>(m_clock.GetMilliseconds());
		if (remainingTime > 0)
			NzThread::Sleep(remainingTime);

		m_clock.Restart();
	}

	if (m_context && m_parameters.doubleBuffered)
		m_context->SwapBuffers();
}

void NzRenderWindow::EnableVerticalSync(bool enabled)
{
    if (m_context)
    {
		#if defined(NAZARA_PLATFORM_WINDOWS)
        if (!m_context->SetActive(true))
		{
			NazaraError("Failed to activate context");
			return;
		}

        if (wglSwapInterval)
            wglSwapInterval(enabled ? 1 : 0);
        else
		#elif defined(NAZARA_PLATFORM_LINUX)
        if (!m_context->SetActive(true))
        {
            NazaraError("Failed to activate context");
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

unsigned int NzRenderWindow::GetHeight() const
{
	return NzWindow::GetHeight();
}

NzRenderTargetParameters NzRenderWindow::GetParameters() const
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

unsigned int NzRenderWindow::GetWidth() const
{
	return NzWindow::GetWidth();
}

bool NzRenderWindow::IsRenderable() const
{
	return m_impl != nullptr; // Si m_impl est valide, alors m_context l'est aussi
}

bool NzRenderWindow::IsValid() const
{
	return m_impl != nullptr;
}

void NzRenderWindow::SetFramerateLimit(unsigned int limit)
{
	m_framerateLimit = limit;
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

bool NzRenderWindow::Activate() const
{
	if (m_context->SetActive(true))
	{
		glDrawBuffer((m_parameters.doubleBuffered) ? GL_BACK : GL_FRONT);
		return true;
	}
	else
	{
		NazaraError("Failed to activate window's context");
		return false;
	}
}

void NzRenderWindow::EnsureTargetUpdated() const
{
	// Rien à faire
}

bool NzRenderWindow::OnWindowCreated()
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

	if (!SetActive(true)) // Les fenêtres s'activent à la création
		NazaraWarning("Failed to activate window");

    EnableVerticalSync(false);

	NzVector2ui size = GetSize();

	// Le scissorBox/viewport (à la création) est de la taille de la fenêtre
	// https://www.opengl.org/sdk/docs/man/xhtml/glGet.xml
	NzOpenGL::SetScissorBox(NzRecti(0, 0, size.x, size.y));
	NzOpenGL::SetViewport(NzRecti(0, 0, size.x, size.y));

	NotifyParametersChange();
	NotifySizeChange();

	m_clock.Restart();

    return true;
}

void NzRenderWindow::OnWindowDestroy()
{
	if (m_context)
	{
		delete m_context;
		m_context = nullptr;
	}
}

void NzRenderWindow::OnWindowResized()
{
	NotifySizeChange();
}

