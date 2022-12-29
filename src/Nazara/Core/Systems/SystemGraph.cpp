// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Systems/SystemGraph.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SystemGraph::NodeBase::~NodeBase() = default;

	void SystemGraph::Update()
	{
		return Update(m_clock.Restart());
	}

	void SystemGraph::Update(Time elapsedTime)
	{
		if (!m_systemOrderUpdated)
		{
			m_orderedNodes.clear();
			m_orderedNodes.reserve(m_nodes.size());
			for (auto& nodePtr : m_nodes)
				m_orderedNodes.emplace_back(nodePtr.get());

			std::sort(m_orderedNodes.begin(), m_orderedNodes.end(), [](const NodeBase* a, const NodeBase* b)
			{
				return a->executionOrder < b->executionOrder;
			});

			m_systemOrderUpdated = true;
		}

		for (NodeBase* node : m_orderedNodes)
			node->Update(elapsedTime);
	}
}
