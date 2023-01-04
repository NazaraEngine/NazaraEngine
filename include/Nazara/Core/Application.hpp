// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_APPLICATION_HPP
#define NAZARA_CORE_APPLICATION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/Clock.hpp>
#include <atomic>
#include <vector>

namespace Nz
{
	class NAZARA_CORE_API Application
	{
		public:
			inline Application();
			inline Application(int argc, char** argv);
			Application(int argc, const char** argv);
			Application(const Application&) = delete;
			Application(Application&&) = delete;
			~Application() = default;

			template<typename T, typename... Args> T& AddComponent(Args&&... args);
			inline void AddComponent(std::unique_ptr<ApplicationComponent>&& component);

			int Run();

			inline void Quit();

			void Update(Time elapsedTime);

			Application& operator=(const Application&) = delete;
			Application& operator=(Application&&) = delete;

		private:
			std::atomic_bool m_running;
			std::vector<ApplicationComponent> m_components;
			HighPrecisionClock m_clock;
	};
}

#include <Nazara/Core/Application.inl>

#endif // NAZARA_CORE_APPLICATION_HPP
