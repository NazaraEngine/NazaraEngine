// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONBASE_HPP
#define NAZARA_CORE_APPLICATIONBASE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/ApplicationUpdater.hpp>
#include <Nazara/Core/Clock.hpp>
#include <atomic>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API ApplicationBase
	{
		public:
			struct FixedInterval { Time interval; };
			struct Interval { Time interval; };

			inline ApplicationBase();
			inline ApplicationBase(int argc, char** argv);
			ApplicationBase(int argc, const Pointer<const char>* argv);
			ApplicationBase(const ApplicationBase&) = delete;
			ApplicationBase(ApplicationBase&&) = delete;
			~ApplicationBase() = default;

			inline void AddUpdater(std::unique_ptr<ApplicationUpdater>&& functor);
			template<typename F> void AddUpdaterFunc(F&& functor);
			template<typename F> void AddUpdaterFunc(FixedInterval fixedInterval, F&& functor);
			template<typename F> void AddUpdaterFunc(Interval interval, F&& functor);

			inline void ClearComponents();

			template<typename T> T& GetComponent();
			template<typename T> const T& GetComponent() const;

			int Run();

			inline void Quit();

			bool Update(Time elapsedTime);

			ApplicationBase& operator=(const ApplicationBase&) = delete;
			ApplicationBase& operator=(ApplicationBase&&) = delete;

		protected:
			template<typename T, typename... Args> T& AddComponent(Args&&... args);

		private:
			template<typename F, bool Fixed> void AddUpdaterFunc(Time interval, F&& functor);

			struct Updater
			{
				std::unique_ptr<ApplicationUpdater> updater;
				Time lastUpdate;
				Time nextUpdate;
			};

			std::atomic_bool m_running;
			std::vector<std::unique_ptr<ApplicationComponent>> m_components;
			std::vector<Updater> m_updaters;
			HighPrecisionClock m_clock;
			Time m_currentTime;
	};
}

#include <Nazara/Core/ApplicationBase.inl>

#endif // NAZARA_CORE_APPLICATIONBASE_HPP
