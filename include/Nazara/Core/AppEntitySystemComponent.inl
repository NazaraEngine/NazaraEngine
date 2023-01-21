// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AppEntitySystemComponent.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline AppEntitySystemComponent::AppEntitySystemComponent(ApplicationBase& app) :
	ApplicationComponent(app),
	m_systemGraph(m_registry)
	{
	}

	template<typename T, typename... Args>
	T& AppEntitySystemComponent::AddSystem(Args&&... args)
	{
		return m_systemGraph.AddSystem<T>(std::forward<Args>(args)...);
	}

	inline entt::handle AppEntitySystemComponent::CreateEntity()
	{
		return entt::handle(m_registry, m_registry.create());
	}

	inline entt::registry& AppEntitySystemComponent::GetRegistry()
	{
		return m_registry;
	}

	inline const entt::registry& AppEntitySystemComponent::GetRegistry() const
	{
		return m_registry;
	}

	template<typename T>
	T& AppEntitySystemComponent::GetSystem() const
	{
		return m_systemGraph.GetSystem<T>();
	}
}

#include <Nazara/Core/DebugOff.hpp>
