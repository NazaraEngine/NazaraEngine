#include <ShaderNode/DataModels/OutputValue.hpp>
#include <Nazara/Renderer/ShaderBuilder.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>

OutputValue::OutputValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	m_onOutputListUpdateSlot.Connect(GetGraph().OnOutputListUpdate, [&](ShaderGraph*) { OnOutputListUpdate(); });
	m_onOutputUpdateSlot.Connect(GetGraph().OnOutputUpdate, [&](ShaderGraph*, std::size_t inputIndex)
	{
		if (m_currentOutputIndex == inputIndex)
			UpdatePreview();
	});

	if (graph.GetOutputCount() > 0)
	{
		m_currentOutputIndex = 0;
		UpdateOutputText();
	}

	EnablePreview();
	SetPreviewSize({ 128, 128 });
	DisableCustomVariableName();
}

void OutputValue::BuildNodeEdition(QFormLayout* layout)
{
	ShaderNode::BuildNodeEdition(layout);

	QComboBox* outputSelection = new QComboBox;
	for (const auto& outputEntry : GetGraph().GetOutputs())
		outputSelection->addItem(QString::fromStdString(outputEntry.name));

	if (m_currentOutputIndex)
		outputSelection->setCurrentIndex(int(*m_currentOutputIndex));
	
	connect(outputSelection, qOverload<int>(&QComboBox::currentIndexChanged), [&](int index)
	{
		if (index >= 0)
			m_currentOutputIndex = static_cast<std::size_t>(index);
		else
			m_currentOutputIndex.reset();

		UpdateOutputText();
		UpdatePreview();
	});

	layout->addRow(tr("Output"), outputSelection);
}

Nz::ShaderNodes::ExpressionPtr OutputValue::GetExpression(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count) const
{
	using namespace Nz::ShaderBuilder;
	using namespace Nz::ShaderNodes;

	assert(count == 1);

	if (!m_currentOutputIndex)
		throw std::runtime_error("no output");

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);

	Nz::ShaderNodes::BasicType expression = [&]
	{
		switch (outputEntry.type)
		{
			case PrimitiveType::Bool:   return Nz::ShaderNodes::BasicType::Boolean;
			case PrimitiveType::Float1: return Nz::ShaderNodes::BasicType::Float1;
			case PrimitiveType::Float2: return Nz::ShaderNodes::BasicType::Float2;
			case PrimitiveType::Float3: return Nz::ShaderNodes::BasicType::Float3;
			case PrimitiveType::Float4: return Nz::ShaderNodes::BasicType::Float4;
		}

		assert(false);
		throw std::runtime_error("Unhandled output type");
	}();

	auto output = Nz::ShaderBuilder::Identifier(Nz::ShaderBuilder::Output(outputEntry.name, expression));

	return Nz::ShaderBuilder::Assign(std::move(output), *expressions);
}

QtNodes::NodeDataType OutputValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	if (!m_currentOutputIndex)
		return VecData::Type();

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	switch (outputEntry.type)
	{
		//case InOutType::Bool:   return Nz::ShaderNodes::BasicType::Boolean;
		//case InOutType::Float1: return Nz::ShaderNodes::BasicType::Float1;
		case PrimitiveType::Float2:
		case PrimitiveType::Float3:
		case PrimitiveType::Float4:
			return VecData::Type();
	}

	assert(false);
	throw std::runtime_error("Unhandled output type");
}

unsigned int OutputValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
	}

	return 0;
}

std::shared_ptr<QtNodes::NodeData> OutputValue::outData(QtNodes::PortIndex /*port*/)
{
	return {};
}

void OutputValue::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);
	if (value)
	{
		assert(dynamic_cast<VecData*>(value.get()) != nullptr);
		m_input = std::static_pointer_cast<VecData>(value);
	}
	else
		m_input.reset();

	UpdatePreview();
}

QtNodes::NodeValidationState OutputValue::validationState() const
{
	if (!m_currentOutputIndex || !m_input)
		return QtNodes::NodeValidationState::Error;

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	if (GetComponentCount(outputEntry.type) != m_input->componentCount)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString OutputValue::validationMessage() const
{
	if (!m_currentOutputIndex)
		return "No output selected";

	if (!m_input)
		return "Missing input";

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	std::size_t outputComponentCount = GetComponentCount(outputEntry.type);

	if (m_input->componentCount != outputComponentCount)
		return "Incompatible component count (expected " + QString::number(outputComponentCount) + ", got " + QString::number(m_input->componentCount) + ")";

	return QString();
}

bool OutputValue::ComputePreview(QPixmap& pixmap)
{
	if (!m_input)
		return false;

	pixmap = QPixmap::fromImage(m_input->preview.GenerateImage());
	return true;
}

void OutputValue::OnOutputListUpdate()
{
	m_currentOutputIndex.reset();

	std::size_t inputIndex = 0;
	for (const auto& inputEntry : GetGraph().GetOutputs())
	{
		if (inputEntry.name == m_currentOutputText)
		{
			m_currentOutputIndex = inputIndex;
			break;
		}

		inputIndex++;
	}
}

void OutputValue::UpdateOutputText()
{
	if (m_currentOutputIndex)
	{
		auto& output = GetGraph().GetOutput(*m_currentOutputIndex);
		m_currentOutputText = output.name;
	}
	else
		m_currentOutputText.clear();
}

void OutputValue::restore(const QJsonObject& data)
{
	m_currentOutputText = data["output"].toString().toStdString();
	OnOutputListUpdate();

	ShaderNode::restore(data);
}

QJsonObject OutputValue::save() const
{
	QJsonObject data = ShaderNode::save();
	data["output"] = QString::fromStdString(m_currentOutputText);

	return data;
}
