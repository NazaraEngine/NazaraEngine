#include <Nazara/Audio.hpp>
#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Network.hpp>
#include <Nazara/Physics3D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <NazaraSDK/Application.hpp>
#include <NazaraSDK/Components.hpp>
#include <NazaraSDK/Systems.hpp>
#include <NazaraSDK/StateMachine.hpp>
#include "LogoDemo.hpp"
#include "SpacebattleDemo.hpp"
#include <iostream>

int main()
{
	Ndk::Application app;

	// Mix all sounds in mono (in order to give them 3D position)
	Nz::SoundBufferParams soundParams;
	soundParams.forceMono = true;

	Nz::SoundBufferManager::SetDefaultParameters(soundParams);

	// Pour commencer le mode vidéo, celui-ci va définir la taille de la zone de rendu et le nombre de bits par pixels
	Nz::VideoMode mode = Nz::VideoMode::GetDesktopMode(); // Nous récupérons le mode vidéo du bureau

	// Nous allons prendre les trois quarts de la résolution du bureau pour notre fenêtre
	mode.width = 3 * mode.width / 4;
	mode.height = 3 * mode.height / 4;

	Nz::ContextParameters targetParams;
	targetParams.antialiasingLevel = 0;

	Nz::RenderWindow& window = app.AddWindow<Nz::RenderWindow>(mode, "Nazara demo - Particles", Nz::WindowStyle_Closable, targetParams);
	//Nz::RenderWindow& window = app.AddWindow<Nz::RenderWindow>(Nz::VideoMode(1920, 1080), "Nazara demo - Particles", Nz::WindowStyle_Fullscreen, targetParams);

	app.EnableFPSCounter(true);

	Ndk::World& world3D = app.AddWorld();
	Ndk::World& world2D = app.AddWorld();

	std::random_device randomDevice;

	ExampleShared shared;
	shared.randomGen.seed(randomDevice());
	shared.target = &window;
	shared.world2D = &world2D;
	shared.world3D = &world3D;

	shared.demoName = Nz::TextSprite::New();
	shared.demoName->Update(Nz::SimpleTextDrawer::Draw("XX - DemoName", 48));

	shared.fpsCount = Nz::TextSprite::New();
	shared.fpsCount->Update(Nz::SimpleTextDrawer::Draw("XXXXX FPS", 24));

	shared.particleCount = Nz::TextSprite::New();
	shared.particleCount->Update(Nz::SimpleTextDrawer::Draw("XXXXX particles", 36));

	world2D.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
	world3D.GetSystem<Ndk::RenderSystem>().ChangeRenderTechnique<Nz::DeferredRenderTechnique>();


	Ndk::EntityHandle viewEntity = world2D.CreateEntity();
	viewEntity->AddComponent<Ndk::NodeComponent>();

	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(&window);
	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);

	shared.viewer2D = viewEntity;

	Ndk::EntityHandle cameraEntity = world3D.CreateEntity();
	cameraEntity->AddComponent<Ndk::NodeComponent>();
	cameraEntity->AddComponent<Ndk::ListenerComponent>();

	Ndk::CameraComponent& camera = cameraEntity->AddComponent<Ndk::CameraComponent>();
	camera.SetTarget(&window);
	camera.SetZFar(10000.f);

	shared.viewer3D = cameraEntity;

	Ndk::EntityHandle demoNameEntity = world2D.CreateEntity();
	Ndk::NodeComponent& demoNameNode = demoNameEntity->AddComponent<Ndk::NodeComponent>();
	Ndk::GraphicsComponent& demoNameGfx = demoNameEntity->AddComponent<Ndk::GraphicsComponent>();
	demoNameGfx.Attach(shared.demoName, 1);

	Ndk::EntityHandle fpsCountEntity = world2D.CreateEntity();
	Ndk::NodeComponent& fpsNode = fpsCountEntity->AddComponent<Ndk::NodeComponent>();
	Ndk::GraphicsComponent& fpsGfx = fpsCountEntity->AddComponent<Ndk::GraphicsComponent>();
	fpsGfx.Attach(shared.fpsCount, 1);

	Ndk::EntityHandle particleCountEntity = world2D.CreateEntity();
	Ndk::NodeComponent& particleCountNode = particleCountEntity->AddComponent<Ndk::NodeComponent>();
	Ndk::GraphicsComponent& particleCountGfx = particleCountEntity->AddComponent<Ndk::GraphicsComponent>();
	particleCountGfx.Attach(shared.particleCount, 1);


	Nz::Boxf fpsCountBox = fpsGfx.GetAABB();
	Nz::Boxf particleCountBox = particleCountGfx.GetAABB();

	Nz::Vector2ui windowSize = window.GetSize();
	demoNameNode.SetPosition(5.f, 5.f);
	particleCountNode.SetPosition(5.f, windowSize.y - particleCountBox.height - 5.f);
	fpsNode.SetPosition(5.f, windowSize.x - fpsCountBox.height - particleCountBox.height - 5.f);


	shared.demos.push_back(std::make_shared<LogoExample>(shared));
	shared.demos.push_back(std::make_shared<SpacebattleExample>(shared));

	std::size_t demoIndex = 0;
	Ndk::StateMachine stateMachine(shared.demos[demoIndex]);

	window.EnableEventPolling(true);

	while (app.Run())
	{
		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType_KeyPressed:
				{
					switch (event.key.virtualKey)
					{
						case Nz::Keyboard::VKey::Backspace:
							stateMachine.ChangeState(shared.demos[demoIndex]);
							break;

						case Nz::Keyboard::VKey::Escape:
							app.Quit();
							break;

						case Nz::Keyboard::VKey::Left:
						{
							if (shared.demos.size() <= 1)
								break;

							if (demoIndex == 0)
								demoIndex = shared.demos.size();

							demoIndex--;
							stateMachine.ChangeState(shared.demos[demoIndex]);
							break;
						}

						case Nz::Keyboard::VKey::Right:
						{
							if (shared.demos.size() <= 1)
								break;

							demoIndex++;
							if (demoIndex == shared.demos.size())
								demoIndex = 0;

							stateMachine.ChangeState(shared.demos[demoIndex]);
							break;
						}

						case Nz::Keyboard::VKey::Pause:
						{
							auto& velocitySystem = shared.world3D->GetSystem<Ndk::VelocitySystem>();
							velocitySystem.Enable(!velocitySystem.IsEnabled());
							break;
						}

						case Nz::Keyboard::VKey::F5:
						{
							Nz::Image screenshot;
							screenshot.Create(Nz::ImageType_2D, Nz::PixelFormat_RGBA8, 1920, 1080);
							window.CopyToImage(&screenshot);

							static unsigned int counter = 1;
							screenshot.SaveToFile("screenshot_" + std::to_string(counter++) + ".png");
							break;
						}

						default:
							break;
					}
					break;
				}

				case Nz::WindowEventType_Quit:
					window.Close();
					break;

				default:
					break;
			}
		}

		stateMachine.Update(app.GetUpdateTime());

		window.Display();
	}

	shared.target = nullptr;

	return EXIT_SUCCESS;
}
