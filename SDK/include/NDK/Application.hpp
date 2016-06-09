// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_APPLICATION_HPP
#define NDK_APPLICATION_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/World.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Utility/Window.hpp>
#include <list>
#include <vector>

namespace Ndk
{
	class NDK_API Application
	{
		public:
			inline Application();
			Application(const Application&) = delete;
			Application(Application&&) = delete;
			inline ~Application();

			#ifndef NDK_SERVER
			template<typename T, typename... Args> T& AddWindow(Args&&... args);
			#endif
			template<typename... Args> World& AddWorld(Args&&... args);

			inline float GetUpdateTime() const;

			bool Run();

			#ifndef NDK_SERVER
			inline void MakeExitOnLastWindowClosed(bool exitOnClosedWindows);
			#endif

			inline void Quit();

			Application& operator=(const Application&) = delete;
			Application& operator=(Application&&) = delete;

			inline static Application* Instance();

		private:
			#ifndef NDK_SERVER
			std::vector<std::unique_ptr<Nz::Window>> m_windows;
			#endif
			std::list<World> m_worlds;
			Nz::Clock m_updateClock;
			#ifndef NDK_SERVER
			bool m_exitOnClosedWindows;
			#endif
			bool m_shouldQuit;
			float m_updateTime;

			static Application* s_application;
	};
}

#include <NDK/Application.inl>

#endif // NDK_APPLICATION_HPP
