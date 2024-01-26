// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONUPDATER_HPP
#define NAZARA_CORE_APPLICATIONUPDATER_HPP

#include <NazaraUtils/Prerequisites.hpp>
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

			virtual Time Update(Time elapsedTime) = 0;

			ApplicationUpdater& operator=(const ApplicationUpdater&) = delete;
			ApplicationUpdater& operator=(ApplicationUpdater&&) = delete;
	};

	template<typename F>
	class ApplicationUpdaterFunctor : public ApplicationUpdater
	{
		public:
			ApplicationUpdaterFunctor(F functor);

			Time Update(Time elapsedTime) override;

		private:
			template<typename... Args> Time TriggerFunctor(Args&&... args);

			F m_functor;
	};

	template<typename F, bool FixedInterval>
	class ApplicationUpdaterFunctorWithInterval : public ApplicationUpdater
	{
		public:
			ApplicationUpdaterFunctorWithInterval(F functor, Time interval);

			Time Update(Time elapsedTime) override;

		private:
			Time m_interval;
			F m_functor;
	};
}

#include <Nazara/Core/ApplicationUpdater.inl>

#endif // NAZARA_CORE_APPLICATIONUPDATER_HPP
