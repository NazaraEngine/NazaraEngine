// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <stdexcept>

namespace Nz
{
	namespace Detail
	{
		template<typename, typename = void>
		struct FlecsSystemGraphAllowConcurrent : std::true_type {};

		template<typename T>
		struct FlecsSystemGraphAllowConcurrent<T, std::void_t<decltype(T::AllowConcurrent)>> : std::bool_constant<T::AllowConcurrent> {};

		template<typename, typename = void>
		struct FlecsSystemGraphExecutionOrder : std::integral_constant<Int64, 0> {};

		template<typename T>
		struct FlecsSystemGraphExecutionOrder<T, std::void_t<decltype(T::ExecutionOrder)>> : std::integral_constant<Int64, T::ExecutionOrder> {};

		template<typename, typename = void>
		struct FlecsSystemGraphHasUpdate : std::false_type {};

		template<typename T>
		struct FlecsSystemGraphHasUpdate<T, std::void_t<decltype(std::declval<T>().Update(std::declval<Time>()))>> : std::true_type {};
	}

	template<typename T, bool CanUpdate>
	template<typename... Args>
	FlecsSystemGraph::Node<T, CanUpdate>::Node(Args&&... args) :
	system(std::forward<Args>(args)...)
	{
	}

	template<typename T, bool CanUpdate>
	bool FlecsSystemGraph::Node<T, CanUpdate>::HasUpdate() const
	{
		return CanUpdate;
	}

	template<typename T, bool CanUpdate>
	void FlecsSystemGraph::Node<T, CanUpdate>::Update(Time elapsedTime)
	{
		if constexpr (CanUpdate)
			system.Update(elapsedTime);
	}

	inline FlecsSystemGraph::FlecsSystemGraph(flecs::world& world) :
	m_world(world),
	m_systemOrderUpdated(true)
	{
	}

	inline FlecsSystemGraph::~FlecsSystemGraph()
	{
		Clear();
	}

	template<typename T, typename... Args>
	T& FlecsSystemGraph::AddSystem(Args&&... args)
	{
		NazaraAssertMsg(m_systemToNodes.find(TypeHash<T>()) == m_systemToNodes.end(), "this system already exists");

		constexpr bool CanUpdate = Detail::FlecsSystemGraphHasUpdate<T>();

		auto nodePtr = std::make_unique<Node<T, CanUpdate>>(m_world, std::forward<Args>(args)...);
		nodePtr->executionOrder = Detail::FlecsSystemGraphExecutionOrder<T>();

		T& system = nodePtr->system;

		std::size_t nodeIndex = m_nodes.size();

		m_nodes.emplace_back(std::move(nodePtr));
		m_systemToNodes.emplace(TypeHash<T>(), nodeIndex);
		m_systemOrderUpdated = false;

		return system;
	}

	inline void FlecsSystemGraph::Clear()
	{
		// std::vector does not guarantee order of destruction, do it ourselves
		for (auto rit = m_nodes.rbegin(); rit != m_nodes.rend(); ++rit)
			rit->reset();

		m_nodes.clear();
		m_orderedNodes.clear();
		m_systemToNodes.clear();
		m_systemOrderUpdated = true;
	}

	template<typename T>
	T& FlecsSystemGraph::GetSystem() const
	{
		T* system = TryGetSystem<T>();
		if (!system)
			throw std::runtime_error("this system is not part of the graph");

		return *system;
	}

	template<typename T>
	void FlecsSystemGraph::RemoveSystem()
	{
		auto it = m_systemToNodes.find(TypeHash<T>());
		if (it == m_systemToNodes.end())
			return;

		m_nodes.erase(m_nodes.begin() + it->second);
		m_systemToNodes.erase(it);
		m_systemOrderUpdated = false;
	}

	template<typename T>
	T* FlecsSystemGraph::TryGetSystem() const
	{
		constexpr bool CanUpdate = Detail::FlecsSystemGraphHasUpdate<T>();

		auto it = m_systemToNodes.find(TypeHash<T>());
		if (it == m_systemToNodes.end())
			return nullptr;

		auto& node = static_cast<Node<T, CanUpdate>&>(*m_nodes[it->second]);
		return &node.system;
	}

	template<typename T>
	constexpr UInt64 FlecsSystemGraph::TypeHash()
	{
		return FNV1a64(TypeName<T>());
	}
}
