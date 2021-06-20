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
#include <string>

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

			template<typename... Args> World& AddWorld(Args&&... args);

			inline const std::set<std::string>& GetOptions() const;
			inline const std::map<std::string, std::string>& GetParameters() const;

			inline float GetUpdateTime() const;

			inline bool HasOption(const std::string& option) const;
			inline bool HasParameter(const std::string& key, std::string* value) const;

			bool Run();

			inline void Quit();

			Application& operator=(const Application&) = delete;
			Application& operator=(Application&&) = delete;

			inline static Application* Instance();

		protected:
			void ClearWorlds();
			void ParseCommandline(int argc, char* argv[]);

		private:
			std::map<std::string, std::string> m_parameters;
			std::set<std::string> m_options;
			std::list<World> m_worlds;
			Nz::Clock m_updateClock;

			bool m_shouldQuit;
			float m_updateTime;

			static Application* s_application;
	};
}

#include <NazaraSDK/Application.inl>

#endif // NDK_APPLICATION_HPP
