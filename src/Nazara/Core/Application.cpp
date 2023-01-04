// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Application.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Application::Application(int argc, const char** argv) :
	m_running(true)
	{
	}

	int Application::Run()
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

	void Application::Update(Time elapsedTime)
	{
	}
}
