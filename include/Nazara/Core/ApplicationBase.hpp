// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONBASE_HPP
#define NAZARA_CORE_APPLICATIONBASE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/ApplicationUpdater.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/CommandLineParameters.hpp>
#include <atomic>
#include <string>
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
			~ApplicationBase();

			inline void AddUpdater(std::unique_ptr<ApplicationUpdater>&& functor);
			template<typename F> void AddUpdaterFunc(F&& functor);
			template<typename F> void AddUpdaterFunc(FixedInterval fixedInterval, F&& functor);
			template<typename F> void AddUpdaterFunc(Interval interval, F&& functor);

			inline void ClearComponents();

			inline const CommandLineParameters& GetCommandLineParameters() const;
			template<typename T> T& GetComponent();
			template<typename T> const T& GetComponent() const;

			inline void Quit();

			int Run();

			bool Update(Time elapsedTime);

			ApplicationBase& operator=(const ApplicationBase&) = delete;
			ApplicationBase& operator=(ApplicationBase&&) = delete;

			static inline ApplicationBase* Instance();

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
			CommandLineParameters m_commandLineParams;
			HighPrecisionClock m_clock;
			Time m_currentTime;

			static ApplicationBase* s_instance;
	};
}

#include <Nazara/Core/ApplicationBase.inl>

#endif // NAZARA_CORE_APPLICATIONBASE_HPP
