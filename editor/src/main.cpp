#include <Nazara/Core/PluginManager.hpp>
#include <Ndk/Application.hpp>
#include <NdkQt/QtCanvas.hpp>
#include <Editor/EditorWindow.hpp>
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	Ndk::Application ndkApp;
	Nz::PluginManager::Mount(Nz::Plugin_Assimp);

	QApplication App(argc, argv);

	EditorWindow mainWindow;
	mainWindow.setWindowTitle("Nazara Model Importer");
	mainWindow.resize(1280, 720);
	mainWindow.show();

	return App.exec();
}
