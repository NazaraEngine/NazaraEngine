#include <ShaderNode/DataModels/PositionOutputValue.hpp>
#include <Nazara/Shader/ShaderBuilder.hpp>
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

Nz::ShaderNodes::ExpressionPtr PositionOutputValue::GetExpression(Nz::ShaderNodes::ExpressionPtr* expressions, std::size_t count) const
{
	using namespace Nz::ShaderBuilder;
	using namespace Nz::ShaderNodes;

	assert(count == 1);

	auto output = Nz::ShaderBuilder::Identifier(Nz::ShaderBuilder::Builtin(BuiltinEntry::VertexPosition));
	return Nz::ShaderBuilder::Assign(std::move(output), *expressions);
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
