// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Code inspiré de NeHe (Lesson1) et de la SFML par Laurent Gomila

#include <cstring>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Debug.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/SDL2/ContextImpl.hpp>
namespace Nz
{
	ContextImpl::ContextImpl()
	{
	}

	bool ContextImpl::Activate() const
	{
		bool success = SDL_GL_MakeCurrent(m_window, m_context) == 0;

		if (!success)
			NazaraError(SDL_GetError());
		else
			lastActive = m_window;

		return success;
	}

	bool ContextImpl::Create(ContextParameters& parameters)
	{
		if (parameters.window)
		{
			m_window = static_cast<SDL_Window*>(parameters.window);
			m_ownsWindow = false;
		}
		else
		{
			m_window = SDL_CreateWindow("STATIC", 0, 0, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
			if (!m_window)
			{
				NazaraError("Failed to create window");
				return false;
			}

			//SDL_HideWindow(m_window);
			m_ownsWindow = true;
		}

		// En cas d'exception, la ressource sera quand même libérée
		CallOnExit onExit([this] ()
		{
			Destroy();
		});


		bool valid = true;

		std::array<std::pair<SDL_GLattr, int>, 13> attributes{
			std::pair<SDL_GLattr, int>
			{SDL_GL_CONTEXT_PROFILE_MASK,       parameters.compatibilityProfile ? SDL_GL_CONTEXT_PROFILE_COMPATIBILITY : SDL_GL_CONTEXT_PROFILE_CORE},
			{SDL_GL_CONTEXT_MAJOR_VERSION,      parameters.majorVersion},
			{SDL_GL_CONTEXT_MINOR_VERSION,      parameters.minorVersion},
			{SDL_GL_CONTEXT_FLAGS,              parameters.debugMode ? SDL_GL_CONTEXT_DEBUG_FLAG : 0},
			{SDL_GL_SHARE_WITH_CURRENT_CONTEXT, true},
			{SDL_GL_RED_SIZE,           (parameters.bitsPerPixel == 32) ? 8 : parameters.bitsPerPixel / 3}, // sad but I don't have a solution for now
			{SDL_GL_GREEN_SIZE,         (parameters.bitsPerPixel == 32) ? 8 : parameters.bitsPerPixel / 3},
			{SDL_GL_BLUE_SIZE,          (parameters.bitsPerPixel == 32) ? 8 : parameters.bitsPerPixel / 3},
			{SDL_GL_ALPHA_SIZE,         (parameters.bitsPerPixel == 32) ? 8 : 0},
			{SDL_GL_DEPTH_SIZE,         parameters.depthBits},
			{SDL_GL_STENCIL_SIZE,       parameters.stencilBits},
			//{SDL_GL_DOUBLEBUFFER,	    parameters.doubleBuffered}, // doesn't work if we dont close all windows
			{SDL_GL_MULTISAMPLEBUFFERS, parameters.antialiasingLevel > 0 ? GL_TRUE : GL_FALSE},
			{SDL_GL_MULTISAMPLESAMPLES, parameters.antialiasingLevel}
		};

		for (const auto& attribute : attributes) {
			valid &= SDL_GL_SetAttribute(attribute.first, attribute.second) == 0;

			if (!valid) {
				NazaraWarning(SDL_GetError());
				break;
			}
		}

		if (!valid)
			NazaraWarning("Could not find a format matching requirements, disabling antialiasing...");

		int antialiasingLevel;
		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &antialiasingLevel);

		parameters.antialiasingLevel = static_cast<decltype(antialiasingLevel)>(antialiasingLevel);


		onExit.Reset();

		m_context = SDL_GL_CreateContext(m_window);

		if (!m_context) {
			NazaraError(SDL_GetError());

			return false;
		}

		return true;
	}

	void ContextImpl::Destroy()
	{
		if (m_context)
		{
			SDL_GL_DeleteContext(m_context);
			m_context = nullptr;
		}

		if (m_ownsWindow)
		{
			SDL_DestroyWindow(m_window);
			m_window = nullptr;
		}
	}

	void ContextImpl::EnableVerticalSync(bool enabled)
	{

		if (SDL_GL_SetSwapInterval(enabled ? 1 : 0) != 0)
			NazaraError("Vertical sync not supported");
	}

	void ContextImpl::SwapBuffers()
	{
		SDL_GL_SwapWindow(m_window);
	}

	bool ContextImpl::Desactivate()
	{
		return SDL_GL_MakeCurrent(nullptr, nullptr) == 0;
	}

	SDL_Window* ContextImpl::lastActive = nullptr;
}
