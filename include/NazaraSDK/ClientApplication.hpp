// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_CLIENTAPPLICATION_HPP
#define NDK_CLIENTAPPLICATION_HPP

#include <NazaraSDK/ClientPrerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Platform/Window.hpp>
#include <NazaraSDK/Application.hpp>
#include <NazaraSDK/World.hpp>
#include <map>
#include <list>
#include <set>

namespace Ndk
{
	class NDK_CLIENT_API ClientApplication : public Application
	{
		public:
			struct ConsoleOverlay;
			struct FPSCounterOverlay;

			inline ClientApplication();
			ClientApplication(int argc, char* argv[]);
			ClientApplication(const ClientApplication&) = delete;
			ClientApplication(ClientApplication&&) = delete;
			inline ~ClientApplication();

			template<typename T, typename... Args> T& AddWindow(Args&&... args);

			bool Run();

			inline void MakeExitOnLastWindowClosed(bool exitOnClosedWindows);

			ClientApplication& operator=(const ClientApplication&) = delete;
			ClientApplication& operator=(ClientApplication&&) = delete;

			inline static ClientApplication* Instance();

		private:
			enum OverlayFlags
			{
				OverlayFlags_Console    = 0x1,
				OverlayFlags_FPSCounter = 0x2
			};

			struct WindowInfo
			{
				inline WindowInfo(std::unique_ptr<Nz::Window>&& window);

				std::unique_ptr<Nz::Window> window;
			};

			std::vector<WindowInfo> m_windows;
			Nz::UInt32 m_overlayFlags;
			bool m_exitOnClosedWindows;

			static ClientApplication* s_clientApplication;
	};
}

#include <NazaraSDK/ClientApplication.inl>

#endif // NDK_CLIENTAPPLICATION_HPP
