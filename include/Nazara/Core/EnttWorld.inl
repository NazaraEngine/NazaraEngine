// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/EnttWorld.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline EnttWorld::EnttWorld() :
	m_systemGraph(m_registry)
	{
	}

	template<typename T, typename... Args>
	T& EnttWorld::AddSystem(Args&&... args)
	{
		return m_systemGraph.AddSystem<T>(std::forward<Args>(args)...);
	}

	inline entt::handle EnttWorld::CreateEntity()
	{
		return entt::handle(m_registry, m_registry.create());
	}

	inline entt::registry& EnttWorld::GetRegistry()
	{
		return m_registry;
	}

	inline const entt::registry& EnttWorld::GetRegistry() const
	{
		return m_registry;
	}

	template<typename T>
	T& EnttWorld::GetSystem() const
	{
		return m_systemGraph.GetSystem<T>();
	}

	inline EnttWorld::operator entt::registry&()
	{
		return m_registry;
	}

	inline EnttWorld::operator const entt::registry&() const
	{
		return m_registry;
	}
}

#include <Nazara/Core/DebugOff.hpp>
