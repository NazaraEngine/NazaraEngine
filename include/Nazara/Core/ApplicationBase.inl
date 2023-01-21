// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ApplicationBase.hpp>
#include <Nazara/Core/ApplicationComponentRegistry.hpp>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline ApplicationBase::ApplicationBase() :
	ApplicationBase(0, static_cast<const char**>(nullptr))
	{
	}

	inline ApplicationBase::ApplicationBase(int argc, char** argv) :
	ApplicationBase(argc, const_cast<const Pointer<const char>*>(argv))
	{
	}

	template<typename T, typename... Args>
	T& ApplicationBase::AddComponent(Args&&... args)
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();

		std::unique_ptr<T> component = std::make_unique<T>(*this, std::forward<Args>(args)...);
		T& componentRef = *component;

		if (componentIndex >= m_components.size())
			m_components.resize(componentIndex + 1);
		else if (m_components[componentIndex] != nullptr)
			throw std::runtime_error("component was added multiple times");

		m_components[componentIndex] = std::move(component);

		return componentRef;
	}

	template<typename F>
	void ApplicationBase::AddUpdater(F&& functor)
	{
		m_updaters.emplace_back(std::make_unique<ApplicationUpdaterFunctor<std::decay_t<F>>>(std::forward<F>(functor)));
	}

	inline void ApplicationBase::ClearComponents()
	{
		m_components.clear();
	}

	template<typename T>
	T* ApplicationBase::GetComponent()
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();
		if (componentIndex >= m_components.size())
			return nullptr;

		return m_components[componentIndex].get();
	}
	
	template<typename T>
	const T* ApplicationBase::GetComponent() const
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();
		if (componentIndex >= m_components.size())
			return nullptr;

		return m_components[componentIndex].get();
	}

	inline void ApplicationBase::Quit()
	{
		m_running = false;
	}
}

#include <Nazara/Core/DebugOff.hpp>
