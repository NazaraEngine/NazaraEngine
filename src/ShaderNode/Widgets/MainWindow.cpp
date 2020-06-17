#include <ShaderNode/Widgets/MainWindow.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/GlslWriter.hpp>
#include <Nazara/Renderer/ShaderSerializer.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/Widgets/InputEditor.hpp>
#include <ShaderNode/Widgets/OutputEditor.hpp>
#include <ShaderNode/Widgets/NodeEditor.hpp>
#include <ShaderNode/Widgets/TextureEditor.hpp>
#include <nodes/FlowView>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
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
	QMenuBar* menu = menuBar();

	QMenu* shader = menu->addMenu(tr("&Shader"));
	{
		QAction* loadShader = shader->addAction(tr("Load..."));
		QObject::connect(loadShader, &QAction::triggered, this, &MainWindow::OnLoad);
		QAction* saveShader = shader->addAction(tr("Save..."));
		QObject::connect(saveShader, &QAction::triggered, this, &MainWindow::OnSave);
	}

	QMenu* compileMenu = menu->addMenu(tr("&Compilation"));
	QAction* compileToGlsl = compileMenu->addAction(tr("GLSL"));
	connect(compileToGlsl, &QAction::triggered, [&](bool) { OnCompileToGLSL(); });
}

void MainWindow::OnCompileToGLSL()
{
	try
	{
		Nz::ShaderNodes::StatementPtr shaderAst = m_shaderGraph.ToAst();

		Nz::File file("shader.shader", Nz::OpenMode_WriteOnly);
		file.Write(Nz::ShaderNodes::Serialize(shaderAst));

		//TODO: Put in another function
		auto GetExpressionFromInOut = [&] (InOutType type)
		{
			switch (type)
			{
				case InOutType::Bool:   return Nz::ShaderNodes::ExpressionType::Boolean;
				case InOutType::Float1: return Nz::ShaderNodes::ExpressionType::Float1;
				case InOutType::Float2: return Nz::ShaderNodes::ExpressionType::Float2;
				case InOutType::Float3: return Nz::ShaderNodes::ExpressionType::Float3;
				case InOutType::Float4: return Nz::ShaderNodes::ExpressionType::Float4;
			}

			assert(false);
			throw std::runtime_error("Unhandled input type");
		};

		auto GetExpressionFromTexture = [&](TextureType type)
		{
			switch (type)
			{
				case TextureType::Sampler2D: return Nz::ShaderNodes::ExpressionType::Sampler2D;
			}

			assert(false);
			throw std::runtime_error("Unhandled texture type");
		};

		Nz::ShaderAst shader;
		for (const auto& input : m_shaderGraph.GetInputs())
			shader.AddInput(input.name, GetExpressionFromInOut(input.type), input.locationIndex);

		for (const auto& output : m_shaderGraph.GetOutputs())
			shader.AddOutput(output.name, GetExpressionFromInOut(output.type), output.locationIndex);

		for (const auto& uniform : m_shaderGraph.GetTextures())
			shader.AddUniform(uniform.name, GetExpressionFromTexture(uniform.type), uniform.bindingIndex);

		shader.AddFunction("main", shaderAst);

		Nz::GlslWriter writer;
		Nz::String glsl = writer.Generate(shader);

		std::cout << glsl << std::endl;

		QTextEdit* output = new QTextEdit;
		output->setReadOnly(true);
		output->setText(QString::fromUtf8(glsl.GetConstBuffer(), int(glsl.GetSize())));
		output->setAttribute(Qt::WA_DeleteOnClose, true);
		output->setWindowTitle("GLSL Output");
		output->show();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Compilation failed"), QString("Compilation failed: ") + e.what());
	}
}

void MainWindow::OnLoad()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open shader flow"), QDir::homePath(), tr("Shader Flow Files (*.shaderflow)"));
	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, tr("Failed to open file"), QString("Failed to open shader flow file: ") + file.errorString());
		return;
	}

	QJsonObject jsonDocument = QJsonDocument::fromJson(file.readAll()).object();
	if (jsonDocument.isEmpty())
	{
		QMessageBox::critical(this, tr("Invalid file"), tr("Invalid shader flow file"));
		return;
	}

	try
	{
		m_shaderGraph.Load(jsonDocument);
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Invalid file"), tr("Invalid shader flow file: ") + e.what());
		return;
	}
}

void MainWindow::OnSave()
{
	QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Open shader flow"), QDir::homePath(), tr("Shader Flow Files (*.shaderflow)"));
	if (fileName.isEmpty())
		return;

	if (!fileName.endsWith("flow", Qt::CaseInsensitive))
		fileName += ".shaderflow";

	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly))
		file.write(QJsonDocument(m_shaderGraph.Save()).toJson());
}
