// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ENTTSYSTEMGRAPH_HPP
#define NAZARA_CORE_ENTTSYSTEMGRAPH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Config.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API EnttSystemGraph
	{
		public:
			inline EnttSystemGraph(entt::registry& registry);
			EnttSystemGraph(const EnttSystemGraph&) = delete;
			EnttSystemGraph(EnttSystemGraph&&) = delete;
			~EnttSystemGraph() = default;

			template<typename T, typename... Args> T& AddSystem(Args&&... args);

			template<typename T> T& GetSystem() const;

			template<typename T> void RemoveSystem();

			void Update();
			void Update(Time elapsedTime);

			EnttSystemGraph& operator=(const EnttSystemGraph&) = delete;
			EnttSystemGraph& operator=(EnttSystemGraph&&) = delete;

		private:
			struct NAZARA_CORE_API NodeBase
			{
				virtual ~NodeBase();

				virtual void Update(Time elapsedTime) = 0;

				Int64 executionOrder;
			};

			template<typename T>
			struct Node : NodeBase
			{
				template<typename... Args> Node(Args&&... args);

				void Update(Time elapsedTime) override;

				T system;
			};

			std::unordered_map<entt::id_type, std::size_t /*nodeIndex*/> m_systemToNodes;
			std::vector<NodeBase*> m_orderedNodes;
			std::vector<std::unique_ptr<NodeBase>> m_nodes;
			entt::registry& m_registry;
			Nz::HighPrecisionClock m_clock;
			bool m_systemOrderUpdated;
	};
}

#include <Nazara/Core/EnttSystemGraph.inl>

#endif // NAZARA_CORE_ENTTSYSTEMGRAPH_HPP
