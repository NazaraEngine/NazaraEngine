// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLATFORM_WINDOWINGAPPCOMPONENT_HPP
#define NAZARA_PLATFORM_WINDOWINGAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Window.hpp>

namespace Nz
{
	class NAZARA_PLATFORM_API WindowingAppComponent : public ApplicationComponent
	{
		public:
			inline WindowingAppComponent(ApplicationBase& app);
			WindowingAppComponent(const WindowingAppComponent&) = delete;
			WindowingAppComponent(WindowingAppComponent&&) = delete;
			~WindowingAppComponent() = default;

			template<typename... Args> Window& CreateWindow(Args&&... args);

			inline void DisableQuitOnLastWindowClosed();
			inline void EnableQuitOnLastWindowClosed(bool enable = true);

			inline bool IsQuitOnLastWindowClosedEnabled() const;

			void Update(Time elapsedTime) override;

			WindowingAppComponent& operator=(const WindowingAppComponent&) = delete;
			WindowingAppComponent& operator=(WindowingAppComponent&&) = delete;

		private:
			std::vector<std::unique_ptr<Window>> m_windows;
			bool m_quitOnLastWindowClosed;
	};
}

#include <Nazara/Platform/WindowingAppComponent.inl>

#endif // NAZARA_PLATFORM_WINDOWINGAPPCOMPONENT_HPP
