// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WIN32_WIN32HELPER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WIN32_WIN32HELPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <memory>
#include <type_traits>
#include <Windows.h>

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

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WIN32_WIN32HELPER_HPP
