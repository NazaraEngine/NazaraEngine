// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONCOMPONENT_HPP
#define NAZARA_CORE_APPLICATIONCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class ApplicationBase;

	class NAZARA_CORE_API ApplicationComponent
	{
		public:
			inline ApplicationComponent(ApplicationBase& app);
			ApplicationComponent(const ApplicationComponent&) = delete;
			ApplicationComponent(ApplicationComponent&&) = delete;
			virtual ~ApplicationComponent();

			inline ApplicationBase& GetApp();
			inline const ApplicationBase& GetApp() const;

			virtual void Update(Time elapsedTime);

			ApplicationComponent& operator=(const ApplicationComponent&) = delete;
			ApplicationComponent& operator=(ApplicationComponent&&) = delete;

		private:
			ApplicationBase& m_app;
	};
}

#include <Nazara/Core/ApplicationComponent.inl>

#endif // NAZARA_CORE_APPLICATIONCOMPONENT_HPP
