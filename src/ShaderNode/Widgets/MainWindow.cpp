#include <ShaderNode/Widgets/MainWindow.hpp>
#include <Nazara/Core/File.hpp>
#include <NZSL/GlslWriter.hpp>
#include <NZSL/Ast/AstSerializer.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/Widgets/BufferEditor.hpp>
#include <ShaderNode/Widgets/CodeOutputWidget.hpp>
#include <ShaderNode/Widgets/OptionEditor.hpp>
#include <ShaderNode/Widgets/InputEditor.hpp>
#include <ShaderNode/Widgets/OutputEditor.hpp>
#include <ShaderNode/Widgets/NodeEditor.hpp>
#include <ShaderNode/Widgets/ShaderInfoDialog.hpp>
#include <ShaderNode/Widgets/StructEditor.hpp>
#include <ShaderNode/Widgets/TextureEditor.hpp>
#include <nodes/FlowView>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>
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
	inputDock->setWidget(inputEditor);

	addDockWidget(Qt::LeftDockWidgetArea, inputDock);

	// Output editor
	OutputEditor* outputEditor = new OutputEditor(m_shaderGraph);

	QDockWidget* outputDock = new QDockWidget(tr("Outputs"));
	outputDock->setWidget(outputEditor);

	addDockWidget(Qt::LeftDockWidgetArea, outputDock);

	// Texture editor
	TextureEditor* textureEditor = new TextureEditor(m_shaderGraph);

	QDockWidget* textureDock = new QDockWidget(tr("Textures"));
	textureDock->setWidget(textureEditor);

	addDockWidget(Qt::LeftDockWidgetArea, textureDock);

	// Node editor
	m_nodeEditor = new NodeEditor;

	QDockWidget* nodeEditorDock = new QDockWidget(tr("Node editor"));
	nodeEditorDock->setWidget(m_nodeEditor);

	addDockWidget(Qt::RightDockWidgetArea, nodeEditorDock);

	// Buffer editor
	BufferEditor* bufferEditor = new BufferEditor(m_shaderGraph);

	QDockWidget* bufferDock = new QDockWidget(tr("Buffers"));
	bufferDock->setWidget(bufferEditor);

	addDockWidget(Qt::RightDockWidgetArea, bufferDock);

	// Struct editor
	StructEditor* structEditor = new StructEditor(m_shaderGraph);

	QDockWidget* structDock = new QDockWidget(tr("Structs"));
	structDock->setWidget(structEditor);

	addDockWidget(Qt::RightDockWidgetArea, structDock);

	// Option editor
	OptionEditor* optionEditor = new OptionEditor(m_shaderGraph);

	QDockWidget* optionDock = new QDockWidget(tr("Options"));
	optionDock->setWidget(optionEditor);

	addDockWidget(Qt::RightDockWidgetArea, optionDock);

	// Code output
	CodeOutputWidget* codeOutput = new CodeOutputWidget(m_shaderGraph);

	QDockWidget* codeOutputDock = new QDockWidget(tr("Code output"));
	codeOutputDock->setWidget(codeOutput);

	addDockWidget(Qt::BottomDockWidgetArea, codeOutputDock);

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
	{
		QMenu* view = menuBar()->addMenu("View");
		view->addAction(inputDock->toggleViewAction());
		view->addAction(outputDock->toggleViewAction());
		view->addAction(textureDock->toggleViewAction());
		view->addAction(nodeEditorDock->toggleViewAction());
		view->addAction(bufferDock->toggleViewAction());
		view->addAction(structDock->toggleViewAction());
		view->addAction(optionDock->toggleViewAction());
		view->addAction(codeOutputDock->toggleViewAction());
	}

	connect(scene, &QtNodes::FlowScene::connectionCreated, [=](const QtNodes::Connection& /*connection*/)
	{
		QTimer::singleShot(0, [=]
		{
			if (codeOutput->isVisible())
				codeOutput->Refresh();
		});
	});
	
	connect(scene, &QtNodes::FlowScene::connectionDeleted, [=](const QtNodes::Connection& /*connection*/)
	{
		QTimer::singleShot(0, [=]
		{
			if (codeOutput->isVisible())
				codeOutput->Refresh();
		});
	});

	m_onOptionUpdate.Connect(m_shaderGraph.OnOptionUpdate, [=](ShaderGraph*, std::size_t /*optionIndex*/)
	{
		if (codeOutput->isVisible())
			codeOutput->Refresh();
	});
}

MainWindow::~MainWindow()
{
	m_shaderGraph.Clear();
}

void MainWindow::BuildMenu()
{
	QMenuBar* menu = menuBar();

	QMenu* file = menu->addMenu(tr("&File"));
	{
		QAction* loadShader = file->addAction(tr("Load..."));
		QObject::connect(loadShader, &QAction::triggered, this, &MainWindow::OnLoad);

		QAction* saveShader = file->addAction(tr("Save..."));
		QObject::connect(saveShader, &QAction::triggered, this, &MainWindow::OnSave);
	}

	QMenu* shader = menu->addMenu(tr("&Shader"));
	{
		QAction* settings = shader->addAction(tr("Settings..."));
		QObject::connect(settings, &QAction::triggered, this, &MainWindow::OnUpdateInfo);

		QAction* compileShader = shader->addAction(tr("Compile..."));
		QObject::connect(compileShader, &QAction::triggered, this, &MainWindow::OnCompile);
	}
}

void MainWindow::OnCompile()
{
	try
	{
		auto shaderModule = m_shaderGraph.ToModule();

		QString fileName = QFileDialog::getSaveFileName(nullptr, tr("Save shader"), QString(), tr("Shader Files (*.nzslb)"));
		if (fileName.isEmpty())
			return;

		if (!fileName.endsWith("nzslb", Qt::CaseInsensitive))
			fileName += ".nzslb";

		Nz::File file(fileName.toStdString(), Nz::OpenMode::Write);
		nzsl::Serializer serializer;
		nzsl::Ast::SerializeShader(serializer, *shaderModule);

		const std::vector<std::uint8_t>& data = serializer.GetData();
		file.Write(data.data(), data.size());
	}
	catch (const std::exception& e)
	{
		QMessageBox::critical(this, tr("Compilation failed"), QString("Compilation failed: ") + e.what());
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

void MainWindow::OnUpdateInfo()
{
	ShaderInfo info;
	info.type = m_shaderGraph.GetType();

	ShaderInfoDialog* dialog = new ShaderInfoDialog(std::move(info), this);
	dialog->setAttribute(Qt::WA_DeleteOnClose, true);
	connect(dialog, &QDialog::accepted, [this, dialog]
	{
		ShaderInfo shaderInfo = dialog->GetShaderInfo();
		m_shaderGraph.UpdateType(shaderInfo.type);
	});

	dialog->open();
}
