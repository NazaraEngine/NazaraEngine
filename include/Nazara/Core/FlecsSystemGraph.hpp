// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_FLECSSYSTEMGRAPH_HPP
#define NAZARA_CORE_FLECSSYSTEMGRAPH_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <flecs.h>
#include <functional>
#include <unordered_map>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API FlecsSystemGraph
	{
		public:
			inline FlecsSystemGraph(flecs::world& world);
			FlecsSystemGraph(const FlecsSystemGraph&) = delete;
			FlecsSystemGraph(FlecsSystemGraph&&) = delete;
			inline ~FlecsSystemGraph();

			template<typename T, typename... Args> T& AddSystem(Args&&... args);

			inline void Clear();

			template<typename T> T& GetSystem() const;

			template<typename T> void RemoveSystem();

			template<typename T> T* TryGetSystem() const;

			void Update();
			void Update(Time elapsedTime);

			FlecsSystemGraph& operator=(const FlecsSystemGraph&) = delete;
			FlecsSystemGraph& operator=(FlecsSystemGraph&&) = delete;

		private:
			struct NAZARA_CORE_API NodeBase
			{
				virtual ~NodeBase();

				virtual bool HasUpdate() const = 0;
				virtual void Update(Time elapsedTime) = 0;

				Int64 executionOrder;
			};

			template<typename T, bool CanUpdate>
			struct Node : NodeBase
			{
				template<typename... Args> Node(Args&&... args);

				bool HasUpdate() const override;
				void Update(Time elapsedTime) override;

				T system;
			};

			template<typename T> constexpr UInt64 TypeHash();

			std::unordered_map<UInt64 /*typehash*/, std::size_t /*nodeIndex*/> m_systemToNodes;
			std::vector<NodeBase*> m_orderedNodes;
			std::vector<std::unique_ptr<NodeBase>> m_nodes;
			flecs::world& m_world;
			Nz::HighPrecisionClock m_clock;
			bool m_systemOrderUpdated;
	};
}

#include <Nazara/Core/FlecsSystemGraph.inl>

#endif // NAZARA_CORE_FLECSSYSTEMGRAPH_HPP
