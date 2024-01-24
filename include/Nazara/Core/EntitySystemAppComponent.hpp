// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ENTITYSYSTEMAPPCOMPONENT_HPP
#define NAZARA_CORE_ENTITYSYSTEMAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/EntityWorld.hpp>

namespace Nz
{
	class NAZARA_CORE_API EntitySystemAppComponent : public ApplicationComponent
	{
		public:
			using ApplicationComponent::ApplicationComponent;
			EntitySystemAppComponent(const EntitySystemAppComponent&) = delete;
			EntitySystemAppComponent(EntitySystemAppComponent&&) = delete;
			~EntitySystemAppComponent() = default;

			template<typename T, typename... Args> T& AddWorld(Args&&... args);

			void Update(Time elapsedTime) override;

			EntitySystemAppComponent& operator=(const EntitySystemAppComponent&) = delete;
			EntitySystemAppComponent& operator=(EntitySystemAppComponent&&) = delete;

		private:
			std::vector<std::unique_ptr<EntityWorld>> m_worlds;
	};
}

#include <Nazara/Core/EntitySystemAppComponent.inl>

#endif // NAZARA_CORE_ENTITYSYSTEMAPPCOMPONENT_HPP
