// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ApplicationBase.hpp>
#include <Nazara/Core/Error.hpp>
#ifdef NAZARA_PLATFORM_WEB
#include <emscripten/html5.h>
#endif

namespace Nz
{
	ApplicationBase::ApplicationBase(int argc, const Pointer<const char>* argv) :
	m_running(true),
	m_commandLineParams(CommandLineParameters::Parse(argc, argv)),
	m_currentTime(Time::Zero())
	{
		NazaraAssert(s_instance == nullptr, "only one instance of ApplicationBase can exist at a given time");
		s_instance = this;
	}

	ApplicationBase::~ApplicationBase()
	{
		assert(s_instance == this);
		s_instance = nullptr;
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
		m_currentTime += elapsedTime;

		for (auto& updaterEntry : m_updaters)
		{
			if (updaterEntry.nextUpdate > m_currentTime)
				continue;

			Time timeSinceLastUpdate = m_currentTime - updaterEntry.lastUpdate;

			if NAZARA_UNLIKELY(updaterEntry.lastUpdate < Time::Zero())
			{
				// First call
				timeSinceLastUpdate = Time::Zero();
				updaterEntry.lastUpdate = m_currentTime;
			}

			Time interval = updaterEntry.updater->Update(timeSinceLastUpdate);
			if (interval >= Time::Zero())
				updaterEntry.nextUpdate = m_currentTime + interval;
			else
				updaterEntry.nextUpdate = updaterEntry.lastUpdate + (-interval);

			updaterEntry.lastUpdate = m_currentTime;
			updaterEntry.nextUpdate = std::max(updaterEntry.nextUpdate, m_currentTime);
		}

		for (auto&& [typeHash, componentPtr] : m_components)
		{
			NazaraUnused(typeHash);

			if (componentPtr)
				componentPtr->Update(elapsedTime);
		}

		return m_running;
	}

	ApplicationBase* ApplicationBase::s_instance = nullptr;
}
