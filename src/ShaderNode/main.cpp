#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Renderer/ShaderAst.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <Nazara/Renderer/GlslWriter.hpp>
#include <DataModels/FragmentOutput.hpp>
#include <DataModels/ShaderNode.hpp>
#include <DataModels/VecValue.hpp>
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
#include <iostream>

std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels()
{
	auto ret = std::make_shared<QtNodes::DataModelRegistry>();
	ret->registerModel<FragmentOutput>();
	ret->registerModel<Vec4Value>();

	return ret;
}

void GenerateGLSL(QtNodes::FlowScene* scene)
{
	/*using namespace ShaderBuilder;
	using ShaderAst::BuiltinEntry;
	using ShaderAst::ExpressionType;

	// Fragment shader
	{
		auto rt0 = Output("RenderTarget0", ExpressionType::Float4);
		auto color = Uniform("Color", ExpressionType::Float4);

		fragmentShader = writer.Generate(ExprStatement(Assign(rt0, color)));
	}*/

	Nz::GlslWriter writer;
	std::vector<Nz::ShaderAst::StatementPtr> statements;

	std::function<Nz::ShaderAst::ExpressionPtr(QtNodes::Node*)> HandleNode;
	HandleNode = [&](QtNodes::Node* node) -> Nz::ShaderAst::ExpressionPtr
	{
		ShaderNode* shaderNode = static_cast<ShaderNode*>(node->nodeDataModel());

		qDebug() << shaderNode->name();
		std::size_t inputCount = shaderNode->nPorts(QtNodes::PortType::In);
		Nz::StackArray<Nz::ShaderAst::ExpressionPtr> expressions = NazaraStackArray(Nz::ShaderAst::ExpressionPtr, inputCount);
		std::size_t i = 0;

		for (const auto& connectionSet : node->nodeState().getEntries(QtNodes::PortType::In))
		{
			for (const auto& [uuid, conn] : connectionSet)
			{
				assert(i < expressions.size());
				expressions[i] = HandleNode(conn->getNode(QtNodes::PortType::Out));
				i++;
			}
		}

		return shaderNode->GetExpression(expressions.data(), expressions.size());
	};

	scene->iterateOverNodes([&](QtNodes::Node* node)
	{
		if (node->nodeDataModel()->nPorts(QtNodes::PortType::Out) == 0)
		{
			statements.emplace_back(Nz::ShaderBuilder::ExprStatement(HandleNode(node)));
			//qDebug() << node->nodeDataModel()->name();
		}
	});

	Nz::String glsl = writer.Generate(std::make_shared<Nz::ShaderAst::StatementBlock>(std::move(statements)));

	QTextEdit* output = new QTextEdit;
	output->setText(QString::fromUtf8(glsl.GetConstBuffer(), glsl.GetSize()));
	output->setAttribute(Qt::WA_DeleteOnClose, true);
	output->setWindowTitle("GLSL Output");
	output->show();

	std::cout << glsl << std::endl;
}

void SetupTestScene(QtNodes::FlowScene* scene)
{
	auto& node1 = scene->createNode(std::make_unique<Vec4Value>());
	node1.nodeGraphicsObject().setPos(200, 200);

	auto& node2 = scene->createNode(std::make_unique<FragmentOutput>());
	node2.nodeGraphicsObject().setPos(500, 300);

	scene->createConnection(node2, 0, node1, 0);
}

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QWidget mainWindow;
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	QtNodes::FlowScene* scene = new QtNodes::FlowScene(registerDataModels());
	SetupTestScene(scene);

	QMenuBar* menuBar = new QMenuBar;
	QAction* glslCode = menuBar->addAction("To GLSL");
	QObject::connect(glslCode, &QAction::triggered, [&](bool) { GenerateGLSL(scene); });

	layout->addWidget(new QtNodes::FlowView(scene));
	layout->addWidget(menuBar);

	mainWindow.setLayout(layout);
	mainWindow.setWindowTitle("Nazara Shader nodes");
	mainWindow.resize(1280, 720);
	mainWindow.show();

	return app.exec();
}
