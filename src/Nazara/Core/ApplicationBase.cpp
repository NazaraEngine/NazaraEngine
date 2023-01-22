// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ApplicationBase.hpp>
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

		while (m_running)
		{
			Time elapsedTime = m_clock.Restart();
			Update(elapsedTime);
		}

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
