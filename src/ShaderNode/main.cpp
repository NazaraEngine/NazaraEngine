#include <QtWidgets/QApplication>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/Widgets/MainWindow.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	ShaderGraph shaderGraph;
	shaderGraph.AddInput("UV", InputType::Float2, InputRole::TexCoord, 0);
	shaderGraph.AddTexture("Potato", TextureType::Sampler2D);

	MainWindow mainWindow(shaderGraph);
	mainWindow.resize(1280, 720);
	mainWindow.show();

	return app.exec();
}
