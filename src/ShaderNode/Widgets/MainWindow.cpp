#include <ShaderNode/Widgets/MainWindow.hpp>
#include <Nazara/Renderer/GlslWriter.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/Widgets/InputEditor.hpp>
#include <ShaderNode/Widgets/OutputEditor.hpp>
#include <ShaderNode/Widgets/NodeEditor.hpp>
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

	// Input editor
	InputEditor* inputEditor = new InputEditor(m_shaderGraph);

	QDockWidget* inputDock = new QDockWidget(tr("Inputs"));
	inputDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	inputDock->setWidget(inputEditor);

	addDockWidget(Qt::LeftDockWidgetArea, inputDock);

	// Output editor
	OutputEditor* outputEditor = new OutputEditor(m_shaderGraph);

	QDockWidget* outputDock = new QDockWidget(tr("Outputs"));
	outputDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	outputDock->setWidget(outputEditor);

	addDockWidget(Qt::LeftDockWidgetArea, outputDock);

	// Texture editor
	TextureEditor* textureEditor = new TextureEditor(m_shaderGraph);

	QDockWidget* textureDock = new QDockWidget(tr("Textures"));
	textureDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	textureDock->setWidget(textureEditor);

	addDockWidget(Qt::LeftDockWidgetArea, textureDock);

	// Node editor
	m_nodeEditor = new NodeEditor;

	QDockWidget* nodeEditorDock = new QDockWidget(tr("Node editor"));
	nodeEditorDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	nodeEditorDock->setWidget(m_nodeEditor);

	addDockWidget(Qt::RightDockWidgetArea, nodeEditorDock);

	m_onSelectedNodeUpdate.Connect(m_shaderGraph.OnSelectedNodeUpdate, [&](ShaderGraph*, ShaderNode* node)
	{
		if (node)
		{
			m_nodeEditor->UpdateContent(node->caption(), [node](QFormLayout* layout)
			{
				node->BuildNodeEdition(layout);
			});
		}
		else
			m_nodeEditor->Clear();
	});


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
