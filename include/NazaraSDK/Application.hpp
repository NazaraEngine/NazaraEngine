// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_APPLICATION_HPP
#define NDK_APPLICATION_HPP

#include <NazaraSDK/Prerequisites.hpp>
#include <NazaraSDK/World.hpp>
#include <Nazara/Core/Clock.hpp>
#include <map>
#include <list>
#include <set>

#ifndef NDK_SERVER
#include <Nazara/Core/Log.hpp>
#include <Nazara/Platform/Window.hpp>
#endif

namespace Ndk
{
	class NDK_API Application
	{
		public:
			inline Application();
			Application(int argc, char* argv[]);
			Application(const Application&) = delete;
			Application(Application&&) = delete;
			inline ~Application();

			#ifndef NDK_SERVER
			template<typename T, typename... Args> T& AddWindow(Args&&... args);
			#endif
			template<typename... Args> World& AddWorld(Args&&... args);

			inline const std::set<Nz::String>& GetOptions() const;
			inline const std::map<Nz::String, Nz::String>& GetParameters() const;

			inline float GetUpdateTime() const;

			inline bool HasOption(const Nz::String& option) const;
			inline bool HasParameter(const Nz::String& key, Nz::String* value) const;

			#ifndef NDK_SERVER
			inline bool IsConsoleEnabled() const;
			inline bool IsFPSCounterEnabled() const;
			#endif

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
			struct WindowInfo
			{
				inline WindowInfo(std::unique_ptr<Nz::Window>&& window);

				std::unique_ptr<Nz::Window> window;
			};

			std::vector<WindowInfo> m_windows;
			#endif

			std::map<Nz::String, Nz::String> m_parameters;
			std::set<Nz::String> m_options;
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

#include <NazaraSDK/Application.inl>

#endif // NDK_APPLICATION_HPP
