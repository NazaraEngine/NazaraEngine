// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_SYSTEMS_SYSTEMGRAPH_HPP
#define NAZARA_CORE_SYSTEMS_SYSTEMGRAPH_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Utils/MovablePtr.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API SystemGraph
	{
		public:
			inline SystemGraph(entt::registry& registry);
			SystemGraph(const SystemGraph&) = delete;
			SystemGraph(SystemGraph&&) = delete;
			~SystemGraph() = default;

			template<typename T, typename... Args> T& AddSystem(Args&&... args);

			template<typename T> T& GetSystem() const;

			void Update();
			void Update(Time elapsedTime);

			SystemGraph& operator=(const SystemGraph&) = delete;
			SystemGraph& operator=(SystemGraph&&) = delete;

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

#include <Nazara/Core/Systems/SystemGraph.inl>

#endif // NAZARA_CORE_SYSTEMS_SYSTEMGRAPH_HPP
