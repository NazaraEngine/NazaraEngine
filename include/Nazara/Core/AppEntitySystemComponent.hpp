// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPENTITYSYSTEMCOMPONENT_HPP
#define NAZARA_CORE_APPENTITYSYSTEMCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Systems/SystemGraph.hpp>

namespace Nz
{
	class NAZARA_CORE_API AppEntitySystemComponent : public ApplicationComponent
	{
		public:
			inline AppEntitySystemComponent(ApplicationBase& app);
			AppEntitySystemComponent(const AppEntitySystemComponent&) = delete;
			AppEntitySystemComponent(AppEntitySystemComponent&&) = delete;
			~AppEntitySystemComponent() = default;

			template<typename T, typename... Args> T& AddSystem(Args&&... args);

			entt::handle CreateEntity();

			entt::registry& GetRegistry();
			const entt::registry& GetRegistry() const;
			template<typename T> T& GetSystem() const;

			void Update(Time elapsedTime) override;

			AppEntitySystemComponent& operator=(const AppEntitySystemComponent&) = delete;
			AppEntitySystemComponent& operator=(AppEntitySystemComponent&&) = delete;

		private:
			entt::registry m_registry;
			SystemGraph m_systemGraph;
	};
}

#include <Nazara/Core/AppEntitySystemComponent.inl>

#endif // NAZARA_CORE_APPENTITYSYSTEMCOMPONENT_HPP
