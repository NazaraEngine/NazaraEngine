#include <QtWidgets/QApplication>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/Widgets/MainWindow.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	ShaderGraph shaderGraph;

	MainWindow mainWindow(shaderGraph);
	mainWindow.resize(1280, 720);
	mainWindow.show();

	return app.exec();
}
