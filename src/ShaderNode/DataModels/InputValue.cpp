#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/InputValue.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <QtWidgets/QFormLayout>

InputValue::InputValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onInputListUpdateSlot.Connect(GetGraph().OnInputListUpdate, [&](ShaderGraph*) { OnInputListUpdate(); });
	m_onInputUpdateSlot.Connect(GetGraph().OnInputUpdate, [&](ShaderGraph*, std::size_t inputIndex)
	{
		if (m_currentInputIndex == inputIndex)
		{
			UpdatePreview();
			Q_EMIT dataUpdated(0);
		}
	});

	if (graph.GetInputCount() > 0)
	{
		m_currentInputIndex = 0;
		UpdateInputText();
	}

	DisableCustomVariableName();
	UpdatePreview();
}

unsigned int InputValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:  return 0;
		case QtNodes::PortType::Out: return 1;
	}

	return 0;
}

bool InputValue::ComputePreview(QPixmap& pixmap)
{
	if (!m_currentInputIndex)
		return false;

	const ShaderGraph& graph = GetGraph();
	const auto& inputEntry = graph.GetInput(*m_currentInputIndex);
	const auto& preview = graph.GetPreviewModel();

	pixmap = QPixmap::fromImage(preview.GetPreview(inputEntry.role, inputEntry.roleIndex).GenerateImage());
	return true;
}

void InputValue::OnInputListUpdate()
{
	m_currentInputIndex.reset();

	std::size_t inputIndex = 0;
	for (const auto& inputEntry : GetGraph().GetInputs())
	{
		if (inputEntry.name == m_currentInputText)
		{
			m_currentInputIndex = inputIndex;
			break;
		}

		inputIndex++;
	}
}

void InputValue::UpdateInputText()
{
	if (m_currentInputIndex)
	{
		auto& input = GetGraph().GetInput(*m_currentInputIndex);
		m_currentInputText = input.name;
	}
	else
		m_currentInputText.clear();
}

void InputValue::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QComboBox* inputSelection = new QComboBox;
	for (const auto& inputEntry : GetGraph().GetInputs())
		inputSelection->addItem(QString::fromStdString(inputEntry.name));

	if (m_currentInputIndex)
		inputSelection->setCurrentIndex(int(*m_currentInputIndex));

	connect(inputSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentInputIndex = static_cast<std::size_t>(index);
		else
			m_currentInputIndex.reset();

		UpdateInputText();
		UpdatePreview();

		Q_EMIT dataUpdated(0);
	});

	layout->addRow(tr("Input"), inputSelection);
}

Nz::ShaderNodes::ExpressionPtr InputValue::GetExpression(Nz::ShaderNodes::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	if (!m_currentInputIndex)
		throw std::runtime_error("no input");

	const auto& inputEntry = GetGraph().GetInput(*m_currentInputIndex);

	Nz::ShaderNodes::ExpressionType expression = [&]
	{
		switch (inputEntry.type)
		{
			case InOutType::Bool:   return Nz::ShaderNodes::ExpressionType::Boolean;
			case InOutType::Float1: return Nz::ShaderNodes::ExpressionType::Float1;
			case InOutType::Float2: return Nz::ShaderNodes::ExpressionType::Float2;
			case InOutType::Float3: return Nz::ShaderNodes::ExpressionType::Float3;
			case InOutType::Float4: return Nz::ShaderNodes::ExpressionType::Float4;
		}

		assert(false);
		throw std::runtime_error("Unhandled input type");
	}();

	return Nz::ShaderBuilder::Identifier(Nz::ShaderBuilder::Input(inputEntry.name, expression));
}

auto InputValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	if (!m_currentInputIndex)
		return VecData::Type();

	const auto& inputEntry = GetGraph().GetInput(*m_currentInputIndex);
	switch (inputEntry.type)
	{
		//case InputType::Bool:   return Nz::ShaderNodes::ExpressionType::Boolean;
		case InOutType::Float1:
			return FloatData::Type();

		case InOutType::Float2:
		case InOutType::Float3:
		case InOutType::Float4:
			return VecData::Type();
	}

	assert(false);
	throw std::runtime_error("Unhandled input type");
}

std::shared_ptr<QtNodes::NodeData> InputValue::outData(QtNodes::PortIndex port)
{
	if (!m_currentInputIndex)
		return nullptr;

	assert(port == 0);

	const ShaderGraph& graph = GetGraph();
	const auto& inputEntry = graph.GetInput(*m_currentInputIndex);
	const auto& preview = graph.GetPreviewModel();

	auto vecData = std::make_shared<VecData>(GetComponentCount(inputEntry.type));
	vecData->preview = preview.GetPreview(inputEntry.role, inputEntry.roleIndex);

	return vecData;
}

QtNodes::NodeValidationState InputValue::validationState() const
{
	if (!m_currentInputIndex)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString InputValue::validationMessage() const
{
	if (!m_currentInputIndex)
		return "No input selected";

	return QString();
}

void InputValue::restore(const QJsonObject& data)
{
	m_currentInputText = data["input"].toString().toStdString();
	OnInputListUpdate();

	ShaderNode::restore(data);
}

QJsonObject InputValue::save() const
{
	QJsonObject data = ShaderNode::save();
	data["input"] = QString::fromStdString(m_currentInputText);

	return data;
}
