#include <ShaderGraph.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <DataModels/Cast.hpp>
#include <DataModels/FragmentOutput.hpp>
#include <DataModels/InputValue.hpp>
#include <DataModels/SampleTexture.hpp>
#include <DataModels/ShaderNode.hpp>
#include <DataModels/VecBinOp.hpp>
#include <DataModels/VecValue.hpp>
#include <Previews/QuadPreview.hpp>
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
	void RegisterShaderNode(ShaderGraph& graph, std::shared_ptr<QtNodes::DataModelRegistry> registry, QString category = QString())
	{
		auto creator = [&] { return std::make_unique<T>(graph); };
		registry->registerModel<T>(category, std::move(creator));
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

	m_previewModel = std::make_unique<QuadPreview>();
}

ShaderGraph::~ShaderGraph()
{
	m_flowScene.clearScene();
}

std::size_t ShaderGraph::AddInput(std::string name, InputType type, InputRole role, std::size_t roleIndex)
{
	std::size_t index = m_inputs.size();
	auto& inputEntry = m_inputs.emplace_back();
	inputEntry.name = std::move(name);
	inputEntry.role = role;
	inputEntry.roleIndex = roleIndex;
	inputEntry.type = type;

	OnInputListUpdate(this);

	return index;
}

std::size_t ShaderGraph::AddTexture(std::string name, TextureType type)
{
	std::size_t index = m_textures.size();
	auto& textureEntry = m_textures.emplace_back();
	textureEntry.name = std::move(name);
	textureEntry.type = type;

	OnTextureListUpdate(this);

	return index;
}

Nz::ShaderAst::StatementPtr ShaderGraph::ToAst()
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

void ShaderGraph::UpdateInput(std::size_t inputIndex, std::string name, InputType type, InputRole role, std::size_t roleIndex)
{
	assert(inputIndex < m_inputs.size());
	auto& inputEntry = m_inputs[inputIndex];
	inputEntry.name = std::move(name);
	inputEntry.role = role;
	inputEntry.roleIndex = roleIndex;
	inputEntry.type = type;

	OnInputUpdate(this, inputIndex);
}

void ShaderGraph::UpdateTexturePreview(std::size_t textureIndex, QImage preview)
{
	assert(textureIndex < m_textures.size());
	auto& textureEntry = m_textures[textureIndex];
	textureEntry.preview = std::move(preview);
	textureEntry.preview.convertTo(QImage::Format_RGBA8888);

	OnTexturePreviewUpdate(this, textureIndex);
}

std::shared_ptr<QtNodes::DataModelRegistry> ShaderGraph::BuildRegistry()
{
	auto registry = std::make_shared<QtNodes::DataModelRegistry>();
	RegisterShaderNode<CastVec2ToVec3>(*this, registry, "Casts");
	RegisterShaderNode<CastVec2ToVec4>(*this, registry, "Casts");
	RegisterShaderNode<CastVec3ToVec2>(*this, registry, "Casts");
	RegisterShaderNode<CastVec3ToVec4>(*this, registry, "Casts");
	RegisterShaderNode<CastVec4ToVec2>(*this, registry, "Casts");
	RegisterShaderNode<CastVec4ToVec3>(*this, registry, "Casts");
	RegisterShaderNode<FragmentOutput>(*this, registry, "Outputs");
	RegisterShaderNode<InputValue>(*this, registry, "Inputs");
	RegisterShaderNode<SampleTexture>(*this, registry, "Texture");
	RegisterShaderNode<Vec2Add>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec2Mul>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec2Sub>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec3Add>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec3Mul>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec3Sub>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec4Add>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec4Mul>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec4Sub>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec2Value>(*this, registry, "Constants");
	RegisterShaderNode<Vec3Value>(*this, registry, "Constants");
	RegisterShaderNode<Vec4Value>(*this, registry, "Constants");

	return registry;
}
