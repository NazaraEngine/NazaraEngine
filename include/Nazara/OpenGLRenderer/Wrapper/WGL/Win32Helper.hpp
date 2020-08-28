// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WIN32HELPER_HPP
#define NAZARA_OPENGLRENDERER_WIN32HELPER_HPP

#include <Nazara/Prerequisites.hpp>
#include <windows.h>

namespace Nz::GL
{
	struct WindowDeleter
	{
		void operator()(HWND handle) const
		{
			DestroyWindow(handle);
		}
	};

	using HWNDHandle = std::unique_ptr<std::remove_pointer_t<HWND>, WindowDeleter>;
}

#endif
