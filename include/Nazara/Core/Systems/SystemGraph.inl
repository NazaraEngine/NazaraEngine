// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Systems/SystemGraph.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename, typename = void>
		struct SystemGraphAllowConcurrent : std::bool_constant<true> {};

		template<typename T>
		struct SystemGraphAllowConcurrent<T, std::void_t<decltype(T::AllowConcurrent)>> : std::bool_constant<T::AllowConcurrent> {};

		template<typename, typename = void>
		struct SystemGraphExecutionOrder : std::integral_constant<Int64, 0> {};

		template<typename T>
		struct SystemGraphExecutionOrder<T, std::void_t<decltype(T::ExecutionOrder)>> : std::integral_constant<Int64, T::ExecutionOrder> {};
	}

	template<typename T>
	template<typename... Args>
	SystemGraph::Node<T>::Node(Args&&... args) :
	system(std::forward<Args>(args)...)
	{
	}

	template<typename T>
	void SystemGraph::Node<T>::Update(float elapsedTime)
	{
		system.Update(elapsedTime);
	}

	inline SystemGraph::SystemGraph(entt::registry& registry) :
	m_registry(registry),
	m_systemOrderUpdated(true)
	{
	}

	template<typename T, typename... Args>
	T& SystemGraph::AddSystem(Args&&... args)
	{
		NazaraAssert(m_systemToNodes.find(entt::type_hash<T>()) == m_systemToNodes.end(), "this system already exists");

		auto nodePtr = std::make_unique<Node<T>>(m_registry, std::forward<Args>(args)...);
		nodePtr->executionOrder = Detail::SystemGraphExecutionOrder<T>();

		T& system = nodePtr->system;

		std::size_t nodeIndex = m_nodes.size();

		m_nodes.emplace_back(std::move(nodePtr));
		m_systemToNodes.emplace(entt::type_hash<T>(), nodeIndex);
		m_systemOrderUpdated = false;

		return system;
	}

	template<typename T>
	T& SystemGraph::GetSystem() const
	{
		auto it = m_systemToNodes.find(entt::type_hash<T>());
		if (it == m_systemToNodes.end())
			throw std::runtime_error("this system is not part of the graph");

		auto& node = static_cast<Node<T>&>(*m_nodes[it->second]);
		return node.system;
	}
}

#include <Nazara/Core/DebugOff.hpp>
