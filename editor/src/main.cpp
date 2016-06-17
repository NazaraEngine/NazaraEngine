#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Ndk/Application.hpp>
#include <Ndk/Components.hpp>
#include <Ndk/Systems.hpp>
#include <NdkQt/QtCanvas.hpp>
#include <Editor/EditorWindow.hpp>
#include <iostream>
#include <QApplication>
#include <QtGui>
#include <QTimer>
#include <QFrame>

int main(int argc, char *argv[])
{
	Ndk::Application ndkApp;
	ndkApp.MakeExitOnLastWindowClosed(false);

	QApplication App(argc, argv);


	EditorWindow mainWindow;
	mainWindow.setWindowTitle("Nazara Model Importer");
	mainWindow.resize(400, 400);
	mainWindow.show();

	Ndk::QtCanvas* NazaraView = new Ndk::QtCanvas(&mainWindow);
	NazaraView->move(0, 40);
	NazaraView->resize(400, 360);
	NazaraView->show();

	Ndk::World& world = ndkApp.AddWorld();

	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
	world.GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(Nz::Color(192, 100, 100)));


	Ndk::EntityHandle viewEntity = world.CreateEntity();
	viewEntity->AddComponent<Ndk::NodeComponent>();

	Ndk::CameraComponent& viewer = viewEntity->AddComponent<Ndk::CameraComponent>();
	viewer.SetTarget(NazaraView);
	viewer.SetProjectionType(Nz::ProjectionType_Orthogonal);


	Nz::TextSpriteRef textSprite = Nz::TextSprite::New();
	textSprite->Update(Nz::SimpleTextDrawer::Draw("Hello world !", 72));

	Ndk::EntityHandle text = world.CreateEntity();
	Ndk::NodeComponent& nodeComponent = text->AddComponent<Ndk::NodeComponent>();

	Ndk::GraphicsComponent& graphicsComponent = text->AddComponent<Ndk::GraphicsComponent>();
	graphicsComponent.Attach(textSprite);

	Nz::Boxf textBox = graphicsComponent.GetBoundingVolume().aabb;
	nodeComponent.SetPosition(NazaraView->GetWidth() / 2 - textBox.width / 2, NazaraView->GetHeight() / 2 - textBox.height / 2);

	NazaraView->SetEventListener(true);

	QTimer worldUpdate;
	worldUpdate.setInterval(0);
	worldUpdate.connect(&worldUpdate, &QTimer::timeout, [&] () 
	{ 
		if (ndkApp.Run())
		{
			Nz::WindowEvent event;
			while (NazaraView->PollEvent(&event))
			{
				switch (event.type)
				{
					case Nz::WindowEventType_MouseMoved:
						std::cout << event.mouseMove.x << ", " << event.mouseMove.y << std::endl;
						break;

					case Nz::WindowEventType_KeyPressed:
						std::cout << event.key.code << std::endl;
						break;
				}
			}

			NazaraView->Display();
		}
		else
			QApplication::quit();
	});
	worldUpdate.start();

	return App.exec();
}
