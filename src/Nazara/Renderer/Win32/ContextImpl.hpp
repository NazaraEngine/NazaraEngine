// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONTEXTIMPL_HPP
#define NAZARA_CONTEXTIMPL_HPP

#include <Nazara/Renderer/ContextParameters.hpp>
#include <windows.h>

class NzContextImpl
{
	public:
		NzContextImpl();

		bool Activate();
		bool Create(NzContextParameters& parameters);
		void Destroy();
		void SwapBuffers();

		static bool Desactivate();

	private:
		HDC m_deviceContext;
		HGLRC m_context;
		HWND m_window;
		bool m_ownsWindow;
};

#endif // NAZARA_CONTEXTIMPL_HPP
