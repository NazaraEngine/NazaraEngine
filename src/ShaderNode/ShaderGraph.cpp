#include <ShaderNode/ShaderGraph.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <ShaderNode/DataModels/Cast.hpp>
#include <ShaderNode/DataModels/FloatValue.hpp>
#include <ShaderNode/DataModels/InputValue.hpp>
#include <ShaderNode/DataModels/OutputValue.hpp>
#include <ShaderNode/DataModels/SampleTexture.hpp>
#include <ShaderNode/DataModels/ShaderNode.hpp>
#include <ShaderNode/DataModels/TextureValue.hpp>
#include <ShaderNode/DataModels/VecBinOp.hpp>
#include <ShaderNode/DataModels/VecDot.hpp>
#include <ShaderNode/DataModels/VecFloatMul.hpp>
#include <ShaderNode/DataModels/VecValue.hpp>
#include <ShaderNode/Previews/QuadPreview.hpp>
#include <QtCore/QDebug>
#include <nodes/Node>
#include <nodes/NodeData>
#include <nodes/NodeGeometry>
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>
#include <unordered_set>

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
	m_previewModel = std::make_unique<QuadPreview>();

	QObject::connect(&m_flowScene, &QGraphicsScene::selectionChanged, [&]
	{
		auto selectedNodes = m_flowScene.selectedNodes();
		if (selectedNodes.size() == 1)
			OnSelectedNodeUpdate(this, static_cast<ShaderNode*>(selectedNodes.front()->nodeDataModel()));
		else
			OnSelectedNodeUpdate(this, nullptr);
	});

	// Test
	AddInput("UV", InOutType::Float2, InputRole::TexCoord, 0);
	AddOutput("RenderTarget0", InOutType::Float4);
	AddTexture("Potato", TextureType::Sampler2D);

	UpdateTexturePreview(0, QImage(R"(C:\Users\Lynix\Pictures\potatavril.png)"));

	auto& node1 = m_flowScene.createNode(std::make_unique<TextureValue>(*this));
	node1.nodeGraphicsObject().setPos(0, 200);

	auto& node2 = m_flowScene.createNode(std::make_unique<InputValue>(*this));
	node2.nodeGraphicsObject().setPos(50, 350);

	auto& node3 = m_flowScene.createNode(std::make_unique<SampleTexture>(*this));
	node3.nodeGraphicsObject().setPos(200, 200);

	auto& node4 = m_flowScene.createNode(std::make_unique<VecMul>(*this));
	node4.nodeGraphicsObject().setPos(400, 200);

	auto& node5 = m_flowScene.createNode(std::make_unique<OutputValue>(*this));
	node5.nodeGraphicsObject().setPos(600, 300);

	m_flowScene.createConnection(node3, 0, node1, 0);
	m_flowScene.createConnection(node3, 1, node2, 0);
	m_flowScene.createConnection(node4, 0, node3, 0);
	m_flowScene.createConnection(node4, 1, node3, 0);
	m_flowScene.createConnection(node5, 0, node4, 0);
}

ShaderGraph::~ShaderGraph()
{
	m_flowScene.clearScene();
}

std::size_t ShaderGraph::AddInput(std::string name, InOutType type, InputRole role, std::size_t roleIndex)
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

