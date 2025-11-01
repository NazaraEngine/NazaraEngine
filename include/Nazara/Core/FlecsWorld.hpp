// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_FLECSWORLD_HPP
#define NAZARA_CORE_FLECSWORLD_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/EntityWorld.hpp>
#include <Nazara/Core/FlecsSystemGraph.hpp>

namespace Nz
{
	class NAZARA_CORE_API FlecsWorld : public EntityWorld
	{
		public:
			inline FlecsWorld();
			FlecsWorld(const FlecsWorld&) = delete;
			FlecsWorld(FlecsWorld&&) = delete;
			~FlecsWorld() = default;

			template<typename T, typename... Args> T& AddSystem(Args&&... args);

			inline void ClearSystems();

			inline flecs::entity CreateEntity();

			inline std::size_t GetAliveEntityCount() const;
			template<typename T> T& GetSystem() const;
			flecs::world& GetWorld();
			const flecs::world& GetWorld() const;

			template<typename T> void RemoveSystem();

			template<typename T> T* TryGetSystem() const;

			void Update(Time elapsedTime) override;

			operator flecs::world&();
			operator const flecs::world&() const;

			FlecsWorld& operator=(const FlecsWorld&) = delete;
			FlecsWorld& operator=(FlecsWorld&&) = delete;

		private:
			flecs::world m_world;
			FlecsSystemGraph m_systemGraph;
	};
}

#include <Nazara/Core/FlecsWorld.inl>

#endif // NAZARA_CORE_FLECSWORLD_HPP
