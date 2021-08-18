#include <ShaderNode/DataModels/VecDecomposition.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <limits>

VecDecomposition::VecDecomposition(ShaderGraph& graph) :
ShaderNode(graph)
{
	DisablePreview();
	DisableCustomVariableName();
}

Nz::ShaderAst::NodePtr VecDecomposition::BuildNode(Nz::ShaderAst::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	assert(count == 1);
	assert(outputIndex < m_outputs.size());

	using namespace Nz;

	ShaderAst::SwizzleComponent swizzleComponent = static_cast<ShaderAst::SwizzleComponent>(Nz::UnderlyingCast(ShaderAst::SwizzleComponent::First) + outputIndex);
	return ShaderBuilder::Swizzle(std::move(expressions[0]), { swizzleComponent });
}

QString VecDecomposition::caption() const
{
	return "Vector decomposition";
}

QString VecDecomposition::name() const
{
	return "vec_decompose";
}

QtNodes::NodeDataType VecDecomposition::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In:
		{
			assert(portIndex == 0);
			return VecData::Type();
		}

		case QtNodes::PortType::Out:
		{
			assert(portIndex >= 0 && portIndex < m_outputs.size());
			return FloatData::Type();
		}

		default:
			break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

unsigned int VecDecomposition::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return m_outputs.size();

		default:
			break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

QString VecDecomposition::portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::Out);
	assert(portIndex >= 0 && portIndex < s_vectorComponents.size());

	return QString(QChar(s_vectorComponents[portIndex]));
}

bool VecDecomposition::portCaptionVisible(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	switch (portType)
	{
		case QtNodes::PortType::In: return false;
		case QtNodes::PortType::Out: return true;

		default:
			break;
	}

	assert(false);
	throw std::runtime_error("Invalid port type");
}

std::shared_ptr<QtNodes::NodeData> VecDecomposition::outData(QtNodes::PortIndex port)
{
	if (!m_input)
		return {};

	return m_outputs[port];
}

void VecDecomposition::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);

	std::shared_ptr<VecData> castedValue;
	if (value && value->type().id == VecData::Type().id)
		castedValue = std::static_pointer_cast<VecData>(value);

	m_input = std::move(castedValue);

	UpdateOutputs();
}

QtNodes::NodeValidationState VecDecomposition::validationState() const
{
	if (!m_input)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString VecDecomposition::validationMessage() const
{
	if (!m_input)
		return "Missing input";

	return QString();
}

void VecDecomposition::UpdateOutputs()
{
	if (validationState() != QtNodes::NodeValidationState::Valid)
	{
		auto dummy = std::make_shared<FloatData>();
		dummy->preview = PreviewValues(1, 1);
		dummy->preview.Fill(Nz::Vector4f::Zero());

		m_outputs.fill(dummy);
		return;
	}

	std::size_t previewWidth = m_input->preview.GetWidth();
	std::size_t previewHeight = m_input->preview.GetHeight();
	std::size_t pixelCount = previewWidth * previewHeight;

	for (std::size_t i = 0; i < m_input->componentCount; ++i)
	{
		m_outputs[i] = std::make_shared<FloatData>();
		m_outputs[i]->preview = PreviewValues(previewWidth, previewHeight);

		const Nz::Vector4f* inputData = m_input->preview.GetData();
		Nz::Vector4f* outputData = m_outputs[i]->preview.GetData();
		for (std::size_t j = 0; j < pixelCount; ++j)
		{
			const Nz::Vector4f& input = *inputData++;

			*outputData++ = Nz::Vector4f(input[i], input[i], input[i], input[i]);
		}

		Q_EMIT dataUpdated(i);
	}

	for (std::size_t i = m_input->componentCount; i < m_outputs.size(); ++i)
		m_outputs[i] = nullptr;

	UpdatePreview();
}
