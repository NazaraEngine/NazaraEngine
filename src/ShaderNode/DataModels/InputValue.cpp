#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/InputValue.hpp>
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
			UpdatePreview();
	});

	if (graph.GetInputCount() > 0)
	{
		auto& firstInput = graph.GetInput(0);
		m_currentInputIndex = 0;
		m_currentInputText = firstInput.name;
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

	pixmap = QPixmap::fromImage(preview.GetImage(inputEntry.role, inputEntry.roleIndex));
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

void InputValue::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QComboBox* inputSelection = new QComboBox;

	connect(inputSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentInputIndex = static_cast<std::size_t>(index);
		else
			m_currentInputIndex.reset();

		UpdatePreview();
	});

	for (const auto& inputEntry : GetGraph().GetInputs())
		inputSelection->addItem(QString::fromStdString(inputEntry.name));

	layout->addRow(tr("Input"), inputSelection);
}

Nz::ShaderAst::ExpressionPtr InputValue::GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	if (!m_currentInputIndex)
		throw std::runtime_error("no input");

	const auto& inputEntry = GetGraph().GetInput(*m_currentInputIndex);

	Nz::ShaderAst::ExpressionType expression = [&]
	{
		switch (inputEntry.type)
		{
			case InOutType::Bool:   return Nz::ShaderAst::ExpressionType::Boolean;
			case InOutType::Float1: return Nz::ShaderAst::ExpressionType::Float1;
			case InOutType::Float2: return Nz::ShaderAst::ExpressionType::Float2;
			case InOutType::Float3: return Nz::ShaderAst::ExpressionType::Float3;
			case InOutType::Float4: return Nz::ShaderAst::ExpressionType::Float4;
		}

		assert(false);
		throw std::runtime_error("Unhandled input type");
	}();

	return Nz::ShaderBuilder::Input(inputEntry.name, expression);
}

auto InputValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const -> QtNodes::NodeDataType
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex == 0);

	if (!m_currentInputIndex)
		return Vec4Data::Type();

	const auto& inputEntry = GetGraph().GetInput(*m_currentInputIndex);
	switch (inputEntry.type)
	{
		//case InputType::Bool:   return Nz::ShaderAst::ExpressionType::Boolean;
		//case InputType::Float1: return Nz::ShaderAst::ExpressionType::Float1;
		case InOutType::Float2: return Vec2Data::Type();
		case InOutType::Float3: return Vec3Data::Type();
		case InOutType::Float4: return Vec4Data::Type();
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

	auto vecData = std::make_shared<Vec2Data>();
	vecData->preview = preview.GetImage(inputEntry.role, inputEntry.roleIndex);

	return vecData;
}
