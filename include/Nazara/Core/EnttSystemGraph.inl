// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <stdexcept>

namespace Nz
{
	namespace Detail
	{
		template<typename, typename = void>
		struct EnttSystemGraphAllowConcurrent : std::true_type {};

		template<typename T>
		struct EnttSystemGraphAllowConcurrent<T, std::void_t<decltype(T::AllowConcurrent)>> : std::bool_constant<T::AllowConcurrent> {};

		template<typename, typename = void>
		struct EnttSystemGraphExecutionOrder : std::integral_constant<Int64, 0> {};

		template<typename T>
		struct EnttSystemGraphExecutionOrder<T, std::void_t<decltype(T::ExecutionOrder)>> : std::integral_constant<Int64, T::ExecutionOrder> {};

		template<typename, typename = void>
		struct EnttSystemGraphHasUpdate : std::false_type {};

		template<typename T>
		struct EnttSystemGraphHasUpdate<T, std::void_t<decltype(std::declval<T>().Update(std::declval<Time>()))>> : std::true_type {};
	}

	template<typename T, bool CanUpdate>
	template<typename... Args>
	EnttSystemGraph::Node<T, CanUpdate>::Node(Args&&... args) :
	system(std::forward<Args>(args)...)
	{
	}

	template<typename T, bool CanUpdate>
	bool EnttSystemGraph::Node<T, CanUpdate>::HasUpdate() const
	{
		return CanUpdate;
	}

	template<typename T, bool CanUpdate>
	void EnttSystemGraph::Node<T, CanUpdate>::Update(Time elapsedTime)
	{
		if constexpr (CanUpdate)
			system.Update(elapsedTime);
	}

	inline EnttSystemGraph::EnttSystemGraph(entt::registry& registry) :
	m_registry(registry),
	m_systemOrderUpdated(true)
	{
	}

	inline EnttSystemGraph::~EnttSystemGraph()
	{
	}

	template<typename T, typename... Args>
	T& EnttSystemGraph::AddSystem(Args&&... args)
	{
		NazaraAssert(m_systemToNodes.find(entt::type_hash<T>()) == m_systemToNodes.end(), "this system already exists");

		constexpr bool CanUpdate = Detail::EnttSystemGraphHasUpdate<T>();

		auto nodePtr = std::make_unique<Node<T, CanUpdate>>(m_registry, std::forward<Args>(args)...);
		nodePtr->executionOrder = Detail::EnttSystemGraphExecutionOrder<T>();

		T& system = nodePtr->system;

		std::size_t nodeIndex = m_nodes.size();

		m_nodes.emplace_back(std::move(nodePtr));
		m_systemToNodes.emplace(entt::type_hash<T>(), nodeIndex);
		m_systemOrderUpdated = false;

		return system;
	}

	inline void EnttSystemGraph::Clear()
	{
		// std::vector does not guarantee order of destruction, do it ourselves
		for (auto rit = m_nodes.rbegin(); rit != m_nodes.rend(); ++rit)
			rit->reset();

		m_orderedNodes.clear();
		m_systemToNodes.clear();
		m_systemOrderUpdated = true;
	}

	template<typename T>
	T& EnttSystemGraph::GetSystem() const
	{
		constexpr bool CanUpdate = Detail::EnttSystemGraphHasUpdate<T>();

		auto it = m_systemToNodes.find(entt::type_hash<T>());
		if (it == m_systemToNodes.end())
			throw std::runtime_error("this system is not part of the graph");

		auto& node = static_cast<Node<T, CanUpdate>&>(*m_nodes[it->second]);
		return node.system;
	}

	template<typename T>
	void EnttSystemGraph::RemoveSystem()
	{
		auto it = m_systemToNodes.find(entt::type_hash<T>());
		if (it == m_systemToNodes.end())
			return;

		m_nodes.erase(m_nodes.begin() + it->second);
		m_systemToNodes.erase(it);
		m_systemOrderUpdated = false;
	}
}

