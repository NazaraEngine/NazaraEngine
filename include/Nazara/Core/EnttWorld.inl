// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline EnttWorld::EnttWorld() :
	m_systemGraph(m_registry)
	{
		m_registry.ctx().emplace<EnttWorld*>(this);
	}

	template<typename T, typename... Args>
	T& EnttWorld::AddSystem(Args&&... args)
	{
		return m_systemGraph.AddSystem<T>(std::forward<Args>(args)...);
	}

	inline void EnttWorld::ClearSystems()
	{
		m_systemGraph.Clear();
	}

	inline entt::handle EnttWorld::CreateEntity()
	{
		return entt::handle(m_registry, m_registry.create());
	}

	inline std::size_t EnttWorld::GetAliveEntityCount() const
	{
		return m_registry.storage<entt::entity>()->free_list();
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

	template<typename T>
	void EnttWorld::RemoveSystem()
	{
		return m_systemGraph.RemoveSystem<T>();
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

