#include <ShaderGraph.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <DataModels/FragmentOutput.hpp>
#include <DataModels/SampleTexture.hpp>
#include <DataModels/ShaderNode.hpp>
#include <DataModels/VecBinOp.hpp>
#include <DataModels/VecValue.hpp>
#include <QtCore/QDebug>
#include <nodes/Node>
#include <nodes/NodeData>
#include <nodes/NodeGeometry>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>

namespace
{
	template<typename T>
	void RegisterShaderNode(ShaderGraph& graph, std::shared_ptr<QtNodes::DataModelRegistry> registry)
	{
		auto creator = [&] { return std::make_unique<T>(graph); };
		registry->registerModel<T>(std::move(creator));
	}
}

ShaderGraph::ShaderGraph() :
m_flowScene(BuildRegistry())
{
	auto& node1 = m_flowScene.createNode(std::make_unique<Vec4Value>(*this));
	node1.nodeGraphicsObject().setPos(200, 200);

	auto& node2 = m_flowScene.createNode(std::make_unique<FragmentOutput>(*this));
	node2.nodeGraphicsObject().setPos(500, 300);

	m_flowScene.createConnection(node2, 0, node1, 0);
}

void ShaderGraph::AddTexture(std::string name, Nz::ShaderAst::ExpressionType type)
{
	auto& textureEntry = m_textures.emplace_back();
	textureEntry.name = std::move(name);
	textureEntry.type = type;

	OnTextureListUpdate(this);
}

Nz::ShaderAst::StatementPtr ShaderGraph::Generate()
{
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

	m_flowScene.iterateOverNodes([&](QtNodes::Node* node)
	{
		if (node->nodeDataModel()->nPorts(QtNodes::PortType::Out) == 0)
		{
			statements.emplace_back(Nz::ShaderBuilder::ExprStatement(HandleNode(node)));
		}
	});

	return std::make_shared<Nz::ShaderAst::StatementBlock>(std::move(statements));
}

std::shared_ptr<QtNodes::DataModelRegistry> ShaderGraph::BuildRegistry()
{
	auto registry = std::make_shared<QtNodes::DataModelRegistry>();
	RegisterShaderNode<FragmentOutput>(*this, registry);
	RegisterShaderNode<SampleTexture>(*this, registry);
	RegisterShaderNode<Vec4Mul>(*this, registry);
	RegisterShaderNode<Vec2Value>(*this, registry);
	RegisterShaderNode<Vec4Value>(*this, registry);

	return registry;
}
