// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_APPLICATION_HPP
#define NDK_APPLICATION_HPP

#include <NDK/Prerequisites.hpp>
#include <NDK/World.hpp>
#include <Nazara/Core/Clock.hpp>
#include <map>
#include <list>
#include <set>

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

			inline const std::set<Nz::String>& GetOptions() const;
			inline const std::map<Nz::String, Nz::String>& GetParameters() const;

			inline float GetUpdateTime() const;

			inline bool HasOption(const Nz::String& option) const;
			inline bool HasParameter(const Nz::String& key, Nz::String* value) const;

			bool Run();

			inline void Quit();

			Application& operator=(const Application&) = delete;
			Application& operator=(Application&&) = delete;

			inline static Application* Instance();

		protected:
			void ClearWorlds();
			void ParseCommandline(int argc, char* argv[]);

		private:
			std::map<Nz::String, Nz::String> m_parameters;
			std::set<Nz::String> m_options;
			std::list<World> m_worlds;
			Nz::Clock m_updateClock;

			bool m_shouldQuit;
			float m_updateTime;

			static Application* s_application;
	};
}

#include <NDK/Application.inl>

#endif // NDK_APPLICATION_HPP
