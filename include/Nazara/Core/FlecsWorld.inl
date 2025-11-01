// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline FlecsWorld::FlecsWorld() :
	m_systemGraph(m_world)
	{
	}

	template<typename T, typename... Args>
	T& FlecsWorld::AddSystem(Args&&... args)
	{
		return m_systemGraph.AddSystem<T>(std::forward<Args>(args)...);
	}

	inline void FlecsWorld::ClearSystems()
	{
		m_systemGraph.Clear();
	}

	inline flecs::entity FlecsWorld::CreateEntity()
	{
		return m_world.entity();
	}

	inline std::size_t FlecsWorld::GetAliveEntityCount() const
	{
		return 0; // TODO
	}

	inline flecs::world& FlecsWorld::GetWorld()
	{
		return m_world;
	}

	inline const flecs::world& FlecsWorld::GetWorld() const
	{
		return m_world;
	}

	template<typename T>
	T& FlecsWorld::GetSystem() const
	{
		return m_systemGraph.GetSystem<T>();
	}

	template<typename T>
	void FlecsWorld::RemoveSystem()
	{
		return m_systemGraph.RemoveSystem<T>();
	}

	template<typename T>
	T* FlecsWorld::TryGetSystem() const
	{
		return m_systemGraph.TryGetSystem<T>();
	}

	inline FlecsWorld::operator flecs::world&()
	{
		return m_world;
	}

	inline FlecsWorld::operator const flecs::world&() const
	{
		return m_world;
	}
}
