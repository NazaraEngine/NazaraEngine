// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONUPDATER_HPP
#define NAZARA_CORE_APPLICATIONUPDATER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Time.hpp>

namespace Nz
{
	class NAZARA_CORE_API ApplicationUpdater
	{
		public:
			ApplicationUpdater() = default;
			ApplicationUpdater(const ApplicationUpdater&) = delete;
			ApplicationUpdater(ApplicationUpdater&&) = delete;
			virtual ~ApplicationUpdater();

			virtual void Update(Time elapsedTime) = 0;

			ApplicationUpdater& operator=(const ApplicationUpdater&) = delete;
			ApplicationUpdater& operator=(ApplicationUpdater&&) = delete;
	};

	template<typename F>
	class ApplicationUpdaterFunctor : public ApplicationUpdater
	{
		public:
			ApplicationUpdaterFunctor(F functor);

			void Update(Time elapsedTime) override;

		private:
			F m_functor;
	};
}

#include <Nazara/Core/ApplicationUpdater.inl>

#endif // NAZARA_CORE_APPLICATIONUPDATER_HPP
