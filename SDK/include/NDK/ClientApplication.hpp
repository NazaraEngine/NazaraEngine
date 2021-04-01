// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_CLIENTAPPLICATION_HPP
#define NDK_CLIENTAPPLICATION_HPP

#include <NDK/ClientPrerequisites.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Lua/LuaInstance.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Platform/Window.hpp>
#include <NDK/Application.hpp>
#include <NDK/Canvas.hpp>
#include <NDK/Console.hpp>
#include <NDK/World.hpp>
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

			inline void EnableConsole(bool enable);
			inline void EnableFPSCounter(bool enable);

			inline ConsoleOverlay& GetConsoleOverlay(std::size_t windowIndex = 0U);
			inline FPSCounterOverlay& GetFPSCounterOverlay(std::size_t windowIndex = 0U);

			inline bool IsConsoleEnabled() const;
			inline bool IsFPSCounterEnabled() const;

			bool Run();

			inline void MakeExitOnLastWindowClosed(bool exitOnClosedWindows);

			ClientApplication& operator=(const ClientApplication&) = delete;
			ClientApplication& operator=(ClientApplication&&) = delete;

			inline static ClientApplication* Instance();

			struct ConsoleOverlay
			{
				Console* console;
				Nz::LuaInstance lua;

				NazaraSlot(Nz::EventHandler, OnEvent, eventSlot);
				NazaraSlot(Nz::EventHandler, OnKeyPressed, keyPressedSlot);
				NazaraSlot(Nz::RenderTarget, OnRenderTargetSizeChange, resizedSlot);
				NazaraSlot(Nz::Log, OnLogWrite, logSlot);
			};

			struct FPSCounterOverlay
			{
				Nz::TextSpriteRef sprite;
				EntityOwner entity;
				float elapsedTime = 0.f;
				unsigned int frameCount = 0;
			};

		private:
			enum OverlayFlags
			{
				OverlayFlags_Console    = 0x1,
				OverlayFlags_FPSCounter = 0x2
			};

			struct WindowInfo
			{
				inline WindowInfo(std::unique_ptr<Nz::Window>&& window);

				Nz::RenderTarget* renderTarget;
				std::unique_ptr<Nz::Window> window;
				std::unique_ptr<ConsoleOverlay> console;
				std::unique_ptr<Canvas> canvas;
				std::unique_ptr<FPSCounterOverlay> fpsCounter;
				std::unique_ptr<World> overlayWorld;
			};

			void SetupConsole(WindowInfo& info);
			void SetupFPSCounter(WindowInfo& info);
			void SetupOverlay(WindowInfo& info);

			template<typename T> void SetupWindow(WindowInfo& info, T* renderTarget, std::true_type /*isRenderTarget*/);
			template<typename T> void SetupWindow(WindowInfo& /*info*/, T* /*renderTarget*/, std::false_type /*isNotRenderTarget*/);

			std::vector<WindowInfo> m_windows;
			Nz::UInt32 m_overlayFlags;
			bool m_exitOnClosedWindows;

			static ClientApplication* s_clientApplication;
	};
}

#include <NDK/ClientApplication.inl>

#endif // NDK_CLIENTAPPLICATION_HPP
