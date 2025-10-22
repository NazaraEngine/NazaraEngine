// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SYSTEMS_SKELETONSYSTEM_HPP
#define NAZARA_CORE_SYSTEMS_SKELETONSYSTEM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <flecs.h>

namespace Nz
{
	class NAZARA_CORE_API SkeletonSystem
	{
		public:
			static constexpr bool AllowConcurrent = false;
			static constexpr Int64 ExecutionOrder = -1'000;

			SkeletonSystem(flecs::world& world);
			SkeletonSystem(const SkeletonSystem&) = delete;
			SkeletonSystem(SkeletonSystem&&) = delete;
			~SkeletonSystem();

			void Update(Time elapsedTime);

			SkeletonSystem& operator=(const SkeletonSystem&) = delete;
			SkeletonSystem& operator=(SkeletonSystem&&) = delete;

		private:
			flecs::observer m_sharedSkeletonObserver;
			flecs::observer m_skeletonObserver;
			flecs::world& m_world;
	};
}

#include <Nazara/Core/Systems/SkeletonSystem.inl>

#endif // NAZARA_CORE_SYSTEMS_SKELETONSYSTEM_HPP
