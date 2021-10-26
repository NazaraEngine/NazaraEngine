// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_WIN32_WIN32HELPER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_WIN32_WIN32HELPER_HPP

#include <Nazara/Prerequisites.hpp>
#include <memory>
#include <type_traits>
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

#endif // NAZARA_OPENGLRENDERER_WRAPPER_WIN32_WIN32HELPER_HPP
