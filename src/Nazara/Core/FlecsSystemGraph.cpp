// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/FlecsSystemGraph.hpp>

namespace Nz
{
	FlecsSystemGraph::NodeBase::~NodeBase() = default;

	void FlecsSystemGraph::Update()
	{
		return Update(m_clock.Restart());
	}

	void FlecsSystemGraph::Update(Time elapsedTime)
	{
		if (!m_systemOrderUpdated)
		{
			m_orderedNodes.clear();
			m_orderedNodes.reserve(m_nodes.size());
			for (auto& nodePtr : m_nodes)
			{
				if (nodePtr->HasUpdate())
					m_orderedNodes.emplace_back(nodePtr.get());
			}

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
