// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ApplicationBase.hpp>
#include <Nazara/Core/Error.hpp>
#ifdef NAZARA_PLATFORM_WEB
#include <emscripten/html5.h>
#endif
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ApplicationBase::ApplicationBase(int argc, const Pointer<const char>* argv) :
	m_running(true)
	{
	}

	int ApplicationBase::Run()
	{
		// Ignore time between creation and Run() call
		m_clock.Restart();

#ifndef NAZARA_PLATFORM_WEB
		while (m_running)
		{
			Time elapsedTime = m_clock.Restart();
			Update(elapsedTime);
		}
#else
		emscripten_set_main_loop_arg([](void* application)
		{
			ApplicationBase* app = static_cast<ApplicationBase*>(application);
			if (!app->m_running)
			{
				emscripten_cancel_main_loop();
				return;
			}

			try
			{
				Time elapsedTime = app->m_clock.Restart();
				app->Update(elapsedTime);
			}
			catch (const std::exception& e)
			{
				NazaraError(e.what());
			}
		}, this, 0, 1);
		emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
#endif

		return 0;
	}

	bool ApplicationBase::Update(Time elapsedTime)
	{
		for (auto& componentPtr : m_components)
		{
			if (componentPtr)
				componentPtr->Update(elapsedTime);
		}

		for (auto& updater : m_updaters)
			updater->Update(elapsedTime);

		return m_running;
	}
}
