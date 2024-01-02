// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_SYSTEMS_SKELETONSYSTEM_HPP
#define NAZARA_UTILITY_SYSTEMS_SKELETONSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Utility/Config.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API SkeletonSystem
	{
		public:
			static constexpr bool AllowConcurrent = false;
			static constexpr Int64 ExecutionOrder = -1'000;

			SkeletonSystem(entt::registry& registry);
			SkeletonSystem(const SkeletonSystem&) = delete;
			SkeletonSystem(SkeletonSystem&&) = delete;
			~SkeletonSystem();

			void Update(Time elapsedTime);

			SkeletonSystem& operator=(const SkeletonSystem&) = delete;
			SkeletonSystem& operator=(SkeletonSystem&&) = delete;

		private:
			entt::registry& m_registry;
			entt::observer m_sharedSkeletonConstructObserver;
			entt::observer m_skeletonConstructObserver;
	};
}

#include <Nazara/Utility/Systems/SkeletonSystem.inl>

#endif // NAZARA_UTILITY_SYSTEMS_SKELETONSYSTEM_HPP
