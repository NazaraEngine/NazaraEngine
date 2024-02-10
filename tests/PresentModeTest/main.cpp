#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/TextRenderer.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	Nz::Renderer::Config rendererConfig;
#ifndef NAZARA_PLATFORM_WEB
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() == 'y')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;
#endif

	Nz::Application<Nz::Graphics> app(rendererConfig);

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();

	auto& world = ecs.AddWorld<Nz::EnttWorld>();
	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();

	std::string windowTitle = "Physics 2D";
	Nz::Window& window = windowing.CreateWindow(Nz::VideoMode(1920, 1080, 32), windowTitle);
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(window);
	Nz::Swapchain* swapchain = windowSwapchain.GetSwapchain();

	Nz::Vector2ui windowSize = window.GetSize();

	entt::handle viewer = world.CreateEntity();
	{
		viewer.emplace<Nz::NodeComponent>();
		viewer.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain), Nz::ProjectionType::Orthographic);
	}

	// Turn present mode flags into vector for easier processing
	Nz::FixedVector<Nz::PresentMode, Nz::EnumValueCount_v<Nz::PresentMode>> supportedPresentModes;
	for (Nz::PresentMode presentMode : swapchain->GetSupportedPresentModes())
		supportedPresentModes.push_back(presentMode);

	auto presentFlagIt = std::find(supportedPresentModes.begin(), supportedPresentModes.end(), swapchain->GetPresentMode());

	bool limitFps = false;

	std::shared_ptr<Nz::TextSprite> presentModeText = std::make_shared<Nz::TextSprite>();
	auto UpdatePresentModeText = [&]
	{
		Nz::RichTextDrawer textDrawer;
		textDrawer.SetTextStyle(Nz::TextStyle::Bold);
		textDrawer.AppendText("Supported present modes:\n");
		textDrawer.SetTextStyle(Nz::TextStyle_Regular);
		textDrawer.AppendText("Use +/- to switch present mode (and * to limit FPS to 50)\n");

		for (Nz::PresentMode presentMode : supportedPresentModes)
		{
			textDrawer.AppendText("- ");

			if (presentMode == swapchain->GetPresentMode())
				textDrawer.SetTextColor(Nz::Color::Yellow());
			else
				textDrawer.SetTextColor(Nz::Color::White());

			switch (presentMode)
			{
				case Nz::PresentMode::Immediate:           textDrawer.AppendText("Immediate\n"); break;
				case Nz::PresentMode::Mailbox:             textDrawer.AppendText("Mailbox\n"); break;
				case Nz::PresentMode::RelaxedVerticalSync: textDrawer.AppendText("RelaxedVerticalSync\n"); break;
				case Nz::PresentMode::VerticalSync:        textDrawer.AppendText("VerticalSync\n"); break;
			}
		}

		textDrawer.SetTextColor(Nz::Color::White());
		textDrawer.AppendText("Use * to limit FPS to 50\n");
		if (limitFps)
		{
			textDrawer.SetTextColor(Nz::Color::Red());
			textDrawer.AppendText("FPS limited to 50\n");
		}
		else
			textDrawer.AppendText("Unlimited FPS\n");

		presentModeText->Update(textDrawer);
	};

	UpdatePresentModeText();

	entt::handle textEntity = world.CreateEntity();
	{
		textEntity.emplace<Nz::NodeComponent>();
		textEntity.emplace<Nz::GraphicsComponent>(presentModeText);
	}

	entt::handle spriteEntity = world.CreateEntity();
	{
		std::shared_ptr<Nz::Sprite> sprite = std::make_shared<Nz::Sprite>(Nz::MaterialInstance::GetDefault(Nz::MaterialType::Basic));
		sprite->SetCornerColor(Nz::RectCorner::LeftTop, Nz::Color::Red());
		sprite->SetCornerColor(Nz::RectCorner::RightTop, Nz::Color::Red());
		sprite->SetCornerColor(Nz::RectCorner::LeftBottom, Nz::Color::Green());
		sprite->SetCornerColor(Nz::RectCorner::RightBottom, Nz::Color::Green());
		sprite->SetOrigin({ 0.5f, 0.5f });
		sprite->SetSize({ 128.f, float(windowSize.y) });

		spriteEntity.emplace<Nz::NodeComponent>();
		spriteEntity.emplace<Nz::GraphicsComponent>(sprite);
	}

	Nz::WindowEventHandler& eventHandler = window.GetEventHandler();
	eventHandler.OnKeyPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& event)
	{
		if (event.virtualKey == Nz::Keyboard::VKey::Add)
		{
			++presentFlagIt;
			if (presentFlagIt == supportedPresentModes.end())
				presentFlagIt = supportedPresentModes.begin();

			swapchain->SetPresentMode(*presentFlagIt);
			UpdatePresentModeText();
		}
		else if (event.virtualKey == Nz::Keyboard::VKey::Subtract)
		{
			if (presentFlagIt == supportedPresentModes.begin())
				presentFlagIt = supportedPresentModes.end();

			--presentFlagIt;

			swapchain->SetPresentMode(*presentFlagIt);
			UpdatePresentModeText();
		}
		else if (event.virtualKey == Nz::Keyboard::VKey::Multiply)
		{
			limitFps = !limitFps;
			UpdatePresentModeText();
		}
	});

	Nz::Time accumulatorTime = Nz::Time::Zero();
	constexpr Nz::Time timeToMove = Nz::Time::Second();

	Nz::HighPrecisionClock fpsLimitClock;
	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;
	bool forward = true;
	app.AddUpdaterFunc([&](Nz::Time elapsedTime)
	{
		// Move sprite
		if (forward)
		{
			accumulatorTime += elapsedTime;
			if (accumulatorTime >= timeToMove)
				forward = false;
		}
		else
		{
			accumulatorTime -= elapsedTime;
			if (accumulatorTime <= Nz::Time::Zero())
				forward = true;
		}

		float delta = (accumulatorTime.AsSeconds() / timeToMove.AsSeconds());

		spriteEntity.get<Nz::NodeComponent>().SetPosition(Nz::Lerp(128.f / 2.f, windowSize.x - 128.f / 2.f, delta * delta * (3.f - 2.f * delta)), windowSize.y / 2.f, 0.f);

		// Limit FPS
		if (limitFps)
		{
			Nz::Time remainingTime = Nz::Time::TickDuration(50) - fpsLimitClock.GetElapsedTime();
			if (remainingTime > Nz::Time::Zero())
				std::this_thread::sleep_for(remainingTime.AsDuration<std::chrono::milliseconds>());

			fpsLimitClock.Restart();
		}

		// FPS update
		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");
			fps = 0;
		}
	});

	return app.Run();
}
