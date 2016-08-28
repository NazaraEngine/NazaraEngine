// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Application.hpp>

#ifndef NDK_SERVER
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/LuaAPI.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#endif

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::Application
	* \brief NDK class that represents the application, it offers a set of tools to ease the development
	*/

	/*!
	* \brief Runs the application by updating worlds, taking care about windows, ...
	*/
	bool Application::Run()
	{
		#ifndef NDK_SERVER
		bool hasAtLeastOneActiveWindow = false;

		auto it = m_windows.begin();
		while (it != m_windows.end())
		{
			Nz::Window& window = *it->window;

			window.ProcessEvents();

			if (!window.IsOpen(true))
			{
				it = m_windows.erase(it);
				continue;
			}

			hasAtLeastOneActiveWindow = true;

			++it;
		}
		#endif

		#ifndef NDK_SERVER
		if (m_exitOnClosedWindows && !hasAtLeastOneActiveWindow)
			return false;
		#endif

		if (m_shouldQuit)
			return false;

		m_updateTime = m_updateClock.GetSeconds();
		m_updateClock.Restart();

		for (World& world : m_worlds)
			world.Update(m_updateTime);

		#ifndef NDK_SERVER
		for (WindowInfo& info : m_windows)
		{
			if (info.fpsCounter)
			{
				FPSCounterOverlay& fpsCounter = *info.fpsCounter;

				fpsCounter.frameCount++;

				fpsCounter.elapsedTime += m_updateTime;
				if (fpsCounter.elapsedTime >= 1.f)
				{
					fpsCounter.sprite->Update(Nz::SimpleTextDrawer::Draw("FPS: " + Nz::String::Number(fpsCounter.frameCount), 36));
					fpsCounter.frameCount = 0;
					fpsCounter.elapsedTime = 0.f;
				}
			}

			info.overlayWorld->Update(m_updateTime);
		}
		#endif

		return true;
	}

	#ifndef NDK_SERVER
	void Application::SetupConsole(WindowInfo& info)
	{
		std::unique_ptr<ConsoleOverlay> overlay = std::make_unique<ConsoleOverlay>();

		overlay->console = std::make_unique<Console>(*info.overlayWorld, Nz::Vector2f(Nz::Vector2ui(info.window->GetWidth(), info.window->GetHeight() / 4)), overlay->lua);
		
		Console& consoleRef = *overlay->console;
		// Redirect logs toward the console
		overlay->logSlot.Connect(Nz::Log::OnLogWrite, [&consoleRef] (const Nz::String& str)
		{
			consoleRef.AddLine(str);
		});

		LuaAPI::RegisterClasses(overlay->lua);

		// Override "print" function to add a line in the console
		overlay->lua.PushFunction([&consoleRef] (Nz::LuaInstance& instance)
		{
			Nz::StringStream stream;

			unsigned int argCount = instance.GetStackTop();
			instance.GetGlobal("tostring");
			for (unsigned int i = 1; i <= argCount; ++i)
			{
				instance.PushValue(-1); // tostring function
				instance.PushValue(i);  // argument
				instance.Call(1, 1);

				std::size_t length;
				const char* str = instance.CheckString(-1, &length);
				if (i > 1)
					stream << '\t';

				stream << Nz::String(str, length);
				instance.Pop(1);
			}

			consoleRef.AddLine(stream);
			return 0;
		});
		overlay->lua.SetGlobal("print");

		// Define a few base variables to allow our interface to interact with the application
		overlay->lua.PushGlobal("Application", Ndk::Application::Instance());
		overlay->lua.PushGlobal("Console", consoleRef.CreateHandle());

		// Setup a few event callback to handle the console
		Nz::EventHandler& eventHandler = info.window->GetEventHandler();

		overlay->eventSlot.Connect(eventHandler.OnEvent, [&consoleRef] (const Nz::EventHandler*, const Nz::WindowEvent& event)
		{
			if (consoleRef.IsVisible())
				consoleRef.SendEvent(event);
		});

		overlay->keyPressedSlot.Connect(eventHandler.OnKeyPressed, [&consoleRef] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& event)
		{
			if (event.code == Nz::Keyboard::F9)
				consoleRef.Show(!consoleRef.IsVisible());
		});

		overlay->resizedSlot.Connect(eventHandler.OnResized, [&consoleRef] (const Nz::EventHandler*, const Nz::WindowEvent::SizeEvent& event)
		{
			consoleRef.SetSize({float(event.width), event.height / 4.f});
		});

		info.console = std::move(overlay);
	}

	void Application::SetupFPSCounter(WindowInfo& info)
	{
		std::unique_ptr<FPSCounterOverlay> fpsCounter = std::make_unique<FPSCounterOverlay>();
		fpsCounter->sprite = Nz::TextSprite::New();

		fpsCounter->entity = info.overlayWorld->CreateEntity();
		fpsCounter->entity->AddComponent<NodeComponent>();
		fpsCounter->entity->AddComponent<GraphicsComponent>().Attach(fpsCounter->sprite);

		info.fpsCounter = std::move(fpsCounter);
	}

	void Application::SetupOverlay(WindowInfo& info)
	{
		info.overlayWorld = std::make_unique<World>(false); //< No default system

		RenderSystem& renderSystem = info.overlayWorld->AddSystem<RenderSystem>();
		renderSystem.ChangeRenderTechnique<Nz::ForwardRenderTechnique>();
		renderSystem.SetDefaultBackground(nullptr);
		renderSystem.SetGlobalUp(Nz::Vector3f::Down());

		EntityHandle viewer = info.overlayWorld->CreateEntity();
		CameraComponent& camComponent = viewer->AddComponent<CameraComponent>();
		viewer->AddComponent<NodeComponent>();

		camComponent.SetProjectionType(Nz::ProjectionType_Orthogonal);
		camComponent.SetTarget(info.renderTarget);
	}
	#endif

	Application* Application::s_application = nullptr;
}