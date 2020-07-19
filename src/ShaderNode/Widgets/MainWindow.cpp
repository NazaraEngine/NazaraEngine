#include <ShaderNode/Widgets/MainWindow.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Renderer/GlslWriter.hpp>
#include <Nazara/Renderer/ShaderSerializer.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/Widgets/BufferEditor.hpp>
#include <ShaderNode/Widgets/InputEditor.hpp>
#include <ShaderNode/Widgets/OutputEditor.hpp>
#include <ShaderNode/Widgets/NodeEditor.hpp>
#include <ShaderNode/Widgets/StructEditor.hpp>
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

	// Buffer editor
	BufferEditor* bufferEditor = new BufferEditor(m_shaderGraph);

	QDockWidget* bufferDock = new QDockWidget(tr("Buffers"));
	bufferDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	bufferDock->setWidget(bufferEditor);

	addDockWidget(Qt::RightDockWidgetArea, bufferDock);

	// Struct editor
	StructEditor* structEditor = new StructEditor(m_shaderGraph);

	QDockWidget* structDock = new QDockWidget(tr("Structs"));
	structDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	structDock->setWidget(structEditor);

	addDockWidget(Qt::RightDockWidgetArea, structDock);

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
		QAction* compileShader = shader->addAction(tr("Compile..."));
		QObject::connect(compileShader, &QAction::triggered, this, &MainWindow::OnCompile);
	}

	QMenu* generateMenu = menu->addMenu(tr("&Generate"));
	QAction* generateGlsl = generateMenu->addAction(tr("GLSL"));
	connect(generateGlsl, &QAction::triggered, [&](bool) { OnGenerateGLSL(); });
}

void MainWindow::OnCompile()
{
	try
	{
		auto shader = ToShader();

		QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save shader"), QString(), tr("Shader Files (*.shader)"));
		if (fileName.isEmpty())
			return;

		if (!fileName.endsWith("shader", Qt::CaseInsensitive))
			fileName += ".shader";

		Nz::File file(fileName.toStdString(), Nz::OpenMode_WriteOnly);
		file.Write(Nz::SerializeShader(shader));
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Compilation failed"), QString("Compilation failed: ") + e.what());
	}
}

void MainWindow::OnGenerateGLSL()
{
	try
	{
		Nz::GlslWriter writer;
		std::string glsl = writer.Generate(ToShader());

		std::cout << glsl << std::endl;

		QTextEdit* output = new QTextEdit;
		output->setReadOnly(true);
		output->setText(QString::fromStdString(glsl));
		output->setAttribute(Qt::WA_DeleteOnClose, true);
		output->setWindowTitle("GLSL Output");
		output->show();
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Generation failed"), QString("Generation failed: ") + e.what());
	}
}

void MainWindow::OnLoad()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open shader flow"), QString(), tr("Shader Flow Files (*.shaderflow)"));
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
	QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Open shader flow"), QString(), tr("Shader Flow Files (*.shaderflow)"));
	if (fileName.isEmpty())
		return;

	if (!fileName.endsWith("shaderflow", Qt::CaseInsensitive))
		fileName += ".shaderflow";

	QFile file(fileName);
	if (file.open(QIODevice::WriteOnly))
		file.write(QJsonDocument(m_shaderGraph.Save()).toJson());
}

Nz::ShaderAst MainWindow::ToShader()
{
	Nz::ShaderNodes::StatementPtr shaderAst = m_shaderGraph.ToAst();

	Nz::ShaderAst shader;
	for (const auto& input : m_shaderGraph.GetInputs())
		shader.AddInput(input.name, m_shaderGraph.ToShaderExpressionType(input.type), input.locationIndex);

	for (const auto& output : m_shaderGraph.GetOutputs())
		shader.AddOutput(output.name, m_shaderGraph.ToShaderExpressionType(output.type), output.locationIndex);

	for (const auto& buffer : m_shaderGraph.GetBuffers())
	{
		const auto& structInfo = m_shaderGraph.GetStruct(buffer.structIndex);
		shader.AddUniform(buffer.name, structInfo.name, buffer.bindingIndex, Nz::ShaderNodes::MemoryLayout::Std140);
	}

	for (const auto& uniform : m_shaderGraph.GetTextures())
		shader.AddUniform(uniform.name, m_shaderGraph.ToShaderExpressionType(uniform.type), uniform.bindingIndex, {});

	for (const auto& s : m_shaderGraph.GetStructs())
	{
		std::vector<Nz::ShaderAst::StructMember> members;
		for (const auto& sMember : s.members)
		{
			auto& member = members.emplace_back();
			member.name = sMember.name;

			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, PrimitiveType>)
					member.type = m_shaderGraph.ToShaderExpressionType(arg);
				else if constexpr (std::is_same_v<T, std::size_t>)
					member.type = m_shaderGraph.GetStruct(arg).name;
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");
			}, sMember.type);
		}

		shader.AddStruct(s.name, std::move(members));
	}

	shader.AddFunction("main", shaderAst);

	return shader;
}
