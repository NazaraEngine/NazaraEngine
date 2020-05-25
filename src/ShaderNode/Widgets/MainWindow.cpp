#include <ShaderNode/Widgets/MainWindow.hpp>
#include <Nazara/Renderer/GlslWriter.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/Widgets/InputEditor.hpp>
#include <ShaderNode/Widgets/TextureEditor.hpp>
#include <nodes/FlowView>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTextEdit>
#include <iostream>

MainWindow::MainWindow(ShaderGraph& graph) :
m_shaderGraph(graph)
{
	setWindowTitle("Nazara Shader nodes");

	QtNodes::FlowScene* scene = &m_shaderGraph.GetScene();

	QtNodes::FlowView* flowView = new QtNodes::FlowView(scene);
	setCentralWidget(flowView);

	QDockWidget* inputDock = new QDockWidget(tr("&Inputs"));

	InputEditor* inputEditor = new InputEditor(m_shaderGraph);
	inputDock->setWidget(inputEditor);

	addDockWidget(Qt::LeftDockWidgetArea, inputDock);

	QDockWidget* textureDock = new QDockWidget(tr("&Textures"));

	TextureEditor* textureEditor = new TextureEditor(m_shaderGraph);
	textureDock->setWidget(textureEditor);

	addDockWidget(Qt::LeftDockWidgetArea, textureDock);

	BuildMenu();
}

void MainWindow::BuildMenu()
{
	QMenu* compileMenu = menuBar()->addMenu(tr("&Compilation"));
	QAction* compileToGlsl = compileMenu->addAction(tr("GLSL"));
	connect(compileToGlsl, &QAction::triggered, [&](bool) { OnCompileToGLSL(); });
}

void MainWindow::OnCompileToGLSL()
{
	Nz::GlslWriter writer;
	Nz::String glsl = writer.Generate(m_shaderGraph.ToAst());

	std::cout << glsl << std::endl;

	QTextEdit* output = new QTextEdit;
	output->setReadOnly(true);
	output->setText(QString::fromUtf8(glsl.GetConstBuffer(), int(glsl.GetSize())));
	output->setAttribute(Qt::WA_DeleteOnClose, true);
	output->setWindowTitle("GLSL Output");
	output->show();
}
