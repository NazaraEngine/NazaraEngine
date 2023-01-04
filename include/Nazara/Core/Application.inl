// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Application.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T, typename ...Args>
	T& Nz::Application::AddComponent(Args&& ...args)
	{
		std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
		T& componentRef = *component;
		AddComponent(std::move(component));

		return componentRef;
	}

	inline Application::Application() :
	Application(0, static_cast<const char**>(nullptr))
	{
	}

	inline Application::Application(int argc, char** argv) :
	Application(argc, static_cast<const char**>(argv))
	{
	}

	void Application::AddComponent(std::unique_ptr<ApplicationComponent>&& component)
	{
		m_components.emplace_back(std::move(component));
	}

	inline void Application::Quit()
	{
		m_running = false;
	}
}

#include <Nazara/Core/DebugOff.hpp>
