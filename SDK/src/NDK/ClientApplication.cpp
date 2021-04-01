// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/ClientApplication.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/ForwardRenderTechnique.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <regex>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::ClientApplication
	* \brief NDK class that represents a client-side application, it offers a set of tools to ease the development
	*/

	/*!
	* \brief Constructs an ClientApplication object with command-line arguments
	*
	* Pass the argc and argv arguments from the main function.
	*
	* Command-line arguments can be retrieved by application methods
	*
	* This calls Sdk::Initialize()
	*
	* \remark Only one Application instance can exist at a time
	*/
	ClientApplication::ClientApplication(int argc, char* argv[]) :
	ClientApplication()
	{
		ParseCommandline(argc, argv);

		if (HasOption("console"))
			EnableConsole(true);

		if (HasOption("fpscounter"))
			EnableFPSCounter(true);
	}

	/*!
	* \brief Runs the application by updating worlds, taking care about windows, ...
	*/
	bool ClientApplication::Run()
	{
		if (!Application::Run())
			return false;

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

		if (m_exitOnClosedWindows && !hasAtLeastOneActiveWindow)
			return false;

		for (WindowInfo& info : m_windows)
		{
			if (!info.overlayWorld)
				continue;

			if (info.fpsCounter)
			{
				FPSCounterOverlay& fpsCounter = *info.fpsCounter;

				fpsCounter.frameCount++;

				fpsCounter.elapsedTime += GetUpdateTime();
				if (fpsCounter.elapsedTime >= 1.f)
				{
					fpsCounter.sprite->Update(Nz::SimpleTextDrawer::Draw("FPS: " + Nz::String::Number(fpsCounter.frameCount), 36));
					fpsCounter.frameCount = 0;
					fpsCounter.elapsedTime = 0.f;
				}
			}

			info.overlayWorld->Update(GetUpdateTime());
		}

		return true;
	}

	void ClientApplication::SetupConsole(WindowInfo& info)
	{
		std::unique_ptr<ConsoleOverlay> overlay = std::make_unique<ConsoleOverlay>();

		Nz::Vector2ui windowDimensions;
		if (info.window->IsValid())
			windowDimensions = info.window->GetSize();
		else
			windowDimensions.MakeZero();

		Nz::LuaInstance& lua = overlay->lua;

		overlay->console = info.canvas->Add<Console>();
		overlay->console->OnCommand.Connect([&lua](Ndk::Console* console, const Nz::String& command)
		{
			if (!lua.Execute(command))
				console->AddLine(lua.GetLastError(), Nz::Color::Red);
		});

		Console& consoleRef = *overlay->console;
		consoleRef.Resize({float(windowDimensions.x), windowDimensions.y / 4.f});
		consoleRef.Show(false);

		// Redirect logs toward the console
		overlay->logSlot.Connect(Nz::Log::OnLogWrite, [&consoleRef] (const Nz::String& str)
		{
			consoleRef.AddLine(str);
		});

		lua.LoadLibraries();

		// Override "print" function to add a line in the console
		lua.PushFunction([&consoleRef] (Nz::LuaState& state)
		{
			Nz::StringStream stream;

			unsigned int argCount = state.GetStackTop();
			state.GetGlobal("tostring");
			for (unsigned int i = 1; i <= argCount; ++i)
			{
				state.PushValue(-1); // tostring function
				state.PushValue(i);  // argument
				state.Call(1, 1);

				std::size_t length;
				const char* str = state.CheckString(-1, &length);
				if (i > 1)
					stream << '\t';

				stream << Nz::String(str, length);
				state.Pop(1);
			}

			consoleRef.AddLine(stream);
			return 0;
		});
		lua.SetGlobal("print");

		// Setup a few event callback to handle the console
		Nz::EventHandler& eventHandler = info.window->GetEventHandler();

		overlay->keyPressedSlot.Connect(eventHandler.OnKeyPressed, [&consoleRef] (const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& event)
		{
			if (event.virtualKey == Nz::Keyboard::VKey::F9)
			{
				// Toggle console visibility and focus
				if (consoleRef.IsVisible())
				{
					consoleRef.ClearFocus();
					consoleRef.Show(false);
				}
				else
				{
					consoleRef.Show(true);
					consoleRef.SetFocus();
				}
			}
		});

		overlay->resizedSlot.Connect(info.renderTarget->OnRenderTargetSizeChange, [&consoleRef] (const Nz::RenderTarget* renderTarget)
		{
			Nz::Vector2ui size = renderTarget->GetSize();
			consoleRef.Resize({float(size.x), size.y / 4.f});
		});

		info.console = std::move(overlay);
	}

	void ClientApplication::SetupFPSCounter(WindowInfo& info)
	{
		std::unique_ptr<FPSCounterOverlay> fpsCounter = std::make_unique<FPSCounterOverlay>();
		fpsCounter->sprite = Nz::TextSprite::New();

		fpsCounter->entity = info.overlayWorld->CreateEntity();
		fpsCounter->entity->AddComponent<NodeComponent>();
		fpsCounter->entity->AddComponent<GraphicsComponent>().Attach(fpsCounter->sprite);

		info.fpsCounter = std::move(fpsCounter);
	}

	void ClientApplication::SetupOverlay(WindowInfo& info)
	{
		info.overlayWorld = std::make_unique<World>();

		if (info.window->IsValid())
			info.canvas = std::make_unique<Canvas>(info.overlayWorld->CreateHandle(), info.window->GetEventHandler(), info.window->GetCursorController().CreateHandle());

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

	ClientApplication* ClientApplication::s_clientApplication = nullptr;
}
