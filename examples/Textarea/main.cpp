// Sources pour https://github.com/DigitalPulseSoftware/NazaraEngine/wiki/(FR)-Tutoriel:-%5B01%5D-Hello-World

#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Application.hpp>
#include <NDK/Components.hpp>
#include <NDK/Systems.hpp>
#include <NDK/World.hpp>
#include <NDK/Canvas.hpp>
#include <NDK/Widgets/TextAreaWidget.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	Ndk::Application application(argc, argv);

	Nz::RenderWindow& mainWindow = application.AddWindow<Nz::RenderWindow>();
	mainWindow.Create(Nz::VideoMode(800, 600, 32), "Test");


	Ndk::World& world = application.AddWorld();
	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
	world.GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(Nz::Color(117, 122, 214)));


	Ndk::EntityHandle viewEntity = world.CreateEntity();
	viewEntity->AddComponent<Ndk::NodeComponent>();

	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(&mainWindow);
	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);

	Ndk::EntityHandle text = world.CreateEntity();
	Ndk::NodeComponent& nodeComponent = text->AddComponent<Ndk::NodeComponent>();

    Ndk::Canvas canvas(world.CreateHandle(), mainWindow.GetEventHandler(), mainWindow.GetCursorController().CreateHandle());
	canvas.SetFixedSize(Nz::Vector2f(mainWindow.GetSize()));

    auto textarea = canvas.Add<Ndk::TextAreaWidget>();
    textarea->EnableBackground(true);
    textarea->SetBackgroundColor(Nz::Color(0, 0, 0, 150));
    textarea->SetTextColor(Nz::Color::White);
    textarea->EnableMultiline();

    textarea->SetFixedSize(canvas.GetSize()/2);

	/*Nz::Boxf textBox = mainWindow.GetSize();
	Nz::Vector2ui windowSize = mainWindow.GetSize();
	nodeComponent.SetPosition(windowSize.x / 2 - textBox.width / 2, windowSize.y / 2 - textBox.height / 2);*/

	while (application.Run())
	{
		mainWindow.Display();
	}

	return EXIT_SUCCESS;
}
