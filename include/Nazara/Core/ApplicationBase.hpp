// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATIONBASE_HPP
#define NAZARA_CORE_APPLICATIONBASE_HPP

#include <Nazara/Prerequisites.hpp>
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
			inline ApplicationBase();
			inline ApplicationBase(int argc, char** argv);
			ApplicationBase(int argc, const Pointer<const char>* argv);
			ApplicationBase(const ApplicationBase&) = delete;
			ApplicationBase(ApplicationBase&&) = delete;
			~ApplicationBase() = default;

			template<typename F> void AddUpdater(F&& functor);

			inline void ClearComponents();

			template<typename T> T* GetComponent();
			template<typename T> const T* GetComponent() const;

			int Run();

			inline void Quit();

			bool Update(Time elapsedTime);

			ApplicationBase& operator=(const ApplicationBase&) = delete;
			ApplicationBase& operator=(ApplicationBase&&) = delete;

		protected:
			template<typename T, typename... Args> T& AddComponent(Args&&... args);

		private:
			std::atomic_bool m_running;
			std::vector<std::unique_ptr<ApplicationComponent>> m_components;
			std::vector<std::unique_ptr<ApplicationUpdater>> m_updaters;
			HighPrecisionClock m_clock;
	};
}

#include <Nazara/Core/ApplicationBase.inl>

#endif // NAZARA_CORE_APPLICATIONBASE_HPP
