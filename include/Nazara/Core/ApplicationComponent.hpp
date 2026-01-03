// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONCOMPONENT_HPP
#define NAZARA_CORE_APPLICATIONCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class ApplicationBase;

	class NAZARA_CORE_API ApplicationComponent
	{
		friend ApplicationBase;

		public:
			inline ApplicationComponent(ApplicationBase& app);
			ApplicationComponent(const ApplicationComponent&) = delete;
			ApplicationComponent(ApplicationComponent&&) = delete;
			virtual ~ApplicationComponent();

			inline ApplicationBase& GetApp();
			inline const ApplicationBase& GetApp() const;

			ApplicationComponent& operator=(const ApplicationComponent&) = delete;
			ApplicationComponent& operator=(ApplicationComponent&&) = delete;

		private:
			virtual void Update(Time elapsedTime);

			ApplicationBase& m_app;
	};
}

#include <Nazara/Core/ApplicationComponent.inl>

#endif // NAZARA_CORE_APPLICATIONCOMPONENT_HPP