std::size_t ShaderGraph::AddOutput(std::string name, InOutType type)
{
	std::size_t index = m_outputs.size();
	auto& outputEntry = m_outputs.emplace_back();
	outputEntry.name = std::move(name);
	outputEntry.type = type;

	OnOutputListUpdate(this);

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

void ShaderGraph::Clear()
{
	m_flowScene.clearScene();
	m_flowScene.clear();

	m_inputs.clear();
	m_outputs.clear();
	m_textures.clear();

	OnInputListUpdate(this);
	OnOutputListUpdate(this);
	OnTextureListUpdate(this);
}

void ShaderGraph::Load(const QJsonObject& data)
{
	Clear();

	QJsonArray inputArray = data["inputs"].toArray();
	for (const auto& inputDocRef : inputArray)
	{
		QJsonObject inputDoc = inputDocRef.toObject();

		InputEntry& input = m_inputs.emplace_back();
		input.name = inputDoc["name"].toString().toStdString();
		input.role = DecodeEnum<InputRole>(inputDoc["role"].toString().toStdString()).value();
		input.roleIndex = static_cast<std::size_t>(inputDoc["roleIndex"].toInt(0));
		input.type = DecodeEnum<InOutType>(inputDoc["type"].toString().toStdString()).value();
	}

	OnInputListUpdate(this);

	QJsonArray outputArray = data["outputs"].toArray();
	for (const auto& outputDocRef : outputArray)
	{
		QJsonObject outputDoc = outputDocRef.toObject();

		OutputEntry& output = m_outputs.emplace_back();
		output.name = outputDoc["name"].toString().toStdString();
		output.type = DecodeEnum<InOutType>(outputDoc["type"].toString().toStdString()).value();
	}

	OnOutputListUpdate(this);

	QJsonArray textureArray = data["textures"].toArray();
	for (const auto& textureDocRef : textureArray)
	{
		QJsonObject textureDoc = textureDocRef.toObject();

		TextureEntry& texture = m_textures.emplace_back();
		texture.name = textureDoc["name"].toString().toStdString();
		texture.type = DecodeEnum<TextureType>(textureDoc["type"].toString().toStdString()).value();
	}

	OnTextureListUpdate(this);

	for (QJsonValueRef node : data["nodes"].toArray())
		m_flowScene.restoreNode(node.toObject());

	for (QJsonValueRef connection : data["connections"].toArray())
		m_flowScene.restoreConnection(connection.toObject());
}

QJsonObject ShaderGraph::Save()
{
	QJsonObject sceneJson;

	QJsonArray inputArray;
	{
		for (const auto& input : m_inputs)
		{
			QJsonObject inputDoc;
			inputDoc["name"] = QString::fromStdString(input.name);
			inputDoc["role"] = QString(EnumToString(input.role));
			inputDoc["roleIndex"] = int(input.roleIndex);
			inputDoc["type"] = QString(EnumToString(input.type));

			inputArray.append(inputDoc);
		}
	}
	sceneJson["inputs"] = inputArray;

	QJsonArray outputArray;
	{
		for (const auto& output : m_outputs)
		{
			QJsonObject outputDoc;
			outputDoc["name"] = QString::fromStdString(output.name);
			outputDoc["type"] = QString(EnumToString(output.type));

			outputArray.append(outputDoc);
		}
	}
	sceneJson["outputs"] = outputArray;

	QJsonArray textureArray;
	{
		for (const auto& texture : m_textures)
		{
			QJsonObject textureDoc;
			textureDoc["name"] = QString::fromStdString(texture.name);
			textureDoc["type"] = QString(EnumToString(texture.type));

			textureArray.append(textureDoc);
		}
	}
	sceneJson["textures"] = textureArray;

	QJsonArray nodesJsonArray;
	{
		for (auto&& [uuid, node] : m_flowScene.nodes())
			nodesJsonArray.append(node->save());
	}
	sceneJson["nodes"] = nodesJsonArray;

	QJsonArray connectionJsonArray;
	{
		for (auto&& [uuid, connection] : m_flowScene.connections())
		{
			QJsonObject connectionJson = connection->save();

			if (!connectionJson.isEmpty())
				connectionJsonArray.append(connectionJson);
		}
	}
	sceneJson["connections"] = connectionJsonArray;

	return sceneJson;
}

Nz::ShaderAst::StatementPtr ShaderGraph::ToAst()
{
	std::vector<Nz::ShaderAst::StatementPtr> statements;
	QHash<QUuid, unsigned int> usageCount;

	std::function<void(QtNodes::Node*)> DetectVariables;
	DetectVariables = [&](QtNodes::Node* node)
	{
		auto it = usageCount.find(node->id());
		if (it == usageCount.end())
		{
			for (const auto& connectionSet : node->nodeState().getEntries(QtNodes::PortType::In))
			{
				for (const auto& [uuid, conn] : connectionSet)
				{
					DetectVariables(conn->getNode(QtNodes::PortType::Out));
				}
			}

			it = usageCount.insert(node->id(), 0);
		}

		(*it)++;
	};

	m_flowScene.iterateOverNodes([&](QtNodes::Node* node)
	{
		if (node->nodeDataModel()->nPorts(QtNodes::PortType::Out) == 0)
			DetectVariables(node);
	});

	QHash<QUuid, Nz::ShaderAst::ExpressionPtr> variableExpressions;

	unsigned int varCount = 0;
	std::unordered_set<std::string> usedVariableNames;

	std::function<Nz::ShaderAst::ExpressionPtr(QtNodes::Node*)> HandleNode;
	HandleNode = [&](QtNodes::Node* node) -> Nz::ShaderAst::ExpressionPtr
	{
		ShaderNode* shaderNode = static_cast<ShaderNode*>(node->nodeDataModel());
		if (shaderNode->validationState() != QtNodes::NodeValidationState::Valid)
			throw std::runtime_error(shaderNode->validationMessage().toStdString());

		qDebug() << shaderNode->name() << node->id();
		if (auto it = variableExpressions.find(node->id()); it != variableExpressions.end())
			return *it;

		auto it = usageCount.find(node->id());
		assert(it != usageCount.end());

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

		auto expression = shaderNode->GetExpression(expressions.data(), expressions.size());

		const std::string& variableName = shaderNode->GetVariableName();
		if (*it > 1 || !variableName.empty())
		{
			Nz::ShaderAst::ExpressionPtr varExpression;
			if (expression->GetExpressionCategory() == Nz::ShaderAst::ExpressionCategory::RValue)
			{
				std::string name;
				if (variableName.empty())
					name = "var" + std::to_string(varCount++);
				else
					name = variableName;

				if (usedVariableNames.find(name) != usedVariableNames.end())
					throw std::runtime_error("duplicate variable found: " + name);

				usedVariableNames.insert(name);

				auto variable = Nz::ShaderBuilder::Variable(std::move(name), expression->GetExpressionType());
				statements.emplace_back(Nz::ShaderBuilder::DeclareVariable(variable, expression));

				varExpression = variable;
			}
			else
				varExpression = expression;

			variableExpressions.insert(node->id(), varExpression);

			return varExpression;
		}
		else
			return expression;
	};

	m_flowScene.iterateOverNodes([&](QtNodes::Node* node)
	{
		if (node->nodeDataModel()->nPorts(QtNodes::PortType::Out) == 0)
		{
			statements.emplace_back(Nz::ShaderBuilder::ExprStatement(HandleNode(node)));
		}
	});

	return Nz::ShaderAst::StatementBlock::Build(std::move(statements));
}

void ShaderGraph::UpdateInput(std::size_t inputIndex, std::string name, InOutType type, InputRole role, std::size_t roleIndex)
{
	assert(inputIndex < m_inputs.size());
	auto& inputEntry = m_inputs[inputIndex];
	inputEntry.name = std::move(name);
	inputEntry.role = role;
	inputEntry.roleIndex = roleIndex;
	inputEntry.type = type;

	OnInputUpdate(this, inputIndex);
}

void ShaderGraph::UpdateOutput(std::size_t outputIndex, std::string name, InOutType type)
{
	assert(outputIndex < m_outputs.size());
	auto& outputEntry = m_outputs[outputIndex];
	outputEntry.name = std::move(name);
	outputEntry.type = type;

	OnOutputUpdate(this, outputIndex);
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
	RegisterShaderNode<CastToVec2>(*this, registry, "Casts");
	RegisterShaderNode<CastToVec3>(*this, registry, "Casts");
	RegisterShaderNode<CastToVec4>(*this, registry, "Casts");
	RegisterShaderNode<FloatValue>(*this, registry, "Constants");
	RegisterShaderNode<InputValue>(*this, registry, "Inputs");
	RegisterShaderNode<OutputValue>(*this, registry, "Outputs");
	RegisterShaderNode<SampleTexture>(*this, registry, "Texture");
	RegisterShaderNode<TextureValue>(*this, registry, "Texture");
	RegisterShaderNode<VecAdd>(*this, registry, "Vector operations");
	RegisterShaderNode<VecDiv>(*this, registry, "Vector operations");
	RegisterShaderNode<VecDot>(*this, registry, "Vector operations");
	RegisterShaderNode<VecFloatMul>(*this, registry, "Vector operations");
	RegisterShaderNode<VecMul>(*this, registry, "Vector operations");
	RegisterShaderNode<VecSub>(*this, registry, "Vector operations");
	RegisterShaderNode<Vec2Value>(*this, registry, "Constants");
	RegisterShaderNode<Vec3Value>(*this, registry, "Constants");
	RegisterShaderNode<Vec4Value>(*this, registry, "Constants");

	return registry;
}
