#include <ShaderNode/DataModels/OutputValue.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>
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

nzsl::Ast::NodePtr OutputValue::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 1);
	assert(outputIndex == 0);

	if (!m_currentOutputIndex)
		throw std::runtime_error("no output");

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	auto output = nzsl::ShaderBuilder::AccessMember(nzsl::ShaderBuilder::Identifier("output"), { outputEntry.name });

	using namespace Nz;
	return nzsl::ShaderBuilder::Assign(nzsl::Ast::AssignType::Simple, std::move(output), std::move(expressions[0]));
}

std::shared_ptr<QtNodes::NodeData> OutputValue::outData(QtNodes::PortIndex /*port*/)
{
	return {};
}

QtNodes::NodeDataType OutputValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	if (!m_currentOutputIndex)
		return VecData::Type();

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	return ShaderGraph::ToNodeDataType(outputEntry.type);
}

unsigned int OutputValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
		default: break;
	}

	assert(false);
	throw std::runtime_error("invalid port type");
}

QString OutputValue::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	if (!m_currentOutputIndex)
		return QString();

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	return QString::fromStdString(outputEntry.name);
}

bool OutputValue::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portIndex == 0);

	switch (portType)
	{
		case QtNodes::PortType::In: return m_currentOutputIndex.has_value();
		case QtNodes::PortType::Out: return false;

		default:
			break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

void OutputValue::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	if (!m_currentOutputIndex)
		return;

	assert(index == 0);
	m_input = std::move(value);

	UpdatePreview();
}

QtNodes::NodeValidationState OutputValue::validationState() const
{
	if (!m_currentOutputIndex || !m_input)
		return QtNodes::NodeValidationState::Error;

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	switch (outputEntry.type)
	{
		case PrimitiveType::Bool:
		case PrimitiveType::Float1:
		case PrimitiveType::Mat4x4:
			break;

		case PrimitiveType::Float2:
		case PrimitiveType::Float3:
		case PrimitiveType::Float4:
		{
			if (m_input->type().id != VecData::Type().id)
				return QtNodes::NodeValidationState::Error;

			assert(dynamic_cast<VecData*>(m_input.get()) != nullptr);
			const VecData& vec = static_cast<const VecData&>(*m_input);
			if (GetComponentCount(outputEntry.type) != vec.componentCount)
				return QtNodes::NodeValidationState::Error;
		}
	}

	return QtNodes::NodeValidationState::Valid;
}

QString OutputValue::validationMessage() const
{
	if (!m_currentOutputIndex)
		return "No output selected";

	if (!m_input)
		return "Missing input";

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	switch (outputEntry.type)
	{
		case PrimitiveType::Bool:
		case PrimitiveType::Float1:
		case PrimitiveType::Mat4x4:
			break;

		case PrimitiveType::Float2:
		case PrimitiveType::Float3:
		case PrimitiveType::Float4:
		{
			if (m_input->type().id != VecData::Type().id)
				return "Expected vector";

			assert(dynamic_cast<VecData*>(m_input.get()) != nullptr);
			const VecData& vec = static_cast<const VecData&>(*m_input);

			std::size_t outputComponentCount = GetComponentCount(outputEntry.type);

			if (outputComponentCount != vec.componentCount)
				return "Incompatible component count (expected " + QString::number(outputComponentCount) + ", got " + QString::number(vec.componentCount) + ")";
		}
	}

	return QString();
}

bool OutputValue::ComputePreview(QPixmap& pixmap)
{
	if (!m_input)
		return false;

	const auto& outputEntry = GetGraph().GetOutput(*m_currentOutputIndex);
	switch (outputEntry.type)
	{
		case PrimitiveType::Bool:
		{
			if (m_input->type().id != BoolData::Type().id)
				return false;

			assert(dynamic_cast<BoolData*>(m_input.get()) != nullptr);
			const BoolData& data = static_cast<const BoolData&>(*m_input);

			pixmap = QPixmap::fromImage(data.preview.GenerateImage());
			return true;
		}

		case PrimitiveType::Float1:
		{
			if (m_input->type().id != FloatData::Type().id)
				return false;

			assert(dynamic_cast<FloatData*>(m_input.get()) != nullptr);
			const FloatData& data = static_cast<const FloatData&>(*m_input);

			pixmap = QPixmap::fromImage(data.preview.GenerateImage());
			return true;
		}

		case PrimitiveType::Mat4x4:
		{
			//TODO
			/*assert(dynamic_cast<Matrix4Data*>(m_input.get()) != nullptr);
			const Matrix4Data& data = static_cast<const Matrix4Data&>(*m_input);*/

			return false;
		}

		case PrimitiveType::Float2:
		case PrimitiveType::Float3:
		case PrimitiveType::Float4:
		{
			if (m_input->type().id != VecData::Type().id)
				return false;

			assert(dynamic_cast<VecData*>(m_input.get()) != nullptr);
			const VecData& data = static_cast<const VecData&>(*m_input);

			pixmap = QPixmap::fromImage(data.preview.GenerateImage());
			return true;
		}
	}

	return false;
}

void OutputValue::OnOutputListUpdate()
{
	m_currentOutputIndex.reset();

	std::size_t outputIndex = 0;
	for (const auto& outputEntry : GetGraph().GetOutputs())
	{
		if (outputEntry.name == m_currentOutputText)
		{
			m_currentOutputIndex = outputIndex;
			break;
		}

		outputIndex++;
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
