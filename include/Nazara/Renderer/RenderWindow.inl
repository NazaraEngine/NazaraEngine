// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Core/ErrorFlags.hpp>

namespace Nz
{
	inline RenderWindow::RenderWindow() :
	m_framerateLimit(0U)
	{
	}

	inline RenderWindow::RenderWindow(VideoMode mode, const String& title, UInt32 style, const RenderWindowParameters& parameters) :
	RenderWindow()
	{
		ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		Create(mode, title, style, parameters);
	}

	inline RenderWindow::RenderWindow(WindowHandle handle, const RenderWindowParameters& parameters)
	{
		ErrorFlags errFlags(ErrorFlag_ThrowException, true);

		Create(handle, parameters);
	}

	inline bool RenderWindow::Create(VideoMode mode, const String& title, UInt32 style, const RenderWindowParameters& parameters)
	{
		m_parameters = parameters;

		return Window::Create(mode, title, style);
	}

	inline bool RenderWindow::Create(WindowHandle handle, const RenderWindowParameters& parameters)
	{
		m_parameters = parameters;

		return Window::Create(handle);
	}

	inline RenderWindowImpl* Nz::RenderWindow::GetImpl()
	{
		return m_impl.get();
	}

	inline bool RenderWindow::IsValid() const
	{
		return m_impl != nullptr;
	}

	inline void RenderWindow::SetFramerateLimit(unsigned int limit)
	{
		m_framerateLimit = limit;
	}
}
