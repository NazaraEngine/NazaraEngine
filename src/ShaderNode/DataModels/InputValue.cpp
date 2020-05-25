#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataModels/InputValue.hpp>
#include <ShaderNode/DataModels/VecValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>

InputValue::InputValue(ShaderGraph& graph) :
ShaderNode(graph),
m_currentInputIndex(0)
{
	m_layout = new QVBoxLayout;

	m_inputSelection = new QComboBox;
	m_inputSelection->setStyleSheet("background-color: rgba(255,255,255,255)");
	connect(m_inputSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index < 0)
			return;

		m_currentInputIndex = static_cast<std::size_t>(index);
		UpdatePreview();
	});

	m_layout->addWidget(m_inputSelection);

	m_previewLabel = new QLabel;

	m_layout->addWidget(m_previewLabel);

	m_widget = new QWidget;
	m_widget->setStyleSheet("background-color: rgba(0,0,0,0)");
	m_widget->setLayout(m_layout);

	m_onInputListUpdateSlot.Connect(GetGraph().OnInputListUpdate, [&](ShaderGraph*) { UpdateInputList(); });
	m_onInputUpdateSlot.Connect(GetGraph().OnInputUpdate, [&](ShaderGraph*, std::size_t inputIndex)
	{
		if (m_currentInputIndex == inputIndex)
			UpdatePreview();
	});

	UpdateInputList();
	UpdatePreview();
}

QWidget* InputValue::embeddedWidget()
{
	return m_widget;
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

void InputValue::UpdatePreview()
{
	if (m_inputSelection->count() == 0)
		return;

	const ShaderGraph& graph = GetGraph();
	const auto& inputEntry = graph.GetInput(m_currentInputIndex);
	const auto& preview = graph.GetPreviewModel();

	m_previewLabel->setPixmap(QPixmap::fromImage(preview.GetImage(inputEntry.role, inputEntry.roleIndex)));

	Q_EMIT dataUpdated(0);
}

void InputValue::UpdateInputList()
{
	QString currentInput = m_inputSelection->currentText();
	m_inputSelection->clear();

	for (const auto& inputEntry : GetGraph().GetInputs())
		m_inputSelection->addItem(QString::fromStdString(inputEntry.name));

	m_inputSelection->setCurrentText(currentInput);
}

Nz::ShaderAst::ExpressionPtr InputValue::GetExpression(Nz::ShaderAst::ExpressionPtr* /*expressions*/, std::size_t count) const
{
	assert(count == 0);

	const auto& inputEntry = GetGraph().GetInput(m_currentInputIndex);

	Nz::ShaderAst::ExpressionType expression = [&]
	{
		switch (inputEntry.type)
		{
			case InputType::Bool:   return Nz::ShaderAst::ExpressionType::Boolean;
			case InputType::Float1: return Nz::ShaderAst::ExpressionType::Float1;
			case InputType::Float2: return Nz::ShaderAst::ExpressionType::Float2;
			case InputType::Float3: return Nz::ShaderAst::ExpressionType::Float3;
			case InputType::Float4: return Nz::ShaderAst::ExpressionType::Float4;
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

	const auto& inputEntry = GetGraph().GetInput(m_currentInputIndex);
	switch (inputEntry.type)
	{
		//case InputType::Bool:   return Nz::ShaderAst::ExpressionType::Boolean;
		//case InputType::Float1: return Nz::ShaderAst::ExpressionType::Float1;
		case InputType::Float2: return Vec2Data::Type();
		case InputType::Float3: return Vec3Data::Type();
		case InputType::Float4: return Vec4Data::Type();
	}

	assert(false);
	throw std::runtime_error("Unhandled input type");
}

std::shared_ptr<QtNodes::NodeData> InputValue::outData(QtNodes::PortIndex port)
{
	assert(port == 0);

	const ShaderGraph& graph = GetGraph();
	const auto& inputEntry = graph.GetInput(m_currentInputIndex);
	const auto& preview = graph.GetPreviewModel();

	auto vecData = std::make_shared<Vec2Data>();
	vecData->preview = preview.GetImage(inputEntry.role, inputEntry.roleIndex);

	return vecData;
}
