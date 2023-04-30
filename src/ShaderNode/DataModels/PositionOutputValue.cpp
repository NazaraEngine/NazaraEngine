#include <ShaderNode/DataModels/PositionOutputValue.hpp>
#include <NZSL/ShaderBuilder.hpp>
#include <ShaderNode/ShaderGraph.hpp>
#include <ShaderNode/DataTypes/BoolData.hpp>
#include <ShaderNode/DataTypes/FloatData.hpp>
#include <ShaderNode/DataTypes/Matrix4Data.hpp>
#include <ShaderNode/DataTypes/VecData.hpp>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>

PositionOutputValue::PositionOutputValue(ShaderGraph& graph) :
ShaderNode(graph)
{
	DisableCustomVariableName();
}

nzsl::Ast::NodePtr PositionOutputValue::BuildNode(nzsl::Ast::ExpressionPtr* expressions, std::size_t count, std::size_t outputIndex) const
{
	using namespace Nz;

	assert(count == 1);
	assert(outputIndex == 0);

	auto output = nzsl::ShaderBuilder::AccessMember(nzsl::ShaderBuilder::Identifier("output"), { "position" });
	return nzsl::ShaderBuilder::Assign(nzsl::Ast::AssignType::Simple, std::move(output), std::move(expressions[0]));
}

QtNodes::NodeDataType PositionOutputValue::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const
{
	assert(portType == QtNodes::PortType::In);
	assert(portIndex == 0);

	return VecData::Type();
}

unsigned int PositionOutputValue::nPorts(QtNodes::PortType portType) const
{
	switch (portType)
	{
		case QtNodes::PortType::None:
			break;

		case QtNodes::PortType::In: return 1;
		case QtNodes::PortType::Out: return 0;
	}

	return 0;
}

std::shared_ptr<QtNodes::NodeData> PositionOutputValue::outData(QtNodes::PortIndex /*port*/)
{
	return {};
}

void PositionOutputValue::setInData(std::shared_ptr<QtNodes::NodeData> value, int index)
{
	assert(index == 0);
	if (value && value->type().id == VecData::Type().id)
	{
		assert(dynamic_cast<VecData*>(value.get()) != nullptr);
		m_input = std::static_pointer_cast<VecData>(value);
	}
	else
		m_input.reset();
}

QtNodes::NodeValidationState PositionOutputValue::validationState() const
{
	if (!m_input)
		return QtNodes::NodeValidationState::Error;

	if (m_input->componentCount != 4)
		return QtNodes::NodeValidationState::Error;

	return QtNodes::NodeValidationState::Valid;
}

QString PositionOutputValue::validationMessage() const
{
	if (!m_input)
		return "Missing input";

	if (m_input->componentCount != 4)
		return QString("Expected vector with 4 components, got ") + QString::number(m_input->componentCount);

	return QString();
}
