// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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

	inline void ApplicationBase::AddUpdater(std::unique_ptr<ApplicationUpdater>&& functor)
	{
		auto& updaterEntry = m_updaters.emplace_back();
		updaterEntry.lastUpdate = -Time::Nanosecond();
		updaterEntry.nextUpdate = Time::Zero();
		updaterEntry.updater = std::move(functor);
	}

	template<typename F>
	void ApplicationBase::AddUpdaterFunc(F&& functor)
	{
		static_assert(std::is_invocable_v<F> || std::is_invocable_v<F, Time>, "functor must be callable with either a Time parameter or no parameter");
		return AddUpdater(std::make_unique<ApplicationUpdaterFunctor<std::decay_t<F>>>(std::forward<F>(functor)));
	}

	template<typename F>
	void ApplicationBase::AddUpdaterFunc(FixedInterval fixedInterval, F&& functor)
	{
		return AddUpdaterFunc<F, true>(fixedInterval.interval, std::forward<F>(functor));
	}

	template<typename F>
	void ApplicationBase::AddUpdaterFunc(Interval interval, F&& functor)
	{
		return AddUpdaterFunc<F, false>(interval.interval, std::forward<F>(functor));
	}

	inline void ApplicationBase::ClearComponents()
	{
		m_components.clear();
	}

	inline const CommandLineParameters& ApplicationBase::GetCommandLineParameters() const
	{
		return m_commandLineParams;
	}

	template<typename T>
	T& ApplicationBase::GetComponent()
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();
		if (componentIndex >= m_components.size() || !m_components[componentIndex])
			throw std::runtime_error("component not found");

		return static_cast<T&>(*m_components[componentIndex]);
	}
	
	template<typename T>
	const T& ApplicationBase::GetComponent() const
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();
		if (componentIndex >= m_components.size() || !m_components[componentIndex])
			throw std::runtime_error("component not found");

		return static_cast<const T&>(*m_components[componentIndex]);
	}

	template<typename T>
	bool ApplicationBase::HasComponent() const
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();
		if (componentIndex >= m_components.size())
			return false;

		return m_components[componentIndex] != nullptr;
	}

	inline void ApplicationBase::Quit()
	{
		m_running = false;
	}

	template<typename T>
	T* ApplicationBase::TryGetComponent()
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();
		if (componentIndex >= m_components.size())
			return nullptr;

		return static_cast<T*>(m_components[componentIndex].get());
	}

	template<typename T>
	const T* ApplicationBase::TryGetComponent() const
	{
		std::size_t componentIndex = ApplicationComponentRegistry<T>::GetComponentId();
		if (componentIndex >= m_components.size())
			return nullptr;

		return static_cast<const T*>(m_components[componentIndex].get());
	}

	inline ApplicationBase* ApplicationBase::Instance()
	{
		return s_instance;
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

	template<typename F, bool Fixed>
	void ApplicationBase::AddUpdaterFunc(Time interval, F&& functor)
	{
		if constexpr (std::is_invocable_r_v<void, F> || std::is_invocable_r_v<void, F, Time>)
			return AddUpdater(std::make_unique<ApplicationUpdaterFunctorWithInterval<std::decay_t<F>, Fixed>>(std::forward<F>(functor), interval));
		else if constexpr (std::is_invocable_r_v<Time, F> || std::is_invocable_r_v<Time, F, Time>)
			return AddUpdater(std::make_unique<ApplicationUpdaterFunctor<std::decay_t<F>>>(std::forward<F>(functor)));
		else
			static_assert(AlwaysFalse<F>(), "functor must be callable with either elapsed time or nothing and return void or next update time");
	}
}

#include <Nazara/Core/DebugOff.hpp>
#include "ApplicationBase.hpp"
