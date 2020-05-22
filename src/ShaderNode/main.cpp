#include <QtWidgets/QApplication>
#include <ShaderGraph.hpp>
#include <Widgets/MainWindow.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	ShaderGraph shaderGraph;
	shaderGraph.AddTexture("Potato", Nz::ShaderAst::ExpressionType::Sampler2D);
	shaderGraph.AddTexture("Blackbird", Nz::ShaderAst::ExpressionType::Sampler2D);

	MainWindow mainWindow(shaderGraph);
	mainWindow.resize(1280, 720);
	mainWindow.show();

	return app.exec();
}
