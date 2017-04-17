// Sources pour https://github.com/DigitalPulseSoftware/NazaraEngine/wiki/(FR)-Tutoriel:-%5B02%5D-Gestion-des-événements

#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Application.hpp>
#include <NDK/Components.hpp>
#include <NDK/Systems.hpp>
#include <NDK/World.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	Ndk::Application application(argc, argv);

	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
	mainWindow.Create(Nz::VideoMode(800, 600, 32), "Test");

	mainWindow.EnableCloseOnQuit(false);

	Ndk::World& world = application.AddWorld();
	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
	world.GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(Nz::Color(117, 122, 214)));

	Ndk::EntityHandle viewEntity = world.CreateEntity();
	viewEntity->AddComponent<Ndk::NodeComponent>();

	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(&mainWindow);
	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);


	Nz::EventHandler& eventHandler = mainWindow.GetEventHandler();
	eventHandler.OnKeyPressed.Connect([](const Nz::EventHandler*, const Nz::WindowEvent::KeyEvent& e)
	{
		std::cout << Nz::Keyboard::GetKeyName(e.code) << std::endl;

		// Profitons-en aussi pour nous donner un moyen de quitter le programme
		if (e.code == Nz::Keyboard::Escape)
			Ndk::Application::Instance()->Quit(); // Cette ligne casse la boucle Run() de l'application
	});


	while (application.Run())
	{
		mainWindow.Display();
	}

	return EXIT_SUCCESS;
}
