// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ApplicationBase.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T, typename ...Args>
	T& Nz::ApplicationBase::AddComponent(Args&& ...args)
	{
		std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
		T& componentRef = *component;
		AddComponent(std::move(component));

		return componentRef;
	}

	inline ApplicationBase::ApplicationBase() :
	ApplicationBase(0, static_cast<const char**>(nullptr))
	{
	}

	inline ApplicationBase::ApplicationBase(int argc, char** argv) :
	ApplicationBase(argc, const_cast<const Pointer<const char>*>(argv))
	{
	}

	void ApplicationBase::AddComponent(std::unique_ptr<ApplicationComponent>&& component)
	{
		m_components.emplace_back(std::move(component));
	}

	inline void ApplicationBase::Quit()
	{
		m_running = false;
	}
}

#include <Nazara/Core/DebugOff.hpp>
