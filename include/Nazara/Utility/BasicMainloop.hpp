// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BASIC_MAINLOOP_HPP
#define NAZARA_BASIC_MAINLOOP_HPP

#include <Nazara/Renderer.hpp>

#ifdef NAZARA_PLATFORM_WEB
#include <emscripten/html5.h>
#endif

namespace Nz
{
	template <typename CallbackT>
	void BasicMainloop(RenderWindow& window, CallbackT&& callback)
	{
#ifndef NAZARA_PLATFORM_WEB
		while (window.IsOpen())
		{
			callback();
		}
#else
		emscripten_set_main_loop_arg([] (void* callbackInstance)
		{
			try
			{
				(*static_cast<decltype(&callback)>(callbackInstance))();
			}
			catch(std::exception e)
			{
				NazaraDebug(e.what());
			}
		}, &callback, 0, 1);
#endif
	}
}


#endif // NAZARA_BASIC_MAINLOOP_HPP
