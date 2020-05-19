#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Renderer/GlslWriter.hpp>
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include <nodes/Node>
#include <nodes/NodeData>
#include <nodes/NodeGeometry>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>
#include <ShaderGraph.hpp>
#include <iostream>

void GenerateGLSL(ShaderGraph& graph)
{
	Nz::GlslWriter writer;
	Nz::String glsl = writer.Generate(graph.Generate());

	std::cout << glsl << std::endl;

	QTextEdit* output = new QTextEdit;
	output->setReadOnly(true);
	output->setText(QString::fromUtf8(glsl.GetConstBuffer(), glsl.GetSize()));
	output->setAttribute(Qt::WA_DeleteOnClose, true);
	output->setWindowTitle("GLSL Output");
	output->show();
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	ShaderGraph shaderGraph;
	shaderGraph.AddTexture("TextureMachin", Nz::ShaderAst::ExpressionType::Sampler2D);

	QWidget mainWindow;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	QtNodes::FlowScene* scene = &shaderGraph.GetScene();

	QMenuBar* menuBar = new QMenuBar;
	QAction* glslCode = menuBar->addAction("To GLSL");
	QObject::connect(glslCode, &QAction::triggered, [&](bool) { GenerateGLSL(shaderGraph); });

	layout->addWidget(new QtNodes::FlowView(scene));
	layout->addWidget(menuBar);

	mainWindow.setLayout(layout);
	mainWindow.setWindowTitle("Nazara Shader nodes");
	mainWindow.resize(1280, 720);
	mainWindow.show();

	return app.exec();
}
